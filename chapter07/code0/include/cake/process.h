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

#ifndef _CAKE_PROCESS_H
#define _CAKE_PROCESS_H

#include "config/config.h"
#include "cake/cake.h"
#include "cake/list.h"
#include "cake/lock.h"

#define CPUMASK_SIZE                        BITMAP_SIZE(NUM_CPUS)
#define PROCESS_STATE_RUNNING               0b00000000
#define PROCESS_STATE_INTERRUPTIBLE         0b00000001
#define PROCESS_STATE_ZOMBIE                0b00000010
#define PROCESS_STATE_DEAD                  0b00000100

struct process {
    unsigned int state;
    unsigned int pid;
    unsigned long *stack;
    long preempt_count;
    struct list processlist;
    struct spinlock lock;
    unsigned long cpumask[CPUMASK_SIZE];
};

#endif
