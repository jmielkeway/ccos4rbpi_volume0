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
#include "cake/wait.h"
#include "arch/barrier.h"
#include "arch/lock.h"

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

void wake_waiter(struct waitqueue *waitqueue) 
{
    struct wait *wait;
    SPIN_LOCK(&(waitqueue->lock));
    if(!list_empty(&(waitqueue->waitlist))) {
        wait = LIST_FIRST_ENTRY(&(waitqueue->waitlist), struct wait, waitlist);
        list_delete(&(wait->waitlist));
        SMP_MB();
        WRITE_ONCE(wait->sleeping->state, PROCESS_STATE_RUNNING);
    }
    SPIN_UNLOCK(&(waitqueue->lock));
}
