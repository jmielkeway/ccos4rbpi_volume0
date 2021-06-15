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

#include "cake/bitops.h"
#include "cake/cake.h"
#include "cake/compiler.h"
#include "cake/process.h"
#include "arch/atomic.h"
#include "arch/barrier.h"

#define PID_SHIFT       (9)
#define NUM_PIDS        ((1) << PID_SHIFT)
#define PID_MASK        ((NUM_PIDS) - 1)
#define PIDMAP_SIZE     (BITMAP_SIZE(NUM_PIDS))

unsigned long nextpid = 0;
unsigned long pidmap[PIDMAP_SIZE];
struct process *procmap[NUM_PIDS];

unsigned long allocate_pid(struct process *p)
{
    unsigned long pid;
    do {
        do {
            pid = READ_ONCE(nextpid);
        } while(pid != CMPXCHG_RELAXED(&nextpid, pid, pid + 1));
        pid &= PID_MASK;
    } while(test_and_set_bit(pidmap, pid));
    WRITE_ONCE(procmap[pid], p);
    return pid;
}

void deallocate_pid(unsigned int pid)
{
    WRITE_ONCE(procmap[pid], 0);
    SMP_MB();
    clear_bit(pidmap, pid);
}

struct process *pid_process(unsigned int pid)
{
    return procmap[pid];
}
