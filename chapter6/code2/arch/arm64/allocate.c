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

#include "cake/allocate.h"
#include "cake/list.h"
#include "cake/log.h"
#include "cake/string.h"
#include "arch/asm-functions.h"
#include "arch/cache.h"
#include "arch/memory.h"
#include "arch/page.h"
#include "arch/prot.h"

#define BABY_BOOT_SIZE              NUM_ENTRIES_PER_TABLE
#define OVERWRITE_FREEBLOCK_SHIFT   (3)
#define OVERWRITE_FREEBLOCK_SIZE    ((PAGE_SIZE) << OVERWRITE_FREEBLOCK_SHIFT)

extern struct address_map *addrmap();
extern struct draminit *draminit();

static void linear_map_section(unsigned long start, unsigned long flags);

extern unsigned long page_global_dir[];
static unsigned long baby_boot_allocator[BABY_BOOT_SIZE];
static unsigned int baby_boot_pointer = 0;
static struct address_map *address_map;

unsigned long alloc_baby_boot_pages(unsigned int numpages)
{
    unsigned long addr = 0;
    if(baby_boot_pointer + (numpages - 1) < BABY_BOOT_SIZE) {
        addr = baby_boot_allocator[baby_boot_pointer];
        baby_boot_pointer += numpages;
    }
    return addr;
}

static void alloc_dram_pages(struct address_region *addrreg, 
    struct page *system_phys_page_dir,
    struct list *freelists,
    unsigned long firstfree)
{
    unsigned long start = addrreg->start;
    unsigned long end = start + addrreg->size;
    if((start & (SECTION_SIZE - 1)) || (end & (SECTION_SIZE -1))) {
        log("Unexpected memory region bounds\r\n");
    }
    while(start < end) {
        struct list *fp;
        struct page p;
        int allocated_for_page_dir;
        unsigned long startframe, endframe;
        unsigned int end_shift = MAX_ORDER + PAGE_SHIFT;
        while((start + (1 << end_shift)) > end) {
            end_shift--;
        }
        startframe = start >> PAGE_SHIFT;
        endframe = (start + (1 << end_shift)) >> PAGE_SHIFT;
        allocated_for_page_dir = start < firstfree ? 1 : 0;
        p.allocated = allocated_for_page_dir;
        p.reserved = allocated_for_page_dir;
        p.valid = 1;
        p.current_order = end_shift - PAGE_SHIFT;
        p.original_order = end_shift - PAGE_SHIFT;
        p.pfn = startframe;
        p.pagelist.next = 0;
        p.pagelist.prev = 0;
        system_phys_page_dir[startframe] = p;
        if(!allocated_for_page_dir) {
            fp = &(system_phys_page_dir[startframe].pagelist);
            list_enqueue(&(freelists[end_shift - PAGE_SHIFT]), fp);
        }
        for(unsigned int i = startframe + 1; i < endframe; i++) {
            p.valid = 0;
            p.current_order = 0;
            p.original_order = 0;
            p.pfn = i;
            system_phys_page_dir[i] = p;
        }
        start += (1 << end_shift);
    }
}

static void alloc_kernel_pages(struct address_region *addrreg,
    struct page *system_phys_page_dir,
    struct list *freelists)
{
    unsigned long start = addrreg->start;
    unsigned long end = start + addrreg->size;
    if((start & (SECTION_SIZE - 1)) || (end & (SECTION_SIZE - 1))) {
        log("Unexpected kernel bounds\r\n");        
    }
    while(start < end) {
        struct page p;
        unsigned long startframe = start >> PAGE_SHIFT;
        unsigned long endframe = (start + SECTION_SIZE) >> PAGE_SHIFT;
        p.allocated = 1;
        p.reserved = 1;
        p.valid = 1;
        p.current_order = 9;
        p.original_order = 9;
        p.pfn = startframe;
        p.pagelist.next = 0;
        p.pagelist.prev = 0;
        system_phys_page_dir[startframe] = p;
        for(unsigned int i = startframe + 1; i < endframe; i++) {
            p.valid = 0;
            p.current_order = 0;
            p.original_order = 0;
            p.pfn = i;
            system_phys_page_dir[i] = p;
        }
        start += SECTION_SIZE;
    }
}

