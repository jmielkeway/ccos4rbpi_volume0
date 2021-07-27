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
#include "cake/error.h"
#include "cake/fork.h"
#include "cake/process.h"
#include "cake/vm.h"
#include "arch/atomic.h"
#include "arch/lock.h"
#include "arch/page.h"
#include "arch/prot.h"
#include "arch/start.h"

extern struct memmap *alloc_memmap();
extern struct virtualmem *alloc_virtualmem();
extern void memcpy(void *to, void *from, unsigned long size);

extern long _user_begin[];
extern long _user_text_begin[];
extern long _user_text_end[];
extern long _user_rodata_begin[];
extern long _user_rodata_end[];
extern long _user_data_begin[];
extern long _user_data_end[];
extern long _user_bss_begin[];
extern long _user_bss_end[];
extern long _user_end[];
extern long _end[];

static struct virtualmem *user_vm_segment(unsigned long start_addr, unsigned long end_addr,
    unsigned long prot, unsigned long flags, struct memmap *mm, int copyinto)
{
    unsigned int page_order;
    struct page *page, *copypage;
    struct virtualmem *vm = alloc_virtualmem();
    if(!vm) {
        goto nomem;
    }
    unsigned long offset = FIRST_USER_ADDRESS - ((unsigned long) _user_begin);
    vm->mm = mm;
    vm->vm_start = offset + start_addr;
    vm->vm_end = offset + end_addr;
    vm->prot = prot;
    vm->flags = flags;
    page = &(PTR_TO_PAGE(start_addr));
    if(!copyinto) {
        ATOMIC_LONG_INC(&(page->refcount));
        vm->page = page;
    }
    else {
        page_order = LOG2_SAFE(end_addr - start_addr) - PAGE_SHIFT;
        if((end_addr - start_addr) > (1 << (PAGE_SHIFT + page_order))) {
            page_order += 1;
        }
        copypage = alloc_pages(page_order);
        if(!copypage) {
            goto freevirtualmem;
        }
        memcpy(PAGE_TO_PTR(copypage), PAGE_TO_PTR(page), (PAGE_SIZE << page_order));
        vm->page = copypage;
    }
    return vm;
freevirtualmem:
    cake_free(vm);
nomem:
    return 0;
}

static struct virtualmem *anonymous_vm_segment(unsigned long start_addr, 
    unsigned long end_addr, unsigned long prot, unsigned long flags, struct memmap *mm)
{
    unsigned int page_order;
    struct virtualmem *vm;
    struct page *page;
    page_order = LOG2_SAFE(end_addr - start_addr) - PAGE_SHIFT;
    vm = alloc_virtualmem();
    if(!vm) {
        goto nomem;
    }
    page = alloc_pages(page_order);
    if(!page) {
        goto freevirtualmem;
    }
    memset(PAGE_TO_PTR(page), 0, (PAGE_SIZE << page_order));
    vm->mm = mm;
    vm->vm_start = start_addr;
    vm->vm_end = end_addr;
    vm->prot = prot;
    vm->flags = flags;
    vm->page = page;
    return vm;
freevirtualmem:
    cake_free(vm);
nomem:
    return 0;
}

static void exec_mmap(struct memmap *mm, struct process *p)
{
    struct memmap *old_mm, *active_mm;
    old_mm = p->memmap;
    active_mm = p->active_memmap;
    p->memmap = mm;
    p->active_memmap = mm;
    memmap_switch(active_mm, mm, p);
    if(old_mm) {
        put_memmap(old_mm);
    }
    else {
        drop_memmap(active_mm);
    }
}

long do_exec(int (*user_function)(void), int init)
{
    unsigned long heap_start, program_counter, flags;
    struct process *current;
    struct memmap *mm;
    struct stack_save_registers *ssr;
    struct virtualmem *user_text, *user_rodata, *user_data, *user_bss;
    struct virtualmem *heap, *stack;
    current = CURRENT;
    mm = alloc_memmap();
    if(!mm) {
        goto nomem;
    }
    ssr = PROCESS_STACK_SAVE_REGISTERS(current);
    heap_start = VIRT_TO_PHYS((unsigned long) _user_end) + SECTION_SIZE - 1;
    heap_start &= SECTION_MASK;
    heap_start += FIRST_USER_ADDRESS;
    program_counter = (unsigned long) user_function;
    if(init) {
        program_counter -= (unsigned long) _user_text_begin;
        program_counter += FIRST_USER_ADDRESS;
    }
    user_text = user_vm_segment((unsigned long) _user_text_begin, 
        (unsigned long) _user_text_end, PAGE_USER_ROX, 
        (VM_READ | VM_EXEC | VM_SHARED), mm, 0);
    if(!user_text) {
        goto freememmap;
    }
    user_rodata = user_vm_segment((unsigned long) _user_rodata_begin, 
        (unsigned long) _user_rodata_end, PAGE_USER_RO, 
        (VM_READ | VM_SHARED), mm, 0);
    if(!user_rodata) {
        goto freeusertext;
    }
    user_data = user_vm_segment((unsigned long) _user_data_begin, 
        (unsigned long) _user_data_end, PAGE_USER_RW, 
        (VM_READ | VM_WRITE | VM_SHARED), mm, 1);
    if(!user_data) {
        goto freeuserrodata;
    }
    user_bss = user_vm_segment((unsigned long) _user_bss_begin, 
        (unsigned long) _user_bss_end, PAGE_USER_RW, 
        (VM_READ | VM_WRITE | VM_SHARED), mm, 1);
    if(!user_bss) {
        goto freeuserdata;
    }
    heap = anonymous_vm_segment(heap_start, heap_start + SECTION_SIZE, PAGE_USER_RW,
        (VM_READ | VM_WRITE | VM_GROWSUP | VM_SHARED), mm);
    if(!heap) {
        goto freeuserbss;
    }
    stack = anonymous_vm_segment(STACK_TOP - SECTION_SIZE - STACK_SIZE, 
        STACK_TOP - SECTION_SIZE, PAGE_USER_RW, (VM_READ | VM_WRITE | VM_GROWSDOWN), mm);
    if(!stack) {
        goto freeheap;
    }
    list_enqueue(&(mm->vmems), &(user_text->vmlist));
    list_enqueue(&(mm->vmems), &(user_rodata->vmlist));
    list_enqueue(&(mm->vmems), &(user_data->vmlist));
    list_enqueue(&(mm->vmems), &(user_bss->vmlist));
    list_enqueue(&(mm->vmems), &(heap->vmlist));
    list_enqueue(&(mm->vmems), &(stack->vmlist));
    flags = SPIN_LOCK_IRQSAVE(&(mm->lock));
    exec_mmap(mm, current);
    SPIN_UNLOCK_IRQRESTORE(&(mm->lock), flags);
    start_thread(ssr, program_counter, STACK_TOP - SECTION_SIZE);
    return 0;
freeheap:
    cake_free(heap);
freeuserbss:
    cake_free(user_bss);
freeuserdata:
    cake_free(user_data);
freeuserrodata:
    cake_free(user_rodata);
freeusertext:
    cake_free(user_text);
freememmap:
    cake_free(mm);
nomem:
    return -ENOMEM;
}

