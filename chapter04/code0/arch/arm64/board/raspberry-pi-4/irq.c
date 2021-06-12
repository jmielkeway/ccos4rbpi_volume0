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
#include "board/bare-metal.h"
#include "board/gic.h"

#define SPID_TIMER3     (0x63)

#define IRQ_IRQID_SHIFT     (0)
#define IRQ_CPUID_SHIFT     (10)
#define IRQ_IRQID_MASK      (0x000003FF)
#define IRQ_CPUID_MASK      (0x00001C00)
#define IRQ_IRQID_VALUE(irq)    ((irq & IRQ_IRQID_MASK) >> IRQ_IRQID_SHIFT)
#define IRQ_CPUID_VALUE(irq)    ((irq & IRQ_CPUID_MASK) >> IRQ_CPUID_SHIFT)

extern unsigned int __irq_acknowledge();
extern void __irq_enable_spid(unsigned long spid);
extern void __irq_end(unsigned int irq);
extern void __irq_target_cpumask(unsigned long spid, unsigned long mask);
extern void timer_interrupt();

static void enable_irq_target_cpu()
{
    __irq_target_cpumask(SPID_TIMER3, CPU0_MASK);
}

void handle_irq()
{
    do {
        unsigned int irq = __irq_acknowledge();
        unsigned int irqid = IRQ_IRQID_VALUE(irq);
        if(irqid < 1020) {
            __irq_end(irq);
            switch(irqid) {
                case SPID_TIMER3:
                    timer_interrupt();
                    break;
                default:
                    log("Encountered Undefined Interrupt: %x\r\n");
                    break;
            }
        }
        else {
            break;
        }
    } while(1);
}

static void init_irq_registers()
{
    __irq_enable_spid(SPID_TIMER3);
}

void irq_init()
{
    init_irq_registers();
    enable_irq_target_cpu();
}
