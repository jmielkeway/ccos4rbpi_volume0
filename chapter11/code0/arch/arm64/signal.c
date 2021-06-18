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
#include "cake/signal.h"
#include "arch/process.h"
#include "arch/schedule.h"

void check_and_process_signals(struct stack_save_registers *ssr)
{
    struct process *current = CURRENT;
    struct signal *signal = (current->signal);
    if(*(signal->pending)) {
        log("PID %x found with signal %x\r\n", current->pid, LOG2_SAFE(*(signal->pending)));
        while(1) {
            current->state = PROCESS_STATE_STOPPED;
            schedule_self();
        }
    }
}
