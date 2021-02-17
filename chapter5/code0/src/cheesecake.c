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

#include "cake/log.h"
#include "arch/irq.h"
#include "arch/smp.h"

extern void irq_init();
extern void log_init();
extern void paging_init();
extern void smp_init();
extern void timer_init();
void init();

void cheesecake_main(void)
{
    unsigned long count = 1;
    char *version = "0.5.13";
    init();
    log("Hello, Cheesecake!\r\n");
    IRQ_ENABLE();
    while (1) {
        log("Version: %s\r\n", version);
        log("Message count: %x\r\n", count++);
        log("\r\n");
        WAIT_FOR_INTERRUPT();
    }
}

void secondary_main()
{
    unsigned long cpuid = SMP_ID();
    IRQ_ENABLE();
    while(1) {
        log("CPU %x says hello!\r\n", cpuid);
        WAIT_FOR_INTERRUPT();
    }
}

void init()
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
}
