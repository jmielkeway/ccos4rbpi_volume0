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

#define IRQ_TIMER3              (0x3)
#define IRQ_AUX                 (0x1D)
#define IRQ_MASK(irq)           (1 << (irq))

#define IRQ_MAILBOX3_SOURCE     (7)
#define IRQ_GPU_SOURCE          (8)
#define IRQ_SOURCE(source)      (1 << (source))

extern void __irq_broadcast(unsigned int irq);
extern void __irq_enable_id(unsigned int irq);
extern void __irq_enable_mailbox3();
extern unsigned int __irq_mailbox();
extern void __irq_mailbox_clear(unsigned long irq);
extern unsigned int __irq_read();
extern unsigned int __irq_source();
extern void rpi3_miniuart_interrupt();
extern void timer_interrupt();
extern void timer_tick();

void handle_irq()
{
    unsigned int irq, source;
    do {
        source = __irq_source();
        if(source) {
            if(source & IRQ_SOURCE(IRQ_GPU_SOURCE)) {
                irq = __irq_read();
                if(irq & IRQ_MASK(IRQ_TIMER3)) {
                    __irq_broadcast(IRQ_TIMER3);
                    timer_interrupt();
                    timer_tick();
                }
                else if(irq & IRQ_MASK(IRQ_AUX)) {
                    rpi3_miniuart_interrupt();
                }
                else {
                    log("Encountered Undefined Interrupt: %x\r\n", irq);
                }
            }
            else if(source & IRQ_SOURCE(IRQ_MAILBOX3_SOURCE)) {
                irq = __irq_mailbox();
                if(irq & IRQ_MASK(IRQ_TIMER3)) {
                    __irq_mailbox_clear(IRQ_TIMER3);
                    timer_tick();
                }
                else {
                    log("Encountered Undefined Interrupt: %x\r\n", irq);
                }
            }
            else {
                log("Encountered Undefined Source: %x\r\n", source);
            }
        }
        else {
            break;
        }
    } while(1);
}


void irq_init()
{
    __irq_enable_id(IRQ_TIMER3);
    __irq_enable_id(IRQ_AUX);
    __irq_enable_mailbox3();
}
