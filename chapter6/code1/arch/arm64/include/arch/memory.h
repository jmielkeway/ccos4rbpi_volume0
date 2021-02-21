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

#define MEM_TYPE_RESERVED               SET_BIT(0)
#define MEM_TYPE_SDRAM                  SET_BIT(1)
#define MEM_TYPE_CACHE                  SET_BIT(2)
#define MEM_TYPE_PERIPHERAL             SET_BIT(3)
#define MEM_TYPE_LOCAL_PERIPHERAL       SET_BIT(4)
#define MEM_TYPE_PCIE                   SET_BIT(5)

#define MEM_FLAGS_NONE                  (0)
#define MEM_FLAGS_CAKE                  SET_BIT(1)
#define MEM_FLAGS_CAKE_TEXT             SET_BIT(2)
#define MEM_FLAGS_OVERWRITE             SET_BIT(3)
#define MEM_FLAGS_BABY_BOOT             SET_BIT(4)

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

struct draminit {
    unsigned long start;
    unsigned long end;
    unsigned long block;
};

#endif
