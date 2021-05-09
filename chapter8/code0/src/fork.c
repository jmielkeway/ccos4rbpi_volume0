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
#include "cake/fork.h"
#include "cake/process.h"
#include "cake/schedule.h"
#include "arch/bare-metal.h"
#include "arch/schedule.h"

extern unsigned int allocate_pid(struct process *p);
extern int copy_arch_context(unsigned long flags,
    unsigned long thread_input,
    unsigned long arg,
    struct process *p);
extern void memset(void *dest, int c, unsigned long count);

static long do_clone(unsigned long flags, unsigned long thread_input, unsigned long arg);
static struct process *copy_process(unsigned long flags,
    unsigned long thread_input,
    unsigned long arg);
static struct process *duplicate_current();

static struct cache *process_cache;

int cake_thread(int (*fn)(void *), void *arg, unsigned long flags)
{
    return do_clone((flags | CLONE_CAKETHREAD), (unsigned long) fn, (unsigned long) arg);
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

static struct process *copy_process(unsigned long flags,
    unsigned long thread_input,
    unsigned long arg)
{
    struct process *p;
    p = duplicate_current();
    if(!p) {
        goto err;
    }
    if(copy_arch_context(flags, thread_input, arg, p)) {
        goto freeprocess;
    }
    p->pid = allocate_pid(p);
    return p;
freeprocess:
    cake_free(p);
err:
    return 0;
}

static struct process *duplicate_current()
{
    void *new_stack;
    struct process *p;
    struct process *current = CURRENT;
    p = alloc_obj(process_cache);
    if(!p) {
        goto nomem;
    }
    new_stack = PAGE_TO_PTR(alloc_pages((STACK_SHIFT)));
    if(!new_stack) {
        goto freeprocess;
    }
    memset(new_stack, 0, STACK_SIZE);
    *p = *current;
    p->stack = new_stack;
    return p;
freeprocess:
    cake_free(p);
nomem:
    return 0;
}

void fork_init()
{
    process_cache = alloc_cache("process", sizeof(struct process));
}
