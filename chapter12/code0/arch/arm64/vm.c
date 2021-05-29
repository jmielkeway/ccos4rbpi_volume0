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
#include "cake/allocate.h"
#include "cake/bitops.h"
#include "cake/cake.h"
#include "cake/compiler.h"
#include "cake/lock.h"
#include "cake/schedule.h"
#include "cake/vm.h"
#include "arch/atomic.h"
#include "arch/barrier.h"
#include "arch/lock.h"
#include "arch/page.h"
#include "arch/prot.h"
#include "arch/smp.h"
#include "arch/vm.h"

#define ASID_BITS           (16)
#define ASID_MASK           (((1UL) << (ASID_BITS)) - 1)
#define ASID_FIRST_VERSION  ((1UL) << (ASID_BITS))
#define NUM_USER_ASIDS      (ASID_FIRST_VERSION)
#define ASID2IDX(asid)      ((asid) & ASID_MASK)
#define IDX2ASID(idx)       ASID2IDX(idx)
#define ASID_BITMAP_SIZE    BITMAP_SIZE(NUM_USER_ASIDS)
#define FLUSH_BITMAP_SIZE   BITMAP_SIZE(NUM_CPUS)

#define TLBI_ASID(x)    ((x & 0xFFFF) << 48)
#define USER_EXEC(vm)   (!(((vm)->prot) & PTE_UXN))

extern unsigned long empty_zero_page[];
extern struct memmap idle_memmap;

extern void __flush_icache_range(void *va, unsigned long length);
extern void __memmap_switch(unsigned long pgd, unsigned long asid, unsigned long zero_page);
extern void __tlbi_aside1is(unsigned long asid);
extern void __tlbi_vmalle1();
extern struct virtualmem *alloc_virtualmem();

static unsigned long new_asid_context(struct memmap *new);

static unsigned long active_asids[NUM_CPUS];
static unsigned long asid_bitmap[ASID_BITMAP_SIZE];
static unsigned long asid_generation = ASID_FIRST_VERSION;
static struct spinlock asid_lock = {
    .owner = 0,
    .ticket = 0
};
static unsigned long reserved_asids[NUM_CPUS];
static unsigned long tlb_flush_bitmap[FLUSH_BITMAP_SIZE];

static inline int can_switch_fast(unsigned long cpuid,
    unsigned long asid,
    unsigned long oldasid)
{
    return oldasid && 
        !((asid ^ READ_ONCE(asid_generation)) >> ASID_BITS) &&
        CMPXCHG_RELAXED(&(active_asids[cpuid]), oldasid, asid);
}

static inline int freeable_page_table(int index, int end, struct memmap *mm,
    struct virtualmem *next, unsigned int shift)
{
    return (
        index < end ||
        &(next->vmlist) == &(mm->vmems) ||
        index < ((next->vm_start >> shift) & (TABLE_INDEX_MASK))
    );
}

static int check_update_reserved_asid(unsigned long asid, unsigned long newasid)
{
    int hit = 0;
    for(unsigned int i = 0; i < NUM_CPUS; i++) {
        if(reserved_asids[i] == asid) {
            hit = 1;
            reserved_asids[i] = newasid;
        }
    }
    return hit;
}

static void flush_context()
{
    unsigned long asid;
    bitmap_zero(asid_bitmap, NUM_USER_ASIDS);
    for(unsigned int i = 0; i < NUM_CPUS; i++) {
        asid = XCHG_RELAXED(&(active_asids[i]), 0);
        if(!asid) {
            asid = reserved_asids[i];
        }
        set_bit(asid_bitmap, ASID2IDX(asid));
        reserved_asids[i] = asid;
    }
    bitmap_fill(tlb_flush_bitmap, NUM_CPUS);
}