static void alloc_overwrite_pages(struct address_region *addrreg,
    struct page *system_phys_page_dir,
    struct list *freelists)
{
    unsigned long start = addrreg->start;
    unsigned long end = start + addrreg->size;
    if((start & (SECTION_SIZE - 1)) || (end & (SECTION_SIZE -1))) {
        log("Unexpected overwrite bounds\r\n");
    }
    while(start < end) {
        struct list *fp;
        struct page p;
        unsigned long startframe = start >> PAGE_SHIFT;
        unsigned long endframe = (start + OVERWRITE_FREEBLOCK_SIZE) >> PAGE_SHIFT;
        p.allocated = 0;
        p.reserved = 0;
        p.valid = 1;
        p.current_order = OVERWRITE_FREEBLOCK_SHIFT;
        p.original_order = OVERWRITE_FREEBLOCK_SHIFT;
        p.pfn = startframe;
        p.pagelist.next = 0;
        p.pagelist.prev = 0;
        system_phys_page_dir[startframe] = p;
        fp = &(system_phys_page_dir[startframe].pagelist);
        list_enqueue(&(freelists[OVERWRITE_FREEBLOCK_SHIFT]), fp);
        for(unsigned int i = startframe + 1; i < endframe; i++) {
            p.valid = 0;
            p.current_order = 0;
            p.original_order = 0;
            p.pfn = i;
            system_phys_page_dir[i] = p;
        }
        start += OVERWRITE_FREEBLOCK_SIZE;
    }
}

static void alloc_unused_baby_boot_pages(struct address_region *addrreg,
    struct page *system_phys_page_dir,
    struct list *freelists)
{
    unsigned long start = addrreg->start;
    unsigned long end = start + addrreg->size;
    if((start & (SECTION_SIZE - 1)) || (end & (SECTION_SIZE -1))) {
        log("Unexpected baby boot bounds\r\n");
    }
    while(start < end) {
        struct page p;
        unsigned long frame = start >> PAGE_SHIFT;
        p.allocated = 1;
        p.reserved = 1;
        p. valid = 1;
        p.current_order = 0;
        p.original_order = 0;
        p.pfn = frame;
        p.pagelist.next = 0;
        p.pagelist.prev = 0;
        system_phys_page_dir[frame] = p;
        start += PAGE_SIZE;
    }
    while((start = alloc_baby_boot_pages(1)) != 0) {
        struct page *p;
        unsigned long frame = start >> PAGE_SHIFT;
        p = &(system_phys_page_dir[frame]);
        p->allocated = 0;
        p->reserved = 0;
        list_enqueue(&(freelists[0]), &(p->pagelist));
    }
}

void arch_populate_allocate_structures(struct page **system_phys_page_dir,
    struct list *freelists)
{
    unsigned long firstfree, physstart, numpages;
    struct address_region addrreg;
    struct draminit *d = draminit();
    physstart = d->block;
    numpages = d->end >> PAGE_SHIFT;
    firstfree = physstart + (numpages * sizeof(struct page));
    *system_phys_page_dir = (struct page *) PHYS_TO_VIRT(physstart);
    memset(*system_phys_page_dir, 0, firstfree - physstart);
    for(unsigned int i = 0; i < address_map->size; i++) {
        addrreg = address_map->map[i];
        if(addrreg.type == MEM_TYPE_SDRAM) {
            switch(addrreg.flags) {
                case MEM_FLAGS_CAKE_TEXT:
                case MEM_FLAGS_CAKE:
                    alloc_kernel_pages(&addrreg, *system_phys_page_dir, freelists);
                    break;
                case MEM_FLAGS_OVERWRITE:
                    alloc_overwrite_pages(&addrreg, *system_phys_page_dir, freelists);
                    break;
                case MEM_FLAGS_BABY_BOOT:
                    alloc_unused_baby_boot_pages(&addrreg, *system_phys_page_dir, freelists);
                    break;
                default:
                    alloc_dram_pages(&addrreg, *system_phys_page_dir, freelists, firstfree);
                    break;
            }
        }
    }
}

