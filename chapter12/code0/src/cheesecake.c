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
#include "user/fork.h"

extern void allocate_init();
extern int cake_thread(int (*fn)(void*), void *arg, unsigned long flags);
extern void do_idle();
extern void filesystem_init();
extern void fork_init();
extern void irq_init();
extern void log_init();
extern void paging_init();
extern int perform_work(void *unused);
extern void pid_init();
extern void schedule_current();
extern void signal_init();
extern void schedule_init();
extern void smp_init();
extern int startup_user(void *user_function);
extern void timer_init();
extern int USER_STARTUP_FUNCTION();

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
    log("Version: 0.12.0.37\r\n");
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
    pid_init();
    log("PID MODULE INITIALIZED\r\n");
    signal_init();
    log("SIGNAL MODULE INITIALIZED\r\n");
    fork_init();
    log("FORK MODULE INITIALIZED\r\n");
    filesystem_init();
    log("FILESYSTEM MODULE INITIALIZED\r\n");
    cake_thread(startup_user, USER_STARTUP_FUNCTION, CLONE_CAKETHREAD | CLONE_PRIORITY_USER);
    cake_thread(perform_work, (void *) 0, CLONE_CAKETHREAD | CLONE_PRIORITY_CAKE_THREAD);
}

void secondary_main()
{
    SPIN_LOCK_BOOT(&big_cake_lock);
    schedule_current();
    SPIN_UNLOCK_BOOT(&big_cake_lock);
    IRQ_ENABLE();
    do_idle();
}
