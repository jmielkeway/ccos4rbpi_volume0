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

#include "cake/list.h"
#include "cake/process.h"
#include "cake/schedule.h"
#include "cake/signal.h"
#include "cake/user.h"
#include "cake/wait.h"
#include "arch/barrier.h"
#include "arch/lock.h"
#include "user/wait.h"

extern void pid_put(int pid);

void dequeue_wait(struct waitqueue *waitqueue, struct wait *wait)
{
    SPIN_LOCK(&(waitqueue->lock));
    if(!list_empty(&(waitqueue->waitlist))) {
        list_delete(&(wait->waitlist));
    }
    SET_CURRENT_STATE(PROCESS_STATE_RUNNING);
    SPIN_UNLOCK(&(waitqueue->lock));
}

void enqueue_wait(struct waitqueue *waitqueue, struct wait *wait, unsigned int state)
{
    SPIN_LOCK(&(waitqueue->lock));
    if(list_empty(&(wait->waitlist))) {
        list_enqueue(&(waitqueue->waitlist), &(wait->waitlist));
    }
    SET_CURRENT_STATE(state);
    SMP_MB();
    SPIN_UNLOCK(&(waitqueue->lock));
}

int sys_waitpid(int pid, int *status, int options)
{
    int retval, s;
    struct process *current, *p;
    struct wait wait;
    if(pid != -1) {
        return -1;
    }
    current = CURRENT;
    wait.waitlist.prev = &(wait.waitlist);
    wait.waitlist.next = &(wait.waitlist);
    wait.sleeping = current;
    enqueue_wait(&(current->signal->waitqueue), &wait, PROCESS_STATE_INTERRUPTIBLE);
    retval = 0;
    s = 0;
    while(1) {
        LIST_FOR_EACH_ENTRY(p, &(current->childlist), siblinglist) {
            SPIN_LOCK(&(p->signal->lock));
            if(p->signal->flags & SIGNAL_FLAGS_STOPPED) {
                retval = p->pid;
                s |= WSTOPPED;
                p->signal->flags &= ~SIGNAL_FLAGS_STOPPED;
                goto unlock;
            }
            if(p->signal->flags & SIGNAL_FLAGS_CONTINUED) {
                retval = p->pid;
                s |= WCONTINUED;
                p->signal->flags &= ~SIGNAL_FLAGS_CONTINUED;
                goto unlock;
            }
            if(p->state == PROCESS_STATE_EXIT) {
                retval = p->pid;
                s |= WEXITED;
                s |= WEXITCODE(p->exitcode);
                SPIN_LOCK(&(current->lock));
                list_delete(&(p->siblinglist));
                SPIN_UNLOCK(&(current->lock));
                pid_put(retval);
                goto unlock;
            }
            SPIN_UNLOCK(&(p->signal->lock));
        }
        if(!retval && !(options & WNOHANG)) {
            schedule_self();
            continue;
        }
        goto out;
    }
unlock:
    SPIN_UNLOCK(&(p->signal->lock));
out:
    copy_to_user(status, &s, sizeof(*status));
    dequeue_wait(&(current->signal->waitqueue), &wait);
    return retval;
}

void wake_waiter(struct waitqueue *waitqueue) 
{
    struct wait *wait;
    SPIN_LOCK(&(waitqueue->lock));
    if(!list_empty(&(waitqueue->waitlist))) {
        wait = LIST_FIRST_ENTRY(&(waitqueue->waitlist), struct wait, waitlist);
        list_delete_reset(&(wait->waitlist));
        SMP_MB();
        WRITE_ONCE(wait->sleeping->state, PROCESS_STATE_RUNNING);
    }
    SPIN_UNLOCK(&(waitqueue->lock));
}
