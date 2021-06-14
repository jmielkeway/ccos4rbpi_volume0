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
#include "cake/bitops.h"
#include "cake/cake.h"
#include "cake/compiler.h"
#include "cake/lock.h"
#include "cake/vm.h"
#include "arch/lock.h"
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

extern struct memmap idle_memmap;

extern void __memmap_switch(unsigned long pgd, unsigned long asid);
extern void __tlbi_vmalle1();

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
        __memmap_switch(VIRT_TO_PHYS((unsigned long) (new->pgd)), asid);
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