void free_user_memmap(struct memmap *mm)
{
    int g, g_start, g_end, u, u_start, u_end, m, m_start, m_end;
    unsigned long start, end;
    unsigned long pgd_raw_entry;
    unsigned long pud_phys_addr, *pud_virt_addr, pud_raw_entry;
    unsigned long pmd_phys_addr, *pmd_virt_addr, pmd_raw_entry;
    unsigned long pte_phys_addr, *pte_virt_addr;
    struct page *page, *pgtable;
    struct virtualmem *vm, *next;
    unsigned long *pgd = mm->pgd;
    LIST_FOR_EACH_ENTRY_SAFE(vm, next, &(mm->vmems), vmlist) {
        start = vm->vm_start;
        end = vm->vm_end - 1;
        page = vm->page;
        g_start = (start >> PGD_SHIFT) & (TABLE_INDEX_MASK);
        g_end = (end >> PGD_SHIFT) & (TABLE_INDEX_MASK);
        for(g = g_start; g <= g_end; g++) {
            pgd_raw_entry = *(pgd + g);
            pud_phys_addr = pgd_raw_entry & (RAW_PAGE_TABLE_ADDR_MASK);
            pud_virt_addr = (unsigned long *) PHYS_TO_VIRT(pud_phys_addr);
            if(!pud_phys_addr) {
                continue;
            }
            u_start = (start >> PUD_SHIFT) & (TABLE_INDEX_MASK);
            u_end = (end >> PUD_SHIFT) & (TABLE_INDEX_MASK);
            for(u = u_start; u <= u_end; u++) {
                pud_raw_entry = *(pud_virt_addr + u);
                pmd_phys_addr = pud_raw_entry & (RAW_PAGE_TABLE_ADDR_MASK);
                pmd_virt_addr = (unsigned long *) PHYS_TO_VIRT(pmd_phys_addr);
                if(!pmd_phys_addr) {
                    continue;
                }
                m_start = (start >> PMD_SHIFT) & (TABLE_INDEX_MASK);
                m_end = (end >> PMD_SHIFT) & (TABLE_INDEX_MASK);
                for(m = m_start; m <= m_end; m++) {
                    pmd_raw_entry = *(pmd_virt_addr + m);
                    pte_phys_addr = pmd_raw_entry & (RAW_PAGE_TABLE_ADDR_MASK);
                    pte_virt_addr = (unsigned long *) PHYS_TO_VIRT(pte_phys_addr);
                    if(!pte_phys_addr) {
                        continue;
                    }
                    if(freeable_page_table(m, m_end, mm, next, PMD_SHIFT)) {
                        DMB(ishst);
                        pgtable = &(PTR_TO_PAGE(pte_virt_addr));
                        free_pages(pgtable);
                        DSB(ishst);
                    }
                }
                if(freeable_page_table(u, u_end, mm, next, PUD_SHIFT)) {
                    DMB(ishst);
                    pgtable = &(PTR_TO_PAGE(pmd_virt_addr));
                    free_pages(pgtable);
                    DSB(ishst);
                }
            }
            if(freeable_page_table(g, g_end, mm, next, PGD_SHIFT)) {
                DMB(ishst);
                pgtable = &(PTR_TO_PAGE(pud_virt_addr));
                free_pages(pgtable);
                DSB(ishst);
            }
        }
        free_pages(page);
        list_delete(&(vm->vmlist));
        cake_free(vm);
    }
    __tlbi_aside1is(TLBI_ASID(mm->context.id));
}

void init_mem_context(struct memmap *new)
{
    new->context.id = 0;
}

