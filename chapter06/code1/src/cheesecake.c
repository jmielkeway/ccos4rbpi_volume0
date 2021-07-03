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

#include "cake/lock.h"
#include "cake/log.h"
#include "arch/irq.h"
#include "arch/lock.h"
#include "arch/smp.h"

extern void allocate_init();
extern void irq_init();
extern void log_init();
extern void paging_init();
extern void smp_init();
extern void timer_init();

static void do_idle();
static void init();

static unsigned long count = 0;
static struct spinlock big_cake_lock = {
    .owner = 0,
    .ticket = 0
};

void cheesecake_main(void)
{
    SPIN_LOCK(&big_cake_lock);
    init();
    log("Hello, Cheesecake!\r\n");
    SPIN_UNLOCK(&big_cake_lock);
    IRQ_ENABLE();
    do_idle();
}

static void do_idle()
{
    unsigned long cpuid = SMP_ID();
    char *version = "0.6.1.17";
    while (1) {
        unsigned long flags = SPIN_LOCK_IRQSAVE(&big_cake_lock);
        log("Version: %s\r\n", version);
        log("CPU %x says hello!\r\n", cpuid);
        log("Message count: %x\r\n", count++);
        log("\r\n");
        SPIN_UNLOCK_IRQRESTORE(&big_cake_lock, flags);
        WAIT_FOR_INTERRUPT();
    }
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
    smp_init();
    log("SMP MODULE INITIALIZED\r\n");
    allocate_init();
    log("ALLOCATE MODULE INITIALIZED\r\n");
}

void secondary_main()
{
    IRQ_ENABLE();
    do_idle();
}
