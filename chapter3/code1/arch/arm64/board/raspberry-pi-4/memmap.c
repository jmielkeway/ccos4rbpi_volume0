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

#include "config/config.h"
#include "arch/memory.h"
#include "arch/page.h"

#define END_OF_USABLE_SDRAM             MEMORY_SIZE
#define VC_SDRAM_SIZE                   0x8000000
#define START_OF_VC_SDRAM               (0x40000000 - (VC_SDRAM_SIZE))

#define SET_BY_ARCH_AT_INIT             (0)

static struct address_region memory_map[] = {
    {
        .start = 0x0,
        .size = SET_BY_ARCH_AT_INIT,
        .flags = MEM_FLAGS_CAKE_TEXT,
        .type = MEM_TYPE_SDRAM
    },
    {
        .start = SET_BY_ARCH_AT_INIT,
        .size = SET_BY_ARCH_AT_INIT,
        .flags = MEM_FLAGS_CAKE,
        .type = MEM_TYPE_SDRAM
    },
    {
        .start = SET_BY_ARCH_AT_INIT,
        .size = SET_BY_ARCH_AT_INIT,
        .flags = MEM_FLAGS_OVERWRITE,
        .type = MEM_TYPE_SDRAM
    },
    {
        .start = SET_BY_ARCH_AT_INIT,
        .size = SET_BY_ARCH_AT_INIT,
        .flags = MEM_FLAGS_BABY_BOOT,
        .type = MEM_TYPE_SDRAM
    },
    {
        .start = SET_BY_ARCH_AT_INIT,
        .size = START_OF_VC_SDRAM - SET_BY_ARCH_AT_INIT,
        .flags = MEM_FLAGS_NONE,
        .type = MEM_TYPE_SDRAM
    },
    {
        .start = START_OF_VC_SDRAM,
        .size =  VC_SDRAM_SIZE,
        .flags = MEM_FLAGS_NONE,
        .type = MEM_TYPE_RESERVED
    },
    {
        .start = 0x40000000,
        .size = END_OF_USABLE_SDRAM - 0x40000000,
        .flags = MEM_FLAGS_ENDMAP,
        .type = MEM_TYPE_SDRAM
    },
    {
        .start = END_OF_USABLE_SDRAM,
        .size = 0x400000000 - END_OF_USABLE_SDRAM,
        .flags = MEM_FLAGS_NONE,
        .type = MEM_TYPE_RESERVED
    },
    {
        .start = 0x400000000,
        .size = 0x40000000,
        .flags = MEM_FLAGS_NONE,
        .type = MEM_TYPE_CACHE
    },
    {
        .start = 0x440000000,
        .size = 0x47C000000 - 0x440000000,
        .flags = 0,
        .type = MEM_TYPE_RESERVED
    },
    {
        .start = 0x47C000000,
        .size = 0x480000000 - 0x47C000000,
        .flags = 0,
        .type = MEM_TYPE_PERIPHERAL
    },
    {
        .start = 0x480000000,
        .size = 0x4C0000000 - 0x480000000,
        .flags = 0,
        .type = MEM_TYPE_CACHE
    },
    {
        .start = 0x4C0000000,
        .size = 0x500000000 - 0x4C0000000,
        .flags = 0,
        .type = MEM_TYPE_LOCAL_PERIPHERAL
    },
    {
        .start = 0x500000000,
        .size = 0x600000000 - 0x500000000,
        .flags = 0,
        .type = MEM_TYPE_RESERVED
    },
    {
        .start = 0x600000000,
        .size = 0x800000000 - 0x600000000,
        .flags = 0,
        .type = MEM_TYPE_PCIE
    }
};

static struct address_map raspberry_pi_4_address_map = {
    .map = (struct address_region *) memory_map,
    .size = (sizeof(memory_map) / sizeof(struct address_region))
};

struct address_map *addrmap()
{
    extern unsigned long _kernel_text_end[];
    extern unsigned long _end_permenant_image[];
    extern unsigned long _end[];
    struct address_region addr_region_zero = {
        .start = 0x0,
        .size = (unsigned long) _kernel_text_end,
        .flags = MEM_FLAGS_CAKE_TEXT,
        .type = MEM_TYPE_SDRAM
    };
    struct address_region addr_region_one = {
        .start = (unsigned long) _kernel_text_end,
        .size = (unsigned long) _end_permenant_image - (unsigned long) _kernel_text_end,
        .flags = MEM_FLAGS_CAKE,
        .type = MEM_TYPE_SDRAM
    };
    struct address_region addr_region_two = {
        .start = (unsigned long) _end_permenant_image,
        .size = (unsigned long) _end - (unsigned long) _end_permenant_image,
        .flags = MEM_FLAGS_OVERWRITE,
        .type = MEM_TYPE_SDRAM
    };
    struct address_region addr_region_three = {
        .start = (unsigned long) _end,
        .size = SECTION_SIZE,
        .flags = MEM_FLAGS_BABY_BOOT,
        .type = MEM_TYPE_SDRAM
    };
    memory_map[0] = addr_region_zero;
    memory_map[1] = addr_region_one;
    memory_map[2] = addr_region_two;
    memory_map[3] = addr_region_three;
    memory_map[4].start = (unsigned long) _end + SECTION_SIZE;
    memory_map[4].size -= (((unsigned long) _end) + SECTION_SIZE);
    return &raspberry_pi_4_address_map;
}
