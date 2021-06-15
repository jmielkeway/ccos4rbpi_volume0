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

#ifndef _CAKE_SCHEDULE_H
#define _CAKE_SCHEDULE_H

#include "cake/list.h"
#include "cake/lock.h"
#include "cake/process.h"
#include "arch/schedule.h"

#define CURRENT             ARCH_GET_CURRENT
#define PREEMPT_DISABLE()   CURRENT->preempt_count++
#define PREEMPT_ENABLE()    CURRENT->preempt_count--

struct runqueue {
    unsigned int switch_count;
    unsigned int weight;
    struct list queue;
    struct spinlock lock;
    struct process idle_task;
    struct process *current;
};

#endif
