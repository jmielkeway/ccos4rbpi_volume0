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

#ifndef _ARCH_PROT_H
#define _ARCH_PROT_H

#include "arch/bare-metal.h"
#include "arch/page.h"

#define PTE_VALID           BIT_SET(0)
#define PTE_DIRTY           BIT_SET(51)
#define PTE_WRITE           BIT_SET(55)
#define PTE_SPECIAL         BIT_SET(56)
#define PTE_USER            BIT_SET(6)
#define PTE_RDONLY          BIT_SET(7)
#define PTE_SHARED          BIT_SET(8) | BIT_SET(9)
#define PTE_AF              BIT_SET(10)
#define PTE_NG              BIT_SET(11)
#define PTE_DBM             PTE_DIRTY
#define PTE_CONT            BIT_SET(52)
#define PTE_PXN             BIT_SET(53)
#define PTE_UXN             BIT_SET(54)
#define PTE_TYPE_PAGE       PTE_VALID | BIT_SET(1)
#define PROT_PAGE_DEFUALT   (PTE_TYPE_PAGE | PTE_AF | PTE_SHARED)

#define PMD_TYPE_TABLE      PTE_VALID | BIT_SET(1)
#define PMD_TYPE_SECT       PTE_VALID | BIT_NOT_SET(1)
#define PMD_SECT_USER       PTE_USER
#define PMD_SECT_RDONLY     PTE_RDONLY
#define PMD_SECT_SHARED     PTE_SHARED
#define PMD_SECT_AF         PTE_AF
#define PMD_SECT_NG         PTE_NG
#define PMD_SECT_CONT       PTE_CONT
#define PMD_SECT_PXN        PTE_PXN
#define PMD_SECT_UXN        PTE_UXN
#define PROT_SECT_DEFAULT   (PMD_TYPE_SECT | PMD_SECT_AF | PMD_SECT_SHARED)

#define PROT_SECT_DEVICE_nGnRnE  (PROT_SECT_DEFAULT | \
                                  PMD_SECT_PXN | \
                                  PMD_SECT_UXN | \
                                  PAGE_TABLE_ATTR(MT_DEVICE_nGnRnE))

#define PROT_SECT_NORMAL         (PROT_SECT_DEFAULT | \
                                  PMD_SECT_PXN | \
                                  PMD_SECT_UXN | \
                                  PAGE_TABLE_ATTR(MT_NORMAL))

#define SECT_KERNEL         (PROT_SECT_NORMAL | PTE_DIRTY | PTE_WRITE)
#define SECT_KERNEL_ROX     ((PROT_SECT_NORMAL | PMD_SECT_RDONLY) & (~(PMD_SECT_PXN)))

#endif
