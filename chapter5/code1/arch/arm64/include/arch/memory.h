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

#ifndef _ARCH_MEMORY_H
#define _ARCH_MEMORY_H

#include "arch/bare-metal.h"

#define MEM_TYPE_RESERVED               BIT_SET(0)
#define MEM_TYPE_SDRAM                  BIT_SET(1)
#define MEM_TYPE_CACHE                  BIT_SET(2)
#define MEM_TYPE_PERIPHERAL             BIT_SET(3)
#define MEM_TYPE_LOCAL_PERIPHERAL       BIT_SET(4)
#define MEM_TYPE_PCIE                   BIT_SET(5)

#define MEM_FLAGS_NONE                  (0)
#define MEM_FLAGS_CAKE                  BIT_SET(1)
#define MEM_FLAGS_CAKE_TEXT             BIT_SET(2)
#define MEM_FLAGS_OVERWRITE             BIT_SET(3)
#define MEM_FLAGS_BABY_BOOT             BIT_SET(4)
#define MEM_FLAGS_ENDMAP                BIT_SET(5)
#define MEM_FLAGS_TEXT                  BIT_SET(6)

struct address_region {
    unsigned long start;
    unsigned long size;
    unsigned long flags;
    unsigned long type;
};

struct address_map {
    struct address_region *map;
    unsigned long size;
};

#endif
