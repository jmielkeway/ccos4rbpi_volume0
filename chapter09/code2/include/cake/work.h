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

#ifndef _CAKE_WORK_H
#define _CAKE_WORK_H

#include "cake/list.h"

struct work {
    struct list worklist;
    struct {
        unsigned long pending: 1;
        unsigned long reserved: 63;
    };
    void *data;
    void (*todo)(struct work *self);
};

void enqueue_work(struct work *work);
int perform_work(void *unused);

#endif
