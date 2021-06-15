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

#ifndef _CAKE_VM_H
#define _CAKE_VM_H

#include "cake/list.h"
#include "cake/lock.h"
#include "arch/vm.h"

#define VM_READ         (0b00000001)
#define VM_WRITE        (0b00000010)
#define VM_EXEC         (0b00000100)
#define VM_SHARED       (0b00001000)
#define VM_GROWSUP      (0b00010000)
#define VM_GROWSDOWN    (0b00100000)

#define VM_ISCODE(x)    (((x)->flags) == (VM_READ | VM_EXEC | VM_SHARED))
#define VM_ISDATA(x)    (((x)->flags) == (VM_READ | VM_WRITE | VM_SHARED))
#define VM_ISRODATA(x)  (((x)->flags) == (VM_READ | VM_SHARED))
#define VM_ISHEAP(x)    (((x)->flags) == (VM_READ | VM_WRITE | VM_GROWSUP | VM_SHARED))
#define VM_ISSTACK(x)   (((x)->flags) == (VM_READ | VM_WRITE | VM_GROWSDOWN))

struct virtualmem {
    struct memmap *mm;
    unsigned long vm_start;
    unsigned long vm_end;
    unsigned long prot;
    unsigned long flags;
    struct spinlock lock;
    struct list vmlist;
    struct page *page;
};

struct memmap {
    struct list vmems;
    unsigned long vmem_low;
    unsigned long vmem_high;
    unsigned long users;
    unsigned long refcount;
    unsigned long flags;
    void *pgd;
    unsigned long start_stack;
    unsigned long start_heap;
    unsigned long end_heap;
    struct spinlock lock;
    struct mem_context context;    
};

void drop_memmap(struct memmap *memmap);
void put_memmap(struct memmap *memmap);

#endif
