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

#ifndef _ARCH_BARE_METAL_H
#define _ARCH_BARE_METAL_H

#include "config/config.h"

#if defined (__LINKER__) || defined (__ASSEMBLER__)
#define UL(x)                           (x)
#else
#define UL(x)                           (x##UL)
#endif

#define BIT_SET(pos)        ((UL(1)) << (pos))
#define BIT_NOT_SET(pos)    (0)

#define INIT_STACK_SHIFT    (3)
#define INIT_STACK_SIZE     (((UL(1)) << (PAGE_SHIFT)) << (INIT_STACK_SHIFT))

#define LINEAR_ADDR_MASK    ((UL(1) << (VA_BITS)) - 1)
#define VADDR_START         (~(LINEAR_ADDR_MASK))
#define VIRT_TO_PHYS(virt)  ((virt) & (LINEAR_ADDR_MASK))
#define PHYS_TO_VIRT(phys)  ((phys) | (VADDR_START))

#define MMU_M_FLAG          BIT_SET(0)
#define CACHE_C_FLAG        BIT_SET(2)
#define CACHE_I_FLAG        BIT_SET(12)

#endif
