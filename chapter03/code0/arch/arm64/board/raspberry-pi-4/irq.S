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

#include "arch/linux-extension.h"
#include "board/devio.h"
#include "board/gic.h"

.globl __irq_acknowledge
__irq_acknowledge:
    __MOV_Q         x0, GICC_IAR
    __DEV_READ_32   w0, x0
    ret

.globl __irq_enable_spid
__irq_enable_spid:
    mov             x13, #5
    lsr             x1, x0, x13
    mov             x13, #2
    lsl             x1, x1, x13
    __MOV_Q         x3, GICD_ISENABLER_OFFSET
    add             x3, x3, x1
    and             w2, w0, #0x1f
    mov             x0, #1
    lsl             w2, w0, w2
    __DEV_WRITE_32  w2, x3
    isb
    ret

.globl __irq_end
__irq_end:
    __MOV_Q         x1, GICC_EOIR
    __DEV_WRITE_32  w0, x1
    isb
    ret

.globl __irq_target_cpumask
__irq_target_cpumask:
    mov             w3, #3
    and             w3, w0, w3
    lsl             w3, w3, #3
    lsl             w1, w1, w3
    mov             w3, #~(3)
    and             w2, w0, w3
    __MOV_Q         x3, GICD_ITARGETSR_OFFSET
    add             x3, x3, x2
    __DEV_READ_32   w4, x3
    orr             w4, w4, w1
    __DEV_WRITE_32  w4, x3
    isb
    ret
