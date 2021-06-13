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

#ifndef _ARCH_ATOMIC_H
#define _ARCH_ATOMIC_H

#define ATOMIC_LONG_ADD                 __atomic64_add
#define ATOMIC_LONG_ADD_RETURN          __atomic64_add_return
#define ATOMIC_LONG_ADD_RETURN_RELAXED  __atomic64_add_return_relaxed
#define ATOMIC_LONG_ANDNOT              __atomic64_andnot
#define ATOMIC_LONG_FETCH_ANDNOT        __atomic64_fetch_andnot
#define ATOMIC_LONG_FETCH_OR            __atomic64_fetch_or
#define ATOMIC_LONG_INC(var)            ATOMIC_LONG_ADD(var, 1)
#define ATOMIC_LONG_OR                  __atomic64_or
#define ATOMIC_LONG_SUB_RETURN          __atomic64_sub_return
#define CMPXCHG_RELAXED                 __cmpxchg_relaxed
#define XCHG_RELAXED                    __xchg_relaxed

void __atomic64_add(volatile unsigned long *initial, unsigned long count);
unsigned long __atomic64_add_return(volatile unsigned long *initial, unsigned long count);
unsigned long __atomic64_add_return_relaxed(volatile unsigned long *initial, 
    unsigned long count);
void __atomic64_andnot(volatile unsigned long *bitmap, unsigned long bit);
int __atomic64_fetch_andnot(volatile unsigned long *bitmap, unsigned long bit);
int __atomic64_fetch_or(volatile unsigned long *bitmap, unsigned long bit);
void __atomic64_or(volatile unsigned long *bitmap, unsigned long bit);
unsigned long __atomic64_sub_return(volatile unsigned long *initial, unsigned long count);
unsigned long __cmpxchg_relaxed(volatile void *ptr, unsigned long cmp, unsigned long xchg);
unsigned long __xchg_relaxed(volatile void *ptr, unsigned long xchg);
#endif
