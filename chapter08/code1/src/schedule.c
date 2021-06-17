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
#include "cake/process.h"
#include "cake/schedule.h"
#include "cake/work.h"
#include "arch/irq.h"
#include "arch/lock.h"
#include "arch/schedule.h"
#include "arch/smp.h"

extern unsigned int allocate_pid(struct process *p);
extern void memset(void *dest, int c, unsigned long count);

static void finish_switch(struct process *prev);
static void idle_todo(struct work *self);
static struct process *schedule_next(struct runqueue *rq);
static struct runqueue *select_runqueue(unsigned long *cpumask, unsigned long threshold);

static struct runqueue runqueues[NUM_CPUS];

static inline int process_preemptable(struct process *current)
{
    int preemptable = current->preempt_count == 0;
    int countdown_complete = current->tick_countdown == 0;
    int running = current->state == PROCESS_STATE_RUNNING;
    return preemptable && countdown_complete && running;
}

static void context_switch(struct process *prev, struct process *next)
{
    prev = CONTEXT_SWITCH(prev, next);
    finish_switch(prev);
}

void do_idle()
{
    unsigned long cpuid;
    struct work work;
    cpuid = SMP_ID();
    work.todo = idle_todo;
    work.data = (void *) cpuid;
    while (1) {
        WAIT_FOR_INTERRUPT();
        WAIT_FOR_INTERRUPT();
        enqueue_work(&work);
        WAIT_FOR_INTERRUPT();
        WAIT_FOR_INTERRUPT();
    }
}

static void finish_switch(struct process *prev)
{
    unsigned long cpuid, priority, threshold;
    struct runqueue *this_rq, *new_rq;
    cpuid = SMP_ID();
    this_rq = &(runqueues[cpuid]);
    if(prev != &(this_rq->idle_task)) {
        priority = 1 << prev->priority;
        threshold = this_rq->weight - priority;
        new_rq = select_runqueue(prev->cpumask, threshold);
        if(new_rq) {
            this_rq->weight -= priority;
            list_delete(&(prev->processlist));
            SPIN_UNLOCK(&(this_rq->lock));
            SPIN_LOCK(&(new_rq->lock));
            list_add(&(new_rq->queue), &(prev->processlist));
            new_rq->weight += priority;
            SPIN_UNLOCK(&(new_rq->lock));
            goto unlocked;
        }
    }
    SPIN_UNLOCK(&(this_rq->lock));
unlocked:
    IRQ_ENABLE();
}

void idle_todo(struct work *self)
{
    log("CPU %x says hello!\r\n", self->data);
}

void runqueue_process(struct process *process)
{
    unsigned long flags;
    struct runqueue *rq = select_runqueue(process->cpumask, -1UL);
    flags = SPIN_LOCK_IRQSAVE(&(rq->lock));
    list_add(&(rq->queue), &(process->processlist));
    rq->weight += (1 << process->priority);
    SPIN_UNLOCK_IRQRESTORE(&(rq->lock), flags);
}

static void schedule()
{
    unsigned long cpuid;
    struct runqueue *rq;
    struct process *prev, *next;
    struct spinlock *rqlock;
    IRQ_DISABLE();
    cpuid = SMP_ID();
    rq = &(runqueues[cpuid]);
    rqlock = &(rq->lock);
    SPIN_LOCK(rqlock);
    prev = rq->current;
    next = schedule_next(rq);
    if(next != prev) {
        rq->switch_count++;
        rq->current = next;
        context_switch(prev, next);
    }
    else {
        SPIN_UNLOCK(rqlock);
        IRQ_ENABLE();
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
        p->priority = 0;
        p->tick_countdown = 0;
        p->runtime_counter = 0;
        p->stack = 0;
        p->preempt_count = 0;
        set_bit(p->cpumask, i);
        q->prev = q;
        q->next = q;
    }
    schedule_current();
}

static struct process *schedule_next(struct runqueue *rq)
{
    unsigned long n, c = -1;
    struct process *p, *next = 0;
    struct list *q = &(rq->queue);
    LIST_FOR_EACH_ENTRY(p, q, processlist) {
        if(!p->state) {
            n = p->runtime_counter >> p->priority;
            if(!next || c > n) {
                next = p;
                c = n;
            }
        }
    }
    if(!next) {
        next = &(rq->idle_task);
    }
    next->tick_countdown = (1 << next->priority);
    return next;
}

void schedule_self()
{
    PREEMPT_DISABLE();
    schedule();
    PREEMPT_ENABLE();
}

void schedule_tail(struct process *prev)
{
    finish_switch(prev);
    PREEMPT_ENABLE();
}

static struct runqueue *select_runqueue(unsigned long *cpumask, unsigned long threshold)
{
    struct runqueue *rq = 0;
    unsigned long min = threshold;
    for(unsigned long cpu = 0; cpu < NUM_CPUS; cpu++) {
        if(test_bit(cpumask, cpu)) {
            struct runqueue *compare = &(runqueues[cpu]);
            if(compare->weight < min) {
                rq = compare;
                min = compare->weight;
            }
        }
    }
    return rq;
}

void timer_tick()
{
    struct process *current = CURRENT;
    current->runtime_counter++;
    current->tick_countdown = current->tick_countdown <= 0 ? 0 : current->tick_countdown - 1;
    if(process_preemptable(current)) {
        PREEMPT_DISABLE();
        IRQ_ENABLE();
        schedule();
        IRQ_DISABLE();
        PREEMPT_ENABLE();
    }
}
