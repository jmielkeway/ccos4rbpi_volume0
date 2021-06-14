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
#include "cake/lock.h"
#include "cake/list.h"
#include "cake/log.h"
#include "arch/lock.h"

#define PAGE_IS_TAIL(page)      ((page->pfn) & ((1 << ((page->current_order) + 1)) - 1))
#define PAGE_IS_HEAD(page)      (!(PAGE_IS_TAIL(page)))
#define HEAD_BUDDY_PFN(page)    ((page->pfn) - (1 << (page->current_order)))
#define TAIL_BUDDY_PFN(page)    ((page->pfn) + (1 << (page->current_order)))
#define HEAD_BUDDY(page)        (&(GLOBAL_MEMMAP[HEAD_BUDDY_PFN(page)]))
#define TAIL_BUDDY(page)        (&(GLOBAL_MEMMAP[TAIL_BUDDY_PFN(page)]))
#define SHOULD_COALESCE(page, buddy)                        \
    (!(buddy->allocated) &&                                 \
    ((page->current_order) == (buddy->current_order)) &&    \
    ((page->current_order) < (page->original_order))  &&    \
    ((buddy->current_order) < (buddy->original_order)))

extern void arch_populate_allocate_structures(struct list *freelists);

void free_pages(struct page *page);

static struct spinlock allocator_lock = {
    .owner = 0,
    .ticket = 0
};
static struct list freelists[MAX_ORDER + 1];
struct page *system_phys_page_dir;

struct page *alloc_pages(unsigned int order)
{
    struct list *freelist;
    struct page *buddy, *s;
    struct page *p = 0;
    unsigned long pfn, bpfn;
    unsigned int i = order;
    SPIN_LOCK(&allocator_lock);
    while(i <= MAX_ORDER) {
        freelist = &(freelists[i]);
        if(!list_empty(freelist)) {
            p = LIST_FIRST_ENTRY(freelist, struct page, pagelist);
            list_delete(&(p->pagelist));
            break;
        }
        i++;
    }
    SPIN_UNLOCK(&allocator_lock);
    if(p) {
        SPIN_LOCK(&allocator_lock);
        while(i > order) {
            --i;
            p->current_order = i;
            freelist = &(freelists[i]);
            list_add(freelist, &(p->pagelist));
            pfn = p->pfn;
            bpfn = pfn + (1 << i);
            buddy = &(GLOBAL_MEMMAP[bpfn]);
            buddy->valid = 1;
            buddy->original_order = i + 1;
            buddy->current_order = i;
            p = buddy;
        }
        SPIN_UNLOCK(&allocator_lock);
        for(i = p->pfn; i < (p->pfn + (1 << order)); i++) {
            s = &(GLOBAL_MEMMAP[i]);
            s->allocated = 1;
        }
    }
    return p;
}