void memmap_switch(struct memmap *old, struct memmap *new, struct process *p)
{
    unsigned long cpuid, flags, asid, oldasid;
    if(old != new && new != &idle_memmap) {
        cpuid = SMP_ID();
        asid = READ_ONCE(new->context.id);
        oldasid = READ_ONCE(active_asids[cpuid]);
        if(!can_switch_fast(cpuid, asid, oldasid)) {
            flags = SPIN_LOCK_IRQSAVE(&asid_lock);    
            if((asid ^ READ_ONCE(asid_generation)) >> ASID_BITS) {
                asid = new_asid_context(new);
                WRITE_ONCE(new->context.id, asid);
            }
            if(test_and_clear_bit(tlb_flush_bitmap, cpuid)) {
                __tlbi_vmalle1();
            }
            WRITE_ONCE(active_asids[cpuid], asid);
            SPIN_UNLOCK_IRQRESTORE(&asid_lock, flags);
        }
        __memmap_switch(VIRT_TO_PHYS((unsigned long) (new->pgd)), asid, 
            VIRT_TO_PHYS((unsigned long) (empty_zero_page)));
    }
}

static unsigned long new_asid_context(struct memmap *new)
{
    static unsigned int index = 1;
    unsigned long asid = READ_ONCE(new->context.id);
    unsigned long generation = READ_ONCE(asid_generation);
    if(asid) {
        unsigned long newasid = generation | (asid & ASID_MASK);
        if(check_update_reserved_asid(asid, newasid)) {
            return newasid;
        }
        if (!test_and_set_bit(asid_bitmap, ASID2IDX(asid))) {
            return newasid;
        }
    }
    asid = find_next_zero_bit(asid_bitmap, index, NUM_USER_ASIDS);
    if (asid == NUM_USER_ASIDS) {
        generation = ATOMIC_LONG_ADD_RETURN_RELAXED(&generation, ASID_FIRST_VERSION);
        flush_context();
        asid = find_next_zero_bit(asid_bitmap, NUM_USER_ASIDS, 1);
    }
    set_bit(asid_bitmap, asid);
    index = asid;
    return IDX2ASID(asid) | generation;
}

static int grow_stack(unsigned long addr, struct virtualmem **vm)
{
    struct list *insert;
    struct page *page;
    struct virtualmem *new, *check = *vm;
    int isstack = VM_ISSTACK(check);
    int address_in_stack_range = addr >= (STACK_TOP - MAX_STACK_AREA);
    int address_in_next_stack_allocation = addr >= (check->vm_start - STACK_SIZE);
    if(isstack && address_in_stack_range && address_in_next_stack_allocation) {
        new = alloc_virtualmem();
        if(!new) {
            goto failure;
        }
        page = alloc_pages(STACK_SHIFT);
        if(!page) {
            goto freevirtualmem;
        }
        memset(page, 0, STACK_SIZE);
        new->vm_end = check->vm_end - STACK_SIZE;
        new->vm_start = new->vm_end - STACK_SIZE;
        new->mm = check->mm;
        new->prot = check->prot;
        new->flags = check->flags;
        new->page = page;
        insert = check->vmlist.prev;
        list_add(insert, &(new->vmlist));
        *vm = new;
        return 0;
    }
    goto failure;
freevirtualmem:
    cake_free(new);
failure:
    return 1;
}

