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
#include "arch/bare-metal.h"
#include "arch/memory.h"
#include "arch/page.h"
#include "board/peripheral.h"

#define MEMORY_SIZE_1GB                 0x040000000

#define VC_SDRAM_SIZE                   0x3000000
#define START_OF_VC_SDRAM               MAIN_PERIPH_BASE - VC_SDRAM_SIZE
#define END_OF_USABLE_SDRAM             START_OF_VC_SDRAM

#define SET_BY_ARCH_AT_INIT             (0)

#define ADDRESS_REGION_CAKE_TEXT        (0)
#define ADDRESS_REGION_CAKE_DATA        (1)
#define ADDRESS_REGION_OVERWRITE        (2)
#define ADDRESS_REGION_BABY_BOOT        (3)
#define ADDRESS_REGION_DRAM_INIT        (4)

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
        .flags = MEM_FLAGS_ENDMAP,
        .type = MEM_TYPE_SDRAM
    },
    {
        .start = START_OF_VC_SDRAM,
        .size =  VC_SDRAM_SIZE,
        .flags = MEM_FLAGS_NONE,
        .type = MEM_TYPE_RESERVED
    },
    {
        .start = MAIN_PERIPH_BASE,
        .size =  MEMORY_SIZE_1GB - MAIN_PERIPH_BASE,
        .flags = MEM_FLAGS_NONE,
        .type = MEM_TYPE_PERIPHERAL
    },
    {
        .start = MEMORY_SIZE_1GB,
        .size = 0x40000,
        .flags = MEM_FLAGS_NONE,
        .type = MEM_TYPE_PERIPHERAL
    },
    {
        .start = MEMORY_SIZE_1GB + 0x40000,
        .size = 0x100000000 - MEMORY_SIZE_1GB - 0x40000,
        .flags = MEM_FLAGS_NONE,
        .type = MEM_TYPE_RESERVED
    }
};

static struct address_map raspberry_pi_3_address_map = {
    .map = (struct address_region *) memory_map,
    .size = (sizeof(memory_map) / sizeof(struct address_region))
};

static struct draminit raspberry_pi_3_draminit = {
    .start = 0x0,
    .end = END_OF_USABLE_SDRAM,
};

struct address_map *addrmap()
{
    extern unsigned long _kernel_text_end[];
    extern unsigned long _end_permenant_image[];
    extern unsigned long _end[];
    unsigned long kte = VIRT_TO_PHYS((unsigned long) _kernel_text_end);
    unsigned long epi = VIRT_TO_PHYS((unsigned long) _end_permenant_image);
    unsigned long end = VIRT_TO_PHYS((unsigned long) _end);
    struct address_region addr_region_zero = {
        .start = 0x0,
        .size = kte,
        .flags = MEM_FLAGS_CAKE_TEXT,
        .type = MEM_TYPE_SDRAM
    };
    struct address_region addr_region_one = {
        .start = kte,
        .size = epi - kte,
        .flags = MEM_FLAGS_CAKE,
        .type = MEM_TYPE_SDRAM
    };
    struct address_region addr_region_two = {
        .start = epi,
        .size = end - epi,
        .flags = MEM_FLAGS_OVERWRITE,
        .type = MEM_TYPE_SDRAM
    };
    struct address_region addr_region_three = {
        .start = end,
        .size = SECTION_SIZE,
        .flags = MEM_FLAGS_BABY_BOOT,
        .type = MEM_TYPE_SDRAM
    };
    memory_map[ADDRESS_REGION_CAKE_TEXT] = addr_region_zero;
    memory_map[ADDRESS_REGION_CAKE_DATA] = addr_region_one;
    memory_map[ADDRESS_REGION_OVERWRITE] = addr_region_two;
    memory_map[ADDRESS_REGION_BABY_BOOT] = addr_region_three;
    memory_map[ADDRESS_REGION_DRAM_INIT].start = end + SECTION_SIZE;
    memory_map[ADDRESS_REGION_DRAM_INIT].size -= end + SECTION_SIZE;
    return &raspberry_pi_3_address_map;
}

struct draminit *draminit()
{
    struct draminit *d = &raspberry_pi_3_draminit;
    d->block = memory_map[ADDRESS_REGION_DRAM_INIT].start;
    return d;
}
