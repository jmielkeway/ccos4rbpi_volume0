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

extern void do_exit(int errno);
extern void memset(void *dest, int c, unsigned long count);
extern struct process *pid_process(int pid);

static int send_signal(int signal, struct siginfo *info, struct process *p);
static void signal_clear(struct signal *signal, unsigned long signal_mask);
static void signal_parent_stop(struct process *p, unsigned long flags);

static struct cache *sigqueue_cache;

int dequeue_signal(struct process *p, unsigned long *blocked, struct siginfo *info)
{
    int signo;
    unsigned long x;
    struct sigqueue *q;
    unsigned long *pending = p->signal->pending; 
    struct list *l = &(p->signal->signallist);
    x = *pending &~ (*blocked); 
    if(!x) {
        return 0;
    }
    signo = __builtin_ctzl(x);
    LIST_FOR_EACH_ENTRY(q, l, list) {
        if(q->info.signo == signo) {
                break;
        }
    }
    memcpy(info, &(q->info), sizeof(*info));
    signal_clear(p->signal, SIGMASK(signo));
    return signo;
}

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

int get_signal(struct cakesignal *csig)
{
    int signo;
    struct sighandler *sighandler;
    struct sigaction *sigaction;
    struct process *current = CURRENT;
    struct signal *signal = current->signal;
start:
    SPIN_LOCK(&(signal->lock));
    if(signal->flags & SIGNAL_FLAGS_CONTINUED) {
        SPIN_UNLOCK(&(signal->lock));
        signal_parent_stop(current, SIGNAL_FLAGS_CONTINUED);
        goto start;
    }
    while(1) {
        signo = dequeue_signal(current, signal->blocked, &(csig->info));
        if(!signo) {
            break;
        }
        sighandler = &(signal->sighandler);
        sigaction = &(sighandler->sigaction[signo - 1]);
        if(sigaction->fn == SIG_IGN) {
            continue;
        }
        if(sigaction->fn != SIG_DFL) {
            csig->sigaction = *sigaction;
            break;
        }
        if(SIGMASK(signo) & (STOP_SIGNALS_MASK)) {
            signal->flags = SIGNAL_FLAGS_STOPPED;
            current->state = PROCESS_STATE_STOPPED;
            SPIN_UNLOCK(&(signal->lock));
            signal_parent_stop(current, SIGNAL_FLAGS_STOPPED);
            schedule_self();
            goto start;
        }
        current->signal->exitcode = signo;
        do_exit(1);
    }
    SPIN_UNLOCK(&(signal->lock));
    csig->signo = signo;
    return signo > 0;
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

void set_blocked_signals(struct process *p, unsigned long signal_mask)
{
    struct spinlock *lock = &(p->signal->sighandler.lock);
    SPIN_LOCK(lock);
    *(p->signal->blocked) = signal_mask;
    SPIN_UNLOCK(lock);
}

static void signal_clear(struct signal *signal, unsigned long signal_mask)
{
    struct sigqueue *q, *r;
    *(signal->pending) &= ~(signal_mask);
    LIST_FOR_EACH_ENTRY_SAFE(q, r, &(signal->signallist), list) {
        if(q->info.signo & signal_mask) {
            list_delete(&(q->list));
            cake_free(q);
        }
    }
}

void signal_done(struct cakesignal *csig)
{
    unsigned long blocked;
    struct process *current = CURRENT;
    blocked = *(current->signal->blocked);
    blocked |= SIGMASK(csig->signo);
    set_blocked_signals(current, blocked);
}

static void signal_parent_stop(struct process *p, unsigned long flags)
{
    struct process *parent;
    struct sighandler *sighandler;
    struct siginfo info;
    parent = p->parent;
    memset(&info, 0, sizeof(info));
    info.signo = SIGCHLD;
    info.error = 0;
    info.fields.child.child_pid = p->pid;
    info.code = flags;
    switch(flags) {
        case SIGNAL_FLAGS_STOPPED: 
            info.fields.child.status = SIGSTOP;
            break;
        case SIGNAL_FLAGS_CONTINUED: 
            info.fields.child.status = SIGCONT;
            break;
        default:
            break;
    }
    sighandler = &(parent->signal->sighandler);
    if(sighandler->sigaction[SIGCHLD - 1].fn != SIG_IGN) {
        send_signal(SIGCHLD, &info, parent);
    }
    wake_waiter(&(parent->signal->waitqueue));
}

void signal_init()
{
    sigqueue_cache = alloc_cache("sigqueue", sizeof(struct sigqueue));
}

int sys_sigaction(int signo, struct sigaction *sigaction, struct sigaction *unused)
{
    struct process *current = CURRENT;
    struct signal *signal = current->signal;
    struct sigaction *target = &(signal->sighandler.sigaction[signo - 1]);
    copy_from_user(target, sigaction, sizeof(*sigaction));
    return 0;
}

int sys_sigprocmask(unsigned long how, unsigned long *newset, unsigned long *oldset)
{
    unsigned long n, *o;
    struct process *current = CURRENT;
    if(newset) {
        copy_from_user(&n, newset, sizeof(*newset));
    }
    o = (current->signal->blocked);
    if(oldset) {
        copy_to_user(oldset, o, sizeof(*oldset));
    }
    switch(how) {
        case SIG_BLOCK:
            *o |= n;
            break;
        case SIG_UNBLOCK:
            *o &= (~n);
            break;
        case SIG_SETMASK:
            *o = n;
            break;
        default:
            return -1;
    }
    return 0;
}
