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

#ifndef _CAKE_USER_H
#define _CAKE_USER_H

#include "arch/user.h"
#include "arch/page.h"

static inline int outside_bounds(unsigned long cake, unsigned long user, unsigned long count)
{
   return ((user + count)) >= STACK_TOP || (cake < STACK_TOP);
}

static inline unsigned long copy_from_user(void *cake, void *user, unsigned long count) {
    if(outside_bounds((unsigned long) cake, (unsigned long) user, count)) {
        return count;
    }
    return COPY_FROM_USER(cake, user, count);
}

static inline unsigned long copy_to_user(void *user, void *cake, unsigned long count) {
    if(outside_bounds((unsigned long) cake, (unsigned long) user, count)) {
        return count;
    }
    return COPY_TO_USER(user, cake, count);
}

#endif
