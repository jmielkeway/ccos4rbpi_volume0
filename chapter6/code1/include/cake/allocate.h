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

#ifndef _CAKE_ALLOCATE_H
#define _CAKE_ALLOCATE_H

#include "cake/list.h"

#define MAX_ORDER   9

struct page {
    unsigned long allocated: 1;
    unsigned long reserved: 1;
    unsigned long valid: 1;
    unsigned long current_order: 4;
    unsigned long original_order: 4;
    unsigned long pfn: 53;
    struct list pagelist;
    unsigned long refcount;
};

#endif
