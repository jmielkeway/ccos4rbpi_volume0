/*
 *
 * Copyright (C) 1995  Linus Torvalds
 * Copyright (C) 1996-2000 Russell King - Converted to ARM.
 * Copyright (C) 2012 ARM Ltd.
 *
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

#include "config/config.h"
#include "cake/bitops.h"
#include "cake/fork.h"
#include "cake/process.h"
#include "arch/process.h"
#include "arch/schedule.h"
#include "user/fork.h"

extern void __ret_from_fork();
extern void memset(void *dest, int c, unsigned long count);

int copy_arch_context(unsigned long flags, 
    unsigned long thread_input, 
    unsigned long arg,
    struct process *p)
{
    struct stack_save_registers *ssr = PROCESS_STACK_SAVE_REGISTERS(p);
    memset(&(p->context), 0, sizeof(struct cpu_context));
    p->preempt_count = FORK_PREEMPT_COUNT;
    p->priority = CLONE_PRIORITY(flags);
    if(flags & CLONE_CAKETHREAD) {
        memset(ssr, 0, sizeof(*ssr));
        p->context.x19 = thread_input;
        p->context.x20 = arg;
        bitmap_zero(p->cpumask, NUM_CPUS);
        set_bit(p->cpumask, 0);
    }
    else {
        *ssr = *PROCESS_STACK_SAVE_REGISTERS(CURRENT);
        ssr->regs[0] = 0;
        if(arg) {
            ssr->sp = arg;
        }
        bitmap_fill(p->cpumask, NUM_CPUS);
        clear_bit(p->cpumask, 0);
    }
    p->context.pc = (unsigned long) __ret_from_fork;
    p->context.sp = (unsigned long) ssr;
    return 0;
}