static void initialize_baby_boot_allocator(unsigned long start, unsigned long end)
{
    unsigned int pmd_index;
    unsigned long *page_upper_dir, *page_middle_dir, *target_addr, next_addr = start;
    for(unsigned int i = 0; i < BABY_BOOT_SIZE; i++) {
        baby_boot_allocator[i] = next_addr;
        next_addr += PAGE_SIZE;
    }
    page_upper_dir = (unsigned long *) PHYS_TO_VIRT((*page_global_dir & (PAGE_MASK)));
    page_middle_dir = (unsigned long *) PHYS_TO_VIRT((*page_upper_dir & (PAGE_MASK)));
    pmd_index = (start >> (PMD_SHIFT)) & (TABLE_INDEX_MASK);
    target_addr = page_middle_dir + pmd_index;
    for(unsigned long i = start; i < end; i += SECTION_SIZE) {
        *(target_addr++) = (i | NORMAL_INIT_MMU_FLAGS);
    }
    __dsb_sy();
    memset((void *) PHYS_TO_VIRT(start), 0, end - start);
}

static unsigned long linear_map_prot_flags(unsigned long memtype, unsigned long flags)
{
    unsigned long prot = 0;
    switch(memtype) {
        case MEM_TYPE_SDRAM:
            switch(flags) {
                case MEM_FLAGS_CAKE_TEXT:
                    prot = SECT_KERNEL_ROX; 
                    break;
                default:
                    prot = SECT_KERNEL;
                    break;
            }
            break;
        default:
            prot = PROT_SECT_DEVICE_nGnRnE;
            break;
    }
    return prot;
}

static void linear_map_region(struct address_region *addrreg)
{
    unsigned long flags, start_addr, end_addr;
    if(addrreg->type != MEM_TYPE_RESERVED) {
        flags = linear_map_prot_flags(addrreg->type, addrreg->flags);
        start_addr = addrreg->start;
        end_addr = start_addr + addrreg->size;
        for(unsigned long i = start_addr; i < end_addr; i += SECTION_SIZE) {
            linear_map_section(i, flags);
        }
    }
}

static void linear_map_section(unsigned long start, unsigned long flags)
{
    unsigned int pgd_index, pud_index, pmd_index;
    unsigned long pgd_raw_entry;
    unsigned long pud_phys_addr, *pud_virt_addr, pud_raw_entry;
    unsigned long pmd_phys_addr, *pmd_virt_addr;
    unsigned long baby_boot_addr;
    pgd_index = (start >> PGD_SHIFT) & (TABLE_INDEX_MASK);
    pgd_raw_entry = *(page_global_dir + pgd_index);
    pud_phys_addr = pgd_raw_entry & (RAW_PAGE_TABLE_ADDR_MASK);
    pud_virt_addr = (unsigned long *) PHYS_TO_VIRT(pud_phys_addr);
    pud_index = (start >> PUD_SHIFT) & (TABLE_INDEX_MASK);
    pud_raw_entry = *(pud_virt_addr + pud_index);
    pmd_phys_addr = pud_raw_entry & (RAW_PAGE_TABLE_ADDR_MASK);
    pmd_virt_addr = (unsigned long *) PHYS_TO_VIRT(pmd_phys_addr);
    if(!pmd_phys_addr) {
        baby_boot_addr = alloc_baby_boot_pages(1);
        *(pud_virt_addr + pud_index) = (baby_boot_addr | PAGE_TABLE_TABLE);
        pmd_virt_addr = (unsigned long *) PHYS_TO_VIRT(baby_boot_addr);
    }
    pmd_index = (start >> PMD_SHIFT) & (TABLE_INDEX_MASK);
    *(pmd_virt_addr + pmd_index) = (start | flags);
    __tlbi_vmalle1();
}

void paging_init()
{
    address_map = addrmap();
    struct address_region addrreg;
    for(unsigned int i = 0; i < address_map->size; i++) {
        addrreg = address_map->map[i];
        if(addrreg.flags & MEM_FLAGS_BABY_BOOT) {
            initialize_baby_boot_allocator(addrreg.start, addrreg.start + addrreg.size);
            break;
        }
    }
    for(unsigned int i = 0; i < address_map->size; i++) {
        addrreg = address_map->map[i];
        linear_map_region(&addrreg);
    }
}
