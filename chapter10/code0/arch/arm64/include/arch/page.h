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

#ifndef _ARCH_PAGE_H
#define _ARCH_PAGE_H

#include "config/config.h"
#include "arch/bare-metal.h"

#define PAGE_TABLE_LEVELS           (4)
#define TABLE_SHIFT                 (9)
#define PMD_SHIFT                   (PAGE_SHIFT + (1 * TABLE_SHIFT))
#define PUD_SHIFT                   (PAGE_SHIFT + (2 * TABLE_SHIFT))
#define PGD_SHIFT                   (PAGE_SHIFT + (3 * TABLE_SHIFT))

#define NUM_ENTRIES_PER_TABLE       ((UL(1)) << (TABLE_SHIFT))
#define TABLE_INDEX_MASK            ((NUM_ENTRIES_PER_TABLE) - 1)
#define PAGE_SIZE                   ((UL(1)) << PAGE_SHIFT)
#define PAGE_MASK                   (~((PAGE_SIZE) - 1))
#define RAW_PAGE_TABLE_ADDR_MASK    (((UL(1) << VA_BITS) - 1) & (PAGE_MASK))
#define SECTION_SIZE                ((UL(1)) << (PMD_SHIFT))
#define SECTION_MASK                (~((SECTION_SIZE) - 1))

#define PAGE_TABLE_AF               BIT_SET(10)
#define PAGE_TABLE_BLOCK            BIT_SET(0)
#define PAGE_TABLE_TABLE            (0b11)
#define PAGE_TABLE_SH               (0b11 << 8)
#define PAGE_TABLE_ATTR(n)          (n << 2)

#define NORMAL_INIT_MMU_FLAGS       PAGE_TABLE_AF | \
                                    PAGE_TABLE_SH | \
                                    PAGE_TABLE_ATTR((MT_NORMAL)) | \
                                    PAGE_TABLE_BLOCK

#define DEVICE_INIT_MMU_FLAGS       PAGE_TABLE_AF | \
                                    PAGE_TABLE_ATTR((MT_DEVICE_nGnRnE)) | \
                                    PAGE_TABLE_BLOCK


#define MAIR(attr, mt)              ((attr) << ((mt) * 8))
#define MT_DEVICE_nGnRnE            (0)
#define MT_DEVICE_nGnRE             (1)
#define MT_DEVICE_GRE               (2)
#define MT_NORMAL_NC                (3)
#define MT_NORMAL                   (4)
#define MT_NORMAL_WT                (5)

#define MAIR_REGISTER_VALUE         MAIR(0b00000000, MT_DEVICE_nGnRnE) | \
                                    MAIR(0b00000100, MT_DEVICE_nGnRE) | \
                                    MAIR(0b00001100, MT_DEVICE_GRE) | \
                                    MAIR(0b01000100, MT_NORMAL_NC) | \
                                    MAIR(0b11111111, MT_NORMAL) | \
                                    MAIR(0b10111011, MT_NORMAL_WT)

#define TCR_T0SZ_SHIFT              (0)
#define TCR_T1SZ_SHIFT              (16)

#define TCR_T0SZ                    (((UL(64)) - (VA_BITS)) << (TCR_T0SZ_SHIFT))
#define TCR_IRGN0                   BIT_NOT_SET(9) | BIT_SET(8)
#define TCR_ORGN0                   BIT_NOT_SET(11) | BIT_SET(10)
#define TCR_SH0                     BIT_SET(13) | BIT_SET(12)
#define TCR_TG0                     BIT_NOT_SET(15) | BIT_NOT_SET(14)
#define TCR_T1SZ                    (((UL(64)) - (VA_BITS)) << (TCR_T1SZ_SHIFT))
#define TCR_A1                      BIT_SET(22)
#define TCR_IRGN1                   BIT_NOT_SET(25) | BIT_SET(24)
#define TCR_ORGN1                   BIT_NOT_SET(27) | BIT_SET(26)
#define TCR_SH1                     BIT_SET(29) | BIT_SET(28)
#define TCR_TG1                     BIT_SET(31) | BIT_NOT_SET(30)
#define TCR_IPS_48BIT               BIT_SET(34) | BIT_NOT_SET(33) | BIT_SET(32)
#define TCR_AS                      BIT_SET(36)

#define TCR_INIT_CONFIG             TCR_T0SZ | \
                                    TCR_IRGN0 | \
                                    TCR_ORGN0 | \
                                    TCR_SH0 | \
                                    TCR_TG0 | \
                                    TCR_T1SZ | \
                                    TCR_A1 | \
                                    TCR_IRGN1 | \
                                    TCR_ORGN1 | \
                                    TCR_SH1 | \
                                    TCR_TG1 | \
                                    TCR_IPS_48BIT | \
                                    TCR_AS

#define PFN_TO_PTR(pfn)            ((void *) (PHYS_TO_VIRT(pfn << (PAGE_SHIFT))))
#define PTR_TO_PFN(ptr)            (VIRT_TO_PHYS((unsigned long) (ptr)) >> (PAGE_SHIFT))

#define FIRST_USER_ADDRESS          (0)
#define STACK_TOP                   ((UL(1) << (VA_BITS)))
#define MAX_STACK_AREA              (SECTION_SIZE)

#endif
