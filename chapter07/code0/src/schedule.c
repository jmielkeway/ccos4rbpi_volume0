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

#include "config/config.h"
#include "cake/bitops.h"
#include "cake/lock.h"
#include "cake/log.h"
#include "cake/schedule.h"
#include "arch/irq.h"
#include "arch/lock.h"
#include "arch/schedule.h"
#include "arch/smp.h"

extern unsigned int allocate_pid(struct process *p);

static unsigned long count;
static struct spinlock idle_lock = {
    .owner = 0,
    .ticket = 0
};
static struct runqueue runqueues[NUM_CPUS];

void do_idle()
{
    unsigned long cpuid = SMP_ID();
    while (1) {
        unsigned long flags = SPIN_LOCK_IRQSAVE(&idle_lock);
        log("CPU %x says hello from PID %x!\r\n", cpuid, CURRENT->pid);
        log("Message count: %x\r\n", count++);
        log("\r\n");
        SPIN_UNLOCK_IRQRESTORE(&idle_lock, flags);
        WAIT_FOR_INTERRUPT();
    }
}

void schedule_current()
{
    unsigned long cpuid = SMP_ID();
    struct runqueue *rq = &(runqueues[cpuid]);
    struct process *p = rq->current;
    SCHEDULE_CURRENT(p);
}

void schedule_init()
{
    struct runqueue *rq;
    struct process *p;
    struct list *q;
    for(unsigned int i = 0; i < NUM_CPUS; i++) {
        rq = &(runqueues[i]);
        p = &(rq->idle_task);
        q = &(rq->queue);
        rq->switch_count = 0;
        rq->weight = 0;
        rq->current = p;
        p->state = PROCESS_STATE_RUNNING;
        p->pid = allocate_pid(p);
        p->stack = 0;
        p->preempt_count = 0;
        set_bit(p->cpumask, i);
        q->prev = q;
        q->next = q;
    }
}
