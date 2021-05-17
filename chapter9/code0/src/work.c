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

#include "cake/process.h"
#include "cake/list.h"
#include "cake/lock.h"
#include "cake/wait.h"
#include "cake/work.h"
#include "arch/lock.h"
#include "arch/schedule.h"

struct workqueue {
    struct list worklist;
    struct spinlock lock;
    struct waitqueue waitqueue;
};

static struct workqueue workqueue = {
    .worklist = {
        .prev = &(workqueue.worklist),
        .next = &(workqueue.worklist)
    },
    .lock = {
        .owner = 0,
        .ticket = 0
    },
    .waitqueue = {
        .waitlist = {
            .prev = &(workqueue.waitqueue.waitlist),
            .next = &(workqueue.waitqueue.waitlist),
        },
        .lock = {
            .owner = 0,
            .ticket = 0
        }
    }
};

void enqueue_work(struct work *work)
{
    unsigned long flags;
    flags = SPIN_LOCK_IRQSAVE(&(workqueue.lock));
    if(!work->pending) {
        work->pending = 1; 
        list_enqueue(&(workqueue.worklist), &(work->worklist));
    }
    SPIN_UNLOCK_IRQRESTORE(&(workqueue.lock), flags);
    wake_waiter(&(workqueue.waitqueue));
}


int perform_work(void *unused)
{
    for(;;) {
        unsigned long flags;
        struct wait wait;
        wait.sleeping = CURRENT;
        wait.waitlist.prev = &(wait.waitlist);
        wait.waitlist.next = &(wait.waitlist);
        enqueue_wait(&(workqueue.waitqueue), &wait, PROCESS_STATE_INTERRUPTIBLE);
        if(list_empty(&(workqueue.worklist))) {
            schedule_self();
        }
        dequeue_wait(&(workqueue.waitqueue), &wait);
        flags = SPIN_LOCK_IRQSAVE(&(workqueue.lock));
        while(!list_empty(&(workqueue.worklist))) {
            struct work *w;
            w = LIST_ENTRY(workqueue.worklist.next, struct work, worklist);
            list_delete(&(w->worklist));
            w->pending = 0;
            SPIN_UNLOCK_IRQRESTORE(&(workqueue.lock), flags);
            (w->todo)(w);
            flags = SPIN_LOCK_IRQSAVE(&(workqueue.lock));
        }
        SPIN_UNLOCK_IRQRESTORE(&(workqueue.lock), flags);
    }
    return 0;
}
