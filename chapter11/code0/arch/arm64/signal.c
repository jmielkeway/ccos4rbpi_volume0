/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cake/cake.h"
#include "cake/error.h"
#include "cake/process.h"
#include "cake/signal.h"
#include "cake/user.h"
#include "arch/process.h"
#include "arch/schedule.h"

#define INSTRUCTION_SIZE    4

struct user_layout;

extern void memset(void *dest, int c, unsigned long count);

int get_signal(struct cakesignal *csig);
static void run_user_sighandler(struct stack_save_registers *ssr, struct cakesignal *csig);
static unsigned long signal_frame(struct user_layout *user, 
    struct stack_save_registers *ssr);

struct aarch64_ctx {
    u32 magic;
    u32 size;
};

struct user_sigcontext {
    u64 fault_address;
    u64 regs[31];
    u64 sp;
    u64 pc;
    u64 pstate;
    u8 reserved[4096] __attribute__((__aligned__(16)));
};

struct user_context {
    unsigned long uc_flags;
    struct user_context  *uc_link;
    unsigned long uc_stack;
    unsigned long uc_sigmask;
    u8 unused[(1024 / 8) - sizeof(unsigned long)];
    struct user_sigcontext uc_mcontext;
};

struct user_frame {
    struct siginfo info;
    struct user_context context;
};

struct user_record {
    u64 fp;
    u64 lr;
};

struct user_layout {
    struct user_frame *frame;
    struct user_record *record;
    unsigned long size;
    unsigned long limit;
    unsigned long end_offset;
};

void check_and_process_signals(struct stack_save_registers *ssr)
{
    unsigned long continue_addr, restart_addr, x0;
    struct cakesignal csig;
    struct process *current = CURRENT;
    struct signal *signal = (current->signal);
    unsigned long *pending = signal->pending;
    while(READ_ONCE(*pending) &~ READ_ONCE(*(signal->blocked))) {
        if(ssr->syscalln) {
            continue_addr = ssr->pc;
            restart_addr = continue_addr - INSTRUCTION_SIZE;
            x0 = ssr->regs[0];
            ssr->syscalln = 0;
            if(x0 == -ERESTARTSYS) {
                ssr->regs[0] = ssr->orig_x0;
                ssr->pc = restart_addr;
            }
        }
        if(get_signal(&csig)) {
            run_user_sighandler(ssr, &csig);
        }
    };
}

static void run_user_sighandler(struct stack_save_registers *ssr, struct cakesignal *csig)
{
    unsigned long sp;
    struct aarch64_ctx *aactx;
    struct user_layout user;
    struct user_frame *frame;
    struct process *current = CURRENT;
    unsigned long *blocked = current->signal->blocked;
    int signo = csig->signo;
    sp = signal_frame(&user, ssr);
    user.frame = (struct user_frame *) sp;
    frame = user.frame;
    frame->context.uc_flags = 0;
    frame->context.uc_link = 0;
    user.record->fp = ssr->regs[29];
    user.record->lr = ssr->regs[30];
    for(int i = 0; i < 31; i++) {
        frame->context.uc_mcontext.regs[i] = ssr->regs[i];
    }
    frame->context.uc_mcontext.sp = ssr->sp;
    frame->context.uc_mcontext.pc = ssr->pc;
    frame->context.uc_mcontext.pstate = ssr->pstate;
    copy_to_user(&frame->context.uc_sigmask, blocked, sizeof(*blocked));
    aactx = (struct aarch64_ctx *) (((unsigned long) frame) + user.end_offset);
    aactx->magic = 0;
    aactx->size = 0;
    ssr->regs[0] = signo;
    ssr->sp = (unsigned long) frame;
    ssr->regs[29] = (unsigned long) &(user.record->fp);
    ssr->pc = (unsigned long) csig->sigaction.fn;
    ssr->regs[30] = (unsigned long) csig->sigaction.restore;
    signal_done(csig);
}

static unsigned long signal_frame(struct user_layout *user, 
    struct stack_save_registers *ssr)
{
    unsigned long sp, reserved_size, padded_size;
    memset(user, 0, sizeof(*user));
    reserved_size = sizeof(user->frame->context.uc_mcontext.reserved);
    padded_size = ROUND_UP(sizeof(struct aarch64_ctx), 16);
    user->size = OFFSETOF(struct user_frame, context.uc_mcontext.reserved);
    user->limit = user->size + reserved_size;
    user->end_offset = user->size;
    user->size += padded_size;
    user->limit = user->size;
    sp = ssr->sp;
    sp = ROUND_DOWN(sp - sizeof(struct user_record), 16);
    user->record = (struct user_record *) sp;
    sp -= ROUND_UP(sizeof(struct user_frame), 16);
    return sp;
}

int sys_sigreturn()
{
    unsigned long blocked;
    struct process *current = CURRENT;
    struct stack_save_registers *ssr = PROCESS_STACK_SAVE_REGISTERS(current);
    struct user_frame *frame = (struct user_frame *) ssr->sp;
    blocked = frame->context.uc_sigmask;
    set_blocked_signals(current, blocked);
    for(int i = 0; i < 31; i++) {
        ssr->regs[i] = frame->context.uc_mcontext.regs[i];
    }
    ssr->sp = frame->context.uc_mcontext.sp;
    ssr->pc = frame->context.uc_mcontext.pc;
    ssr->pstate = frame->context.uc_mcontext.pstate;
    ssr->syscalln = 0;
    return ssr->regs[0];
}
