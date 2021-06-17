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

#ifndef _CAKE_WAIT_H
#define _CAKE_WAIT_H

#include "cake/list.h"
#include "cake/lock.h"
#include "cake/process.h"

struct waitqueue {
    struct list waitlist;
    struct spinlock lock;
};

struct wait {
    struct process *sleeping;
    struct list waitlist;
};

void dequeue_wait(struct waitqueue *waitqueue, struct wait *wait);
void enqueue_wait(struct waitqueue *waitqueue, struct wait *wait, unsigned int state);
void wake_waiter(struct waitqueue *waitqueue);

#endif
