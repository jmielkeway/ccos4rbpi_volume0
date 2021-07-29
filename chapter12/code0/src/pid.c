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
#include "cake/allocate.h"
#include "cake/bitops.h"
#include "cake/cake.h"
#include "cake/compiler.h"
#include "cake/lock.h"
#include "cake/process.h"
#include "arch/atomic.h"
#include "arch/barrier.h"
#include "arch/lock.h"
#include "arch/schedule.h"

#define PID_SHIFT       (9)
#define NUM_PIDS        ((1) << PID_SHIFT)
#define PID_MASK        ((NUM_PIDS) - 1)
#define PIDMAP_SIZE     (BITMAP_SIZE(NUM_PIDS))

extern void free_process(struct process *p);
extern void memset(void *dest, int c, unsigned long count);

struct pid {
    unsigned long refcount;
    struct process *process;
};

static unsigned long nextpid = 0;
static struct cache *pid_cache;
static struct spinlock pidlock = {
    .owner = 0,
    .ticket = 0
};
static unsigned long pidmap[PIDMAP_SIZE];
static struct pid *refmap[NUM_PIDS];

unsigned long allocate_pid(struct process *p)
{
    unsigned long pid;
    struct pid *pidref;
    ATOMIC_LONG_INC(&(p->refcount));
    do {
        do {
            pid = READ_ONCE(nextpid);
        } while(pid != CMPXCHG_RELAXED(&nextpid, pid, pid + 1));
        pid &= PID_MASK;
    } while(test_and_set_bit(pidmap, pid));
    pidref = alloc_obj(pid_cache);
    memset(pidref, 0, sizeof(*pidref));
    pidref->refcount = 1;
    pidref->process = p;
    SPIN_LOCK(&(pidlock));
    refmap[pid] = pidref;
    SPIN_UNLOCK(&(pidlock));
    return pid;
}

static void deallocate_pid(unsigned int pid, struct process *p)
{
    free_process(p);
    SMP_MB();
    clear_bit(pidmap, pid);
}

void pid_init()
{
    pid_cache = alloc_cache("pid", sizeof(struct pid));
    for(unsigned long i = 0; i < NUM_PIDS; i++) {
        refmap[i] = 0;
    }
    for(unsigned long i = 0; i < NUM_CPUS; i++) {
        set_bit(pidmap, i);
    }
}

struct process *pid_process(unsigned int pid)
{
    struct process *p;
    struct pid *reference;
    p = 0;
    SPIN_LOCK(&(pidlock));
    reference = refmap[pid];
    if(reference) {
        reference->refcount++;
        p = reference->process;
    }
    SPIN_UNLOCK(&(pidlock));
    return p;
}

void pid_put(unsigned int pid)
{
    int deallocate;
    struct process *p;
    struct pid *reference;
    deallocate = 0;
    SPIN_LOCK(&(pidlock));
    reference = refmap[pid];
    if(reference) {
        reference->refcount--;
        if(!(reference->refcount)) {
            p = reference->process;
            refmap[pid] = 0;
            cake_free(reference);
            deallocate = 1;
        }
    }
    SPIN_UNLOCK(&(pidlock));
    if(deallocate) {
        deallocate_pid(pid, p);
    }
}

int sys_getpid()
{
    return CURRENT->pid;
}
