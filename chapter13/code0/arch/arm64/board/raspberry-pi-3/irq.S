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

#include "config/config.h"
#include "arch/bare-metal.h"
#include "arch/linux-extension.h"
#include "board/devio.h"
#include "board/peripheral.h"

#define INTERRUPT_CONTROLLER_BASE   (MAIN_PERIPH_BASE + 0xB000)
#define IRQ_PENDING_1               (INTERRUPT_CONTROLLER_BASE + 0x204)
#define ENABLE_IRQS_1               (INTERRUPT_CONTROLLER_BASE + 0x210)

#define IRQ_MAILBOX_CONTROL_BASE    ((LOCAL_PERIPH_BASE) + 0x50)
#define LOCAL_IRQ_SOURCE_BASE       ((LOCAL_PERIPH_BASE) + 0x60)
#define IRQ_MAILBOX_SET_BASE        ((LOCAL_PERIPH_BASE) + 0x80)
#define IRQ_MAILBOX_CLEAR_BASE      ((LOCAL_PERIPH_BASE) + 0xC0)

#define IRQ_MAILBOX_CPU_SHIFT       (4)
#define IRQ_MAILBOX3_OFFSET         (0xC)
#define IRQ_MAILBOX3_SHIFT          (3)
#define IRQ_MAILBOX3_ENABLE         (1 << (IRQ_MAILBOX3_SHIFT))

.globl __irq_broadcast
__irq_broadcast:
    mov             w4, #1
    lsl             w4, w4, w0
    mov             x0, #(NUM_CPUS - 1)
    __MOV_Q         x3, PHYS_TO_VIRT(IRQ_MAILBOX_SET_BASE)
1:
    mov             x1, x0
    lsl             x1, x1, #IRQ_MAILBOX_CPU_SHIFT
    add             x1, x1, #IRQ_MAILBOX3_OFFSET
    add             x5, x3, x1
    __DEV_WRITE_32  w4, x5
    subs            x0, x0, #1
    b.gt            1b
    ret

.globl __irq_enable_id
__irq_enable_id:
    __MOV_Q         x1, PHYS_TO_VIRT(ENABLE_IRQS_1)
    mov             w2, #1
    lsl             w2, w2, w0
    __DEV_READ_32   w0, x1
    orr             w0, w0, w2
    __DEV_WRITE_32  w0, x1
    isb
    ret

.globl __irq_enable_mailbox3
__irq_enable_mailbox3:
    mov             x0, #(NUM_CPUS - 1)
    __MOV_Q         x3, PHYS_TO_VIRT(IRQ_MAILBOX_CONTROL_BASE)
    mov             w4, #IRQ_MAILBOX3_ENABLE
1:
    mov             x1, x0
    lsl             x1, x1, #2
    add             x5, x3, x1
    __DEV_WRITE_32  w4, x5
    subs            x0, x0, #1
    b.gt            1b
    ret

.globl __irq_mailbox
__irq_mailbox:
    __MOV_Q         x0, PHYS_TO_VIRT(IRQ_MAILBOX_CLEAR_BASE)
    mrs             x1, mpidr_el1
    and             x1, x1, #MPIDR_HWID_MASK_LITE
    lsl             x1, x1, #IRQ_MAILBOX_CPU_SHIFT
    add             x0, x0, x1
    add             x0, x0, #IRQ_MAILBOX3_OFFSET
    __DEV_READ_32   w0, x0
    ret

.globl __irq_mailbox_clear
__irq_mailbox_clear:
    mov             w4, #1
    lsl             w4, w4, w0
    __MOV_Q         x0, PHYS_TO_VIRT(IRQ_MAILBOX_CLEAR_BASE)
    mrs             x1, mpidr_el1
    and             x1, x1, #MPIDR_HWID_MASK_LITE
    lsl             x1, x1, #IRQ_MAILBOX_CPU_SHIFT
    add             x0, x0, x1
    add             x0, x0, #IRQ_MAILBOX3_OFFSET
    __DEV_WRITE_32  w4, x0
    ret

.globl __irq_read
__irq_read:
    __MOV_Q         x0, PHYS_TO_VIRT(IRQ_PENDING_1)
    __DEV_READ_32   w0, x0
    ret

.globl __irq_source
__irq_source:
    __MOV_Q         x0, PHYS_TO_VIRT(LOCAL_IRQ_SOURCE_BASE)
    mrs             x1, mpidr_el1
    and             x1, x1, #MPIDR_HWID_MASK_LITE
    lsl             x1, x1, #2
    add             x0, x0, x1
    __DEV_READ_32   w0, x0
    ret
