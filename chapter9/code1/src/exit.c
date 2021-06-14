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

#include "cake/log.h"
#include "cake/process.h"
#include "cake/schedule.h"
#include "arch/schedule.h"

long do_exit()
{
    struct process *current = CURRENT;
    current->state = PROCESS_STATE_ZOMBIE;
    log("Exited process %x\r\n", current->pid);
    schedule_self();
    return 0;
}
