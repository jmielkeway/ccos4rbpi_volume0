/*
 *
 * Copyright (C) 2012 ARM Ltd.
 *
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

#ifndef _ARCH_BARRIER_H
#define _ARCH_BARRIER_H

#define sev()                       asm volatile("sev" : : : "memory")
#define wfe()                       asm volatile("wfe" : : : "memory")
#define wfi()                       asm volatile("wfi" : : : "memory")

#define isb()                       asm volatile("isb" : : : "memory")
#define dmb(opt)                    asm volatile("dmb " #opt : : : "memory")
#define dsb(opt)                    asm volatile("dsb " #opt : : : "memory")

#define sys_mb()                    dsb(sy)
#define sys_rmb()                   dsb(ld)
#define sys_wmb()                   dsb(st)

#define smp_mb()                    dmb(ish)
#define smp_rmb()                   dmb(ishld)
#define smp_wmb()                   dmb(ishst)

#define LOAD_ACQUIRE(ptr)           __load_acquire(ptr)
#define STORE_RELEASE(ptr, val)     __store_release(ptr, val)

unsigned long __load_acquire(void *src)
void __store_release(void *dest, unsigned long val);

#endif