void allocate_init()
{
    struct page *a, *b, *c, *p, *q, *r, *s;
    for(unsigned int i = 0; i <= MAX_ORDER; i++) {
        struct list *freelist = &(freelists[i]);
        freelist->next = freelist;
        freelist->prev = freelist;
    }
    arch_populate_allocate_structures(freelists);

    log("Full list for order 1:\r\n");
    LIST_FOR_EACH_ENTRY(q, &(freelists[1]), pagelist) {
        log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
            q->pfn, q->allocated, q->valid);
    }

    log("Full list for order 2:\r\n");
    LIST_FOR_EACH_ENTRY(q, &(freelists[2]), pagelist) {
        log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
            q->pfn, q->allocated, q->valid);
    }

    log("Orignal head of list for order 3:\r\n");
    p = LIST_FIRST_ENTRY(&(freelists[3]), struct page, pagelist);
    log("Address: %x\r\n", p);
    log("{index: %x; current order: %x; original order: %x;\r\n", 
        p->pfn, p->current_order, p->original_order);
    log("valid: %x; allocated: %x; reserved: %x\r\n", 
        p->valid, p->allocated, p->reserved);
    log("next: %x; previous: %x}\r\n\r\n", 
        p->pagelist.next, p->pagelist.prev);

    log("Looking at page directory vars:\r\n");
    for(unsigned long i = p->pfn; i < (p->pfn + (1 << 3)); i++) {
        s = &(GLOBAL_MEMMAP[i]);
        log("Address: %x\r\n", s);
        log("{index: %x; current order: %x; original order: %x;\r\n", 
            s->pfn, s->current_order, s->original_order);
        log("valid: %x; allocated: %x; reserved: %x\r\n", 
            s->valid, s->allocated, s->reserved);
        log("next: %x; previous: %x}\r\n\r\n", 
            s->pagelist.next, s->pagelist.prev);
    }

    log("Now let's allocate...\r\n");
    a = alloc_pages(1);
    log("Address: %x\r\n", a);
    log("{index: %x; current order: %x; original order: %x;\r\n", 
        a->pfn, a->current_order, a->original_order);
    log("valid: %x; allocated: %x; reserved: %x\r\n", 
        a->valid, a->allocated, a->reserved);
    log("next: %x; previous: %x}\r\n\r\n", 
        a->pagelist.next, a->pagelist.prev);

    log("Full list for order 1:\r\n");
    LIST_FOR_EACH_ENTRY(q, &(freelists[1]), pagelist) {
        log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
            q->pfn, q->allocated, q->valid);
    }

    log("Full list for order 2:\r\n");
    LIST_FOR_EACH_ENTRY(q, &(freelists[2]), pagelist) {
        log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
            q->pfn, q->allocated, q->valid);
    }

    log("New head of list for order 3:\r\n");
    r = LIST_FIRST_ENTRY(&(freelists[3]), struct page, pagelist);
    log("Address: %x\r\n", r);
    log("{index: %x; current order: %x; original order: %x;\r\n", 
        r->pfn, r->current_order, r->original_order);
    log("valid: %x; allocated: %x; reserved: %x\r\n", 
        r->valid, r->allocated, r->reserved);
    log("next: %x; previous: %x}\r\n\r\n", 
        r->pagelist.next, r->pagelist.prev);

    log("Looking at page directory vars:\r\n");
    for(unsigned long i = p->pfn; i < (p->pfn + (1 << 3)); i++) {
        s = &(GLOBAL_MEMMAP[i]);
        log("Address: %x\r\n", s);
        log("{index: %x; current order: %x; original order: %x;\r\n", 
            s->pfn, s->current_order, s->original_order);
        log("valid: %x; allocated: %x; reserved: %x\r\n", 
            s->valid, s->allocated, s->reserved);
        log("next: %x; previous: %x}\r\n\r\n", 
            s->pagelist.next, s->pagelist.prev);
    }

    log("Time for two more allocates!\r\n");
    b = alloc_pages(1);
    log("Address: %x\r\n", b);
    log("{index: %x; current order: %x; original order: %x;\r\n", 
        b->pfn, b->current_order, b->original_order);
    log("valid: %x; allocated: %x; reserved: %x\r\n", 
        b->valid, b->allocated, b->reserved);
    log("next: %x; previous: %x}\r\n\r\n", 
        b->pagelist.next, b->pagelist.prev);

    c = alloc_pages(1);
    log("Address: %x\r\n", c);
    log("{index: %x; current order: %x; original order: %x;\r\n", 
        c->pfn, c->current_order, c->original_order);
    log("valid: %x; allocated: %x; reserved: %x\r\n", 
        c->valid, c->allocated, c->reserved);
    log("next: %x; previous: %x}\r\n\r\n", 
        c->pagelist.next, c->pagelist.prev);

    log("Full list for order 1:\r\n");
    LIST_FOR_EACH_ENTRY(q, &(freelists[1]), pagelist) {
        log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
            q->pfn, q->allocated, q->valid);
    }

    log("Full list for order 2:\r\n");
    LIST_FOR_EACH_ENTRY(q, &(freelists[2]), pagelist) {
        log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
            q->pfn, q->allocated, q->valid);
    }

    log("Current head of list for order 3:\r\n");
    r = LIST_FIRST_ENTRY(&(freelists[3]), struct page, pagelist);
    log("Address: %x\r\n", r);

    log("Looking at page directory vars:\r\n");
    for(unsigned long i = p->pfn; i < (p->pfn + (1 << 3)); i++) {
        s = &(GLOBAL_MEMMAP[i]);
        log("Address: %x\r\n", s);
        log("{index: %x; current order: %x; original order: %x;\r\n", 
            s->pfn, s->current_order, s->original_order);
        log("valid: %x; allocated: %x; reserved: %x\r\n", 
            s->valid, s->allocated, s->reserved);
        log("next: %x; previous: %x}\r\n\r\n", 
            s->pagelist.next, s->pagelist.prev);
    }

    log("Now, let's free\r\n");

    log("After the first free, there should be no coalescing...\r\n");
    log("Freeing: %x\r\n", b);
    free_pages(b);
    log("Full list for order 1:\r\n");
    LIST_FOR_EACH_ENTRY(q, &(freelists[1]), pagelist) {
        log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
            q->pfn, q->allocated, q->valid);
    }

    log("Full list for order 2:\r\n");
    LIST_FOR_EACH_ENTRY(q, &(freelists[2]), pagelist) {
        log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
            q->pfn, q->allocated, q->valid);
    }

    log("Current head of list for order 3:\r\n");
    r = LIST_FIRST_ENTRY(&(freelists[3]), struct page, pagelist);
    log("Address: %x\r\n", r);

    log("Looking at page directory vars:\r\n");
    for(unsigned long i = p->pfn; i < (p->pfn + (1 << 3)); i++) {
        s = &(GLOBAL_MEMMAP[i]);
        log("Address: %x\r\n", s);
        log("{index: %x; current order: %x; original order: %x;\r\n", 
            s->pfn, s->current_order, s->original_order);
        log("valid: %x; allocated: %x; reserved: %x\r\n", 
            s->valid, s->allocated, s->reserved);
        log("next: %x; previous: %x}\r\n\r\n", 
            s->pagelist.next, s->pagelist.prev);
    }

    log("After the second free, there should be some coalescing...\r\n");
    log("Freeing: %x\r\n", a);
    free_pages(a);
    log("Full list for order 1:\r\n");
    LIST_FOR_EACH_ENTRY(q, &(freelists[1]), pagelist) {
        log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
            q->pfn, q->allocated, q->valid);
    }

    log("Full list for order 2:\r\n");
    LIST_FOR_EACH_ENTRY(q, &(freelists[2]), pagelist) {
        log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
            q->pfn, q->allocated, q->valid);
    }

    log("Current head of list for order 3:\r\n");
    r = LIST_FIRST_ENTRY(&(freelists[3]), struct page, pagelist);
    log("Address: %x\r\n", r);

    log("Looking at page directory vars:\r\n");
    for(unsigned long i = p->pfn; i < (p->pfn + (1 << 3)); i++) {
        s = &(GLOBAL_MEMMAP[i]);
        log("Address: %x\r\n", s);
        log("{index: %x; current order: %x; original order: %x;\r\n", 
            s->pfn, s->current_order, s->original_order);
        log("valid: %x; allocated: %x; reserved: %x\r\n", 
            s->valid, s->allocated, s->reserved);
        log("next: %x; previous: %x}\r\n\r\n", 
            s->pagelist.next, s->pagelist.prev);
    }

    log("After the final free, there should be complete coalescing...\r\n");
    log("Freeing: %x\r\n", c);
    free_pages(c);
    log("Full list for order 1:\r\n");
    LIST_FOR_EACH_ENTRY(q, &(freelists[1]), pagelist) {
        log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
            q->pfn, q->allocated, q->valid);
    }

    log("Full list for order 2:\r\n");
    LIST_FOR_EACH_ENTRY(q, &(freelists[2]), pagelist) {
        log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
            q->pfn, q->allocated, q->valid);
    }

    log("Current head of list for order 3:\r\n");
    r = LIST_FIRST_ENTRY(&(freelists[3]), struct page, pagelist);
    log("Address: %x\r\n", r);

    log("Looking at page directory vars:\r\n");
    for(unsigned long i = p->pfn; i < (p->pfn + (1 << 3)); i++) {
        s = &(GLOBAL_MEMMAP[i]);
        log("Address: %x\r\n", s);
        log("{index: %x; current order: %x; original order: %x;\r\n", 
            s->pfn, s->current_order, s->original_order);
        log("valid: %x; allocated: %x; reserved: %x\r\n", 
            s->valid, s->allocated, s->reserved);
        log("next: %x; previous: %x}\r\n\r\n", 
            s->pagelist.next, s->pagelist.prev);
    }
}

void free_pages(struct page *page)
{
    struct page *buddy, *s;
    for(unsigned long i = page->pfn; i < (page->pfn + (1 << page->current_order)); i++) {
        s = &(GLOBAL_MEMMAP[i]);
        s->allocated = 0;
    }
    SPIN_LOCK(&allocator_lock);
    while(1) {
        if(PAGE_IS_HEAD(page)) {
            buddy = TAIL_BUDDY(page);
            if(SHOULD_COALESCE(page, buddy)) {
                list_delete(&(buddy->pagelist));
                buddy->valid = 0;
                buddy->current_order = 0;
                buddy->original_order = 0;
                page->current_order++;
                continue;
            }
        }
        else {
            buddy = HEAD_BUDDY(page);
            if(SHOULD_COALESCE(page, buddy)) {
                list_delete(&(buddy->pagelist));
                page->valid = 0;
                page->current_order = 0;
                page->original_order = 0;
                buddy->current_order++;
                page = buddy;
                continue;
            }
        }
        break;
    }
    list_add(&(freelists[page->current_order]), &(page->pagelist));
    SPIN_UNLOCK(&allocator_lock);
}
