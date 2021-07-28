/*
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
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

#include "cake/allocate.h"
#include "cake/schedule.h"
#include "cake/signal.h"
#include "cake/user.h"
#include "arch/lock.h"
#include "user/signal.h"

#define SIGMASK(signal)     ((1UL) << signal)
#define STOP_SIGNALS_MASK   (SIGMASK(SIGSTOP) | \
                             SIGMASK(SIGTSTP) | \
                             SIGMASK(SIGTTIN) | \
                             SIGMASK(SIGTTOU))

extern void memset(void *dest, int c, unsigned long count);
extern struct process *pid_process(int pid);

static int send_signal(int signal, struct siginfo *info, struct process *p);
static void signal_clear(struct signal *signal, unsigned long signal_mask);

static struct cache *sigqueue_cache;

int do_kill(int pid, int signal) 
{
    struct siginfo *info = (struct siginfo *) SIGNAL_SEND_PRIVATE;
    struct process *p;
    p = pid_process(pid);
    if(p) {
        return send_signal(signal, info, p);
    }
    else {
        return -1;
    }
}

static int send_signal(int signo, struct siginfo *info, struct process *p)
{
    struct signal *signal = p->signal;
    struct sigqueue *q;
    SPIN_LOCK(&(signal->lock));
    if(SIGMASK(signo) & (STOP_SIGNALS_MASK)) {
        signal_clear(signal, SIGMASK(SIGCONT));
    }
    else if(SIGMASK(signo) & SIGMASK(SIGCONT)) {
        signal_clear(signal, (STOP_SIGNALS_MASK));
        if(signal->flags & SIGNAL_FLAGS_STOPPED) {
            signal->flags = SIGNAL_FLAGS_CONTINUED;
        }
        WRITE_ONCE(p->state, PROCESS_STATE_RUNNING);
        goto unlock;
    } 
    if(*(signal->pending) & SIGMASK(signo)) {
        goto unlock;
    }
    q = alloc_obj(sigqueue_cache);
    list_enqueue(&(signal->signallist), &(q->list));
    switch((unsigned long) info) {
        case SIGNAL_SEND_PRIVATE:
            memset(&q->info, 0, sizeof(q->info));
            q->info.signo = signo;
            q->info.error = 0;
            q->info.fields.kill.sender_pid = 0;
            break;
        default:
            memcpy(&(q->info), info, sizeof(*info));
            break;
    } 
    *(signal->pending) |= SIGMASK(signo);
    if(READ_ONCE(p->state) & PROCESS_STATE_INTERRUPTIBLE) {
        WRITE_ONCE(p->state, PROCESS_STATE_RUNNING);
    }
unlock:
    SPIN_UNLOCK(&(signal->lock));
    return 0;
}

static void signal_clear(struct signal *signal, unsigned long signal_mask)
{
    struct sigqueue *q, *r;
    *(signal->pending) &= ~(signal_mask);
    LIST_FOR_EACH_ENTRY_SAFE(q, r, &(signal->signallist), list) {
        if(SIGMASK(q->info.signo) & signal_mask) {
            list_delete(&(q->list));
            cake_free(q);
        }
    }
}

void signal_init()
{
    sigqueue_cache = alloc_cache("sigqueue", sizeof(struct sigqueue));
}