int populate_page_tables(unsigned long addr, struct memmap *mm)
{
    unsigned int pgd_index, pud_index, pmd_index, pte_index;
    unsigned long pgd_raw_entry;
    unsigned long pud_phys_addr, *pud_virt_addr, pud_raw_entry;
    unsigned long pmd_phys_addr, *pmd_virt_addr, pmd_raw_entry;
    unsigned long pte_phys_addr, *pte_virt_addr, *pte_target, mapping_addr;
    unsigned long flags;
    unsigned long *pgd = mm->pgd;
    struct virtualmem *vm = 0;
    struct page *page, *ptable;
    flags = SPIN_LOCK_IRQSAVE(&(mm->lock));
    LIST_FOR_EACH_ENTRY(vm, &(mm->vmems), vmlist) {
        if(vm->vm_end > addr) {
            break;
        }
    }
    if(!vm) {
        goto failure;
    }
    if(addr < vm->vm_start && grow_stack(addr, &vm)) {
        goto failure;
    }
    page = vm->page;
    pgd_index = (addr >> PGD_SHIFT) & (TABLE_INDEX_MASK);
    pgd_raw_entry = *(pgd + pgd_index);
    pud_phys_addr = pgd_raw_entry & (RAW_PAGE_TABLE_ADDR_MASK);
    pud_virt_addr = (unsigned long *) PHYS_TO_VIRT(pud_phys_addr);
    if(!pud_phys_addr) {
        DMB(ishst);
        ptable = alloc_pages(0);
        if(!ptable) {
            goto unlock;
        }
        pud_virt_addr = (unsigned long *) PFN_TO_PTR((ptable->pfn));
        memset(pud_virt_addr, 0, PAGE_SIZE);
        pud_phys_addr = VIRT_TO_PHYS((unsigned long) pud_virt_addr);
        WRITE_ONCE(*(pgd + pgd_index), pud_phys_addr | PAGE_TABLE_TABLE);
        DSB(ishst);
    }
    pud_index = (addr >> PUD_SHIFT) & (TABLE_INDEX_MASK);
    pud_raw_entry = *(pud_virt_addr + pud_index);
    pmd_phys_addr = pud_raw_entry & (RAW_PAGE_TABLE_ADDR_MASK);
    pmd_virt_addr = (unsigned long *) PHYS_TO_VIRT(pmd_phys_addr);
    if(!pmd_phys_addr) {
        DMB(ishst);
        ptable = alloc_pages(0);
        if(!ptable) {
            goto unlock;
        }
        pmd_virt_addr = (unsigned long *) PFN_TO_PTR((ptable->pfn));
        memset(pmd_virt_addr, 0, PAGE_SIZE);
        pmd_phys_addr = VIRT_TO_PHYS((unsigned long) pmd_virt_addr);
        WRITE_ONCE(*(pud_virt_addr + pud_index), pmd_phys_addr | PAGE_TABLE_TABLE);
        DSB(ishst);
    }
    pmd_index = (addr >> PMD_SHIFT) & (TABLE_INDEX_MASK);
    pmd_raw_entry = *(pmd_virt_addr + pmd_index);
    pte_phys_addr = pmd_raw_entry & (RAW_PAGE_TABLE_ADDR_MASK);
    pte_virt_addr = (unsigned long *) PHYS_TO_VIRT(pte_phys_addr);
    if(!pte_phys_addr) {
        DMB(ishst);
        ptable = alloc_pages(0);
        if(!ptable) {
            goto unlock;
        }
        pte_virt_addr = (unsigned long *) PFN_TO_PTR((ptable->pfn));
        memset(pte_virt_addr, 0, PAGE_SIZE);
        pte_phys_addr = VIRT_TO_PHYS((unsigned long) pte_virt_addr);
        WRITE_ONCE(*(pmd_virt_addr + pmd_index), pte_phys_addr | PAGE_TABLE_TABLE);
        DSB(ishst);
    }
    pte_index = (addr >> PAGE_SHIFT) & (TABLE_INDEX_MASK);
    pte_target = pte_virt_addr + pte_index;
    if(!(*pte_target)) {
        DMB(ishst);
        mapping_addr = VIRT_TO_PHYS((unsigned long) PAGE_TO_PTR(page));
        mapping_addr += (addr - vm->vm_start);
        mapping_addr &= PAGE_MASK;
        WRITE_ONCE(*pte_target, mapping_addr | vm->prot);
        DSB(ishst);
    }
    PREEMPT_DISABLE();
    SPIN_UNLOCK_IRQRESTORE(&(mm->lock), flags);
    if(USER_EXEC(vm)) {
        __flush_icache_range(PAGE_TO_PTR(page), (PAGE_SIZE << page->current_order));
    }
    PREEMPT_ENABLE();
    return 0;
unlock:
    SPIN_UNLOCK_IRQRESTORE(&(mm->lock), flags);
failure:
    return !(0);
}
