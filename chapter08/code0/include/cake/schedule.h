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

#include "cake/compiler.h"
#include "cake/list.h"
#include "cake/lock.h"
#include "cake/process.h"
#include "arch/schedule.h"

#define PREEMPT_DISABLE()       do { \
                                        CURRENT->preempt_count++; \
                                        BARRIER(); \
                                } while(0)
#define PREEMPT_ENABLE()        do { \
                                    BARRIER(); \
                                    CURRENT->preempt_count--; \
                                } while(0)
#define SET_CURRENT_STATE(v)    WRITE_ONCE(CURRENT->state, v)

struct runqueue {
    unsigned int switch_count;
    unsigned int weight;
    struct list queue;
    struct spinlock lock;
    struct process idle_task;
    struct process *current;
};

void runqueue_process(struct process *process);

#endif
