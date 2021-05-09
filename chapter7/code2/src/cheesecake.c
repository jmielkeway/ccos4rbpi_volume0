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

#include "cake/fork.h"
#include "cake/lock.h"
#include "cake/log.h"
#include "arch/irq.h"
#include "arch/lock.h"
#include "arch/smp.h"

extern void allocate_init();
extern int cake_thread(int (*fn)(void*), void *arg, unsigned long flags);
extern void do_idle();
extern void fork_init();
extern void irq_init();
extern void log_init();
extern void paging_init();
extern void schedule_current();
extern void schedule_init();
extern int schedule_test_task();
extern void smp_init();
extern void timer_init();

static void init();

static struct spinlock big_cake_lock = {
    .owner = 0,
    .ticket = 0
};

void cheesecake_main(void)
{
    SPIN_LOCK_BOOT(&big_cake_lock);
    init();
    log("Hello, Cheesecake!\r\n");
    log("Version: 0.7.2.21\r\n");
    SPIN_UNLOCK_BOOT(&big_cake_lock);
    IRQ_ENABLE();
    do_idle();
}

static void init()
{
    paging_init();
    log_init();
    log("PAGING MODULE INITIALIZED\r\n");
    log("LOG MODULE INITIALIZED\r\n");
    irq_init();
    log("IRQ MODULE INITIALIZED\r\n");
    timer_init();
    log("TIMER MODULE INITIALIZED\r\n");
    schedule_init();
    log("SCHEDULE MODULE INITIALIZED\r\n");
    smp_init();
    log("SMP MODULE INITIALIZED\r\n");
    allocate_init();
    log("ALLOCATE MODULE INITIALIZED\r\n");
    fork_init();
    log("FORK MODULE INITIALIZED\r\n");
    cake_thread(schedule_test_task, (void *) 1, CLONE_CAKETHREAD);
    cake_thread(schedule_test_task, (void *) 2, CLONE_CAKETHREAD);
}

void secondary_main()
{
    SPIN_LOCK_BOOT(&big_cake_lock);
    schedule_current();
    SPIN_UNLOCK_BOOT(&big_cake_lock);
    IRQ_ENABLE();
    do_idle();
}
