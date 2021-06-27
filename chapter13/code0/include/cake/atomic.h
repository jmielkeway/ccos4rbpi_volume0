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

#ifndef _CAKE_ATOMIC_H
#define _CAKE_ATOMIC_H

#include "arch/atomic.h"

static inline int atomic_inc_and_test(volatile unsigned long *var)
{
    return ATOMIC_LONG_ADD_RETURN(var, 1) == 0;
}

static inline int atomic_dec_and_test(volatile unsigned long *var)
{
    return ATOMIC_LONG_SUB_RETURN(var, 1) == 0;
}

#endif
