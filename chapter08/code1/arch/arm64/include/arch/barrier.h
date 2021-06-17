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

#ifndef _ARCH_BARRIER_H
#define _ARCH_BARRIER_H

#define SEV()                       asm volatile("sev" : : : "memory")
#define WFE()                       asm volatile("wfe" : : : "memory")
#define WFI()                       asm volatile("wfi" : : : "memory")

#define ISB()                       asm volatile("isb" : : : "memory")
#define DMB(opt)                    asm volatile("dmb " #opt : : : "memory")
#define DSB(opt)                    asm volatile("dsb " #opt : : : "memory")

#define SYS_MB()                    DSB(sy)
#define SYS_RMB()                   DSB(ld)
#define SYS_WMB()                   DSB(st)

#define SMP_MB()                    DMB(ish)
#define SMP_RMB()                   DMB(ishld)
#define SMP_WMB()                   DMB(ishst)

#define LOAD_ACQUIRE(ptr)           __load_acquire(ptr)
#define STORE_RELEASE(ptr, val)     __store_release(ptr, val)

unsigned long __load_acquire(void *src);
void __store_release(void *dest, unsigned long val);

#endif

