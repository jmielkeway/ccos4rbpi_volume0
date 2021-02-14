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

#define SET_BIT(pos)        ((UL(1)) << (pos))
#define INIT_STACK_SIZE     (((UL(1)) << (PAGE_SHIFT)) << 3)

#define LINEAR_ADDR_MASK    ((UL(1) << (VA_BITS)) - 1)
#define VADDR_START         (~(LINEAR_ADDR_MASK))
#define VIRT_TO_PHYS(virt)  ((virt) & (LINEAR_ADDR_MASK))
#define PHYS_TO_VIRT(phys)  ((phys) | (VADDR_START))

#endif