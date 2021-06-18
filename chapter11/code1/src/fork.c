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
#include "cake/schedule.h"
#include "cake/signal.h"
#include "cake/vm.h"
#include "arch/atomic.h"
#include "arch/bare-metal.h"
#include "arch/schedule.h"
#include "user/signal.h"

extern unsigned int allocate_pid(struct process *p);
extern int copy_arch_context(unsigned long flags,
    unsigned long thread_input,
    unsigned long arg,
    struct process *p);
static long copy_signal(struct process *p);
extern void init_mem_context(struct memmap *new);
extern void memcpy(void *to, void *from, unsigned long count);
extern void memset(void *dest, int c, unsigned long count);

static long do_clone(unsigned long flags, unsigned long thread_input, unsigned long arg);
static struct process *duplicate_current();
static struct memmap *duplicate_memmap(struct memmap *old, struct process *p);

static struct cache *memmap_cache;
static struct cache *process_cache;
static struct cache *signal_cache;
static struct cache *virtualmem_cache;

struct memmap *alloc_memmap()
{
    struct memmap *mm = alloc_obj(memmap_cache);
    if(!mm) {
        goto nomem;
    }
    struct page *page = alloc_pages(0);
    if(!page) {
        goto freememmap;
    }
    memset(mm, 0, sizeof(*mm));
    mm->vmems.prev = &(mm->vmems);
    mm->vmems.next = &(mm->vmems);
    mm->pgd = PAGE_TO_PTR(page);
    memset(mm->pgd, 0, PAGE_SIZE);
    mm->users = 1;
    mm->refcount = 1;
    return mm;
freememmap:
    cake_free(mm);
nomem:
    return 0;
}

struct virtualmem *alloc_virtualmem()
{
    struct virtualmem *vm = alloc_obj(virtualmem_cache);
    if(!vm) {
        return 0;
    }
    memset(vm, 0, sizeof(*vm));
    vm->vmlist.prev = &(vm->vmlist);
    vm->vmlist.next = &(vm->vmlist);
    return vm;
}

int cake_thread(int (*fn)(void *), void *arg, unsigned long flags)
{
    return do_clone((flags | CLONE_CAKETHREAD), (unsigned long) fn, (unsigned long) arg);
}

static int copy_memmap(struct process *p)
{
    struct memmap *new, *old;
    p->memmap = 0;
    p->active_memmap = 0;
    old = CURRENT->memmap;
    if(old) {
        new = duplicate_memmap(old, p);
        if(!new) {
            return -ENOMEM;
        }
        p->memmap = new;
        p->active_memmap = new;
    }
    return 0;
}

static struct process *copy_process(unsigned long flags,
    unsigned long thread_input,
    unsigned long arg)
{
    struct process *p;
    p = duplicate_current();
    if(!p) {
        goto err;
    }
    if(copy_memmap(p)) {
        goto freeprocess;
    }
    if(copy_signal(p)) {
        goto freememmap;
    }
    if(copy_arch_context(flags, thread_input, arg, p)) {
        goto freememmap;
    }
    p->pid = allocate_pid(p);
    return p;
freememmap:
    if(p->memmap) {
        put_memmap(p->memmap);
    }
freeprocess:
    cake_free(p);
err:
    return 0;
}

static long copy_signal(struct process *p)
{
    struct signal *signal = alloc_obj(signal_cache);
    if(!signal) {
        return -ENOMEM;
    }
    memset(signal, 0, sizeof(*signal));
    signal->signallist.prev = &(signal->signallist);
    signal->signallist.next = &(signal->signallist);
    signal->waitqueue.waitlist.prev = &(signal->waitqueue.waitlist);
    signal->waitqueue.waitlist.next = &(signal->waitqueue.waitlist);
    for(int i = 0; i < NUM_SIGNALS; i++) {
        signal->sighandler.sigaction[i].fn = SIG_DFL;
    }
    signal->sighandler.sigaction[SIGCHLD - 1].fn = SIG_IGN;
    signal->refcount = 1;
    p->signal = signal;
    return 0;
}

static struct virtualmem *copy_virtualmem(struct virtualmem *old)
{
    unsigned long copy_count;
    struct page *page, *copy_page;
    struct virtualmem *new = alloc_obj(virtualmem_cache);
    if(!new) {
        goto failure;
    }
    page = old->page;
    *new = *old;
    new->lock.owner = 0;
    new->lock.ticket = 0;
    new->vmlist.prev = &(new->vmlist);
    new->vmlist.next = &(new->vmlist);
    if(VM_ISSTACK(old)) {
        copy_page = alloc_pages(page->current_order);
        if(!copy_page) {
            goto freevirtualmem;
        }
        copy_count = PAGE_SIZE << (page->current_order);
        memcpy(PAGE_TO_PTR(copy_page), PAGE_TO_PTR(page), copy_count);
        new->page = copy_page;
    }
    else {
        ATOMIC_LONG_INC(&(page->refcount));
    }
    return new;
freevirtualmem:
    cake_free(new);
failure:
    return 0;
}

static long do_clone(unsigned long flags, unsigned long thread_input, unsigned long arg)
{
    struct process *p;
    p = copy_process(flags, thread_input, arg);
    if(!p) {
        return -1;
    }
    runqueue_process(p);
    return p->pid;
}

static struct process *duplicate_current()
{
    struct page *stack;
    struct process *p, *current;
    current = CURRENT;
    p = alloc_obj(process_cache);
    if(!p) {
        goto nomem;
    }
    stack = alloc_pages(STACK_SHIFT);
    if(!stack) {
        goto freeprocess;
    }
    *p = *current;
    p->parent = current;
    p->stack = PAGE_TO_PTR(stack);
    memset(p->stack, 0, STACK_SIZE);
    return p;
freeprocess:
    cake_free(p);
nomem:
    return 0;
}

static struct memmap *duplicate_memmap(struct memmap *old, struct process *p)
{
    struct memmap *new;
    struct virtualmem *old_vm, *new_vm, *dup_vm;
    struct page *pgd;
    new = alloc_obj(memmap_cache);
    if(!new) {
        goto nomem;
    }
    pgd = alloc_pages(0);
    if(!pgd) {
        goto freememmap;
    }
    *new = *old;
    new->users = 1;
    new->refcount = 1;
    new->lock.owner = 0;
    new->lock.ticket = 0;
    new->pgd = PAGE_TO_PTR(pgd);   
    memset(new->pgd, 0, PAGE_SIZE);
    new->vmems.prev = &(new->vmems);
    new->vmems.next = &(new->vmems);
    init_mem_context(new);
    LIST_FOR_EACH_ENTRY(old_vm, &(old->vmems), vmlist) {
        dup_vm = copy_virtualmem(old_vm);
        if(!dup_vm) {
            goto freevirtualmems;
        }
        dup_vm->mm = new;
        list_enqueue(&(new->vmems), &(dup_vm->vmlist));
    }
    return new;
freevirtualmems:
    LIST_FOR_EACH_ENTRY_SAFE(dup_vm, new_vm, &(new->vmems), vmlist) {
        cake_free(dup_vm);
    }
freememmap:
    cake_free(new);
nomem:
    return 0;
}

void fork_init()
{
    memmap_cache = alloc_cache("memmap", sizeof(struct memmap));
    process_cache = alloc_cache("process", sizeof(struct process));
    signal_cache = alloc_cache("signal", sizeof(struct signal));
    virtualmem_cache = alloc_cache("virtualmem", sizeof(struct virtualmem));
}

int sys_clone(unsigned long flags, unsigned long thread_input, unsigned long arg)
{
    return do_clone(flags, thread_input, arg);
}
