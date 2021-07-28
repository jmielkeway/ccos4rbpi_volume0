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
#include "cake/schedule.h"
#include "cake/signal.h"
#include "cake/vm.h"
#include "arch/atomic.h"
#include "arch/lock.h"
#include "arch/schedule.h"

extern void put_memmap(struct memmap *mm);
extern void signal_parent_exit(struct process *p);

void do_exit(int code)
{
    struct process *current = CURRENT;
    struct memmap *mm = current->memmap;
    ATOMIC_LONG_INC(&(mm->refcount));
    SPIN_LOCK(&(current->signal->lock));
    current->memmap = 0;
    current->exitcode = code;
    current->state = PROCESS_STATE_EXIT;
    current->signal->flags = SIGNAL_FLAGS_EXITED;
    SPIN_UNLOCK(&(current->signal->lock));
    put_memmap(mm);
    signal_parent_exit(current);
    schedule_self();
}

void sys_exit(int code)
{
    do_exit(code);
}
