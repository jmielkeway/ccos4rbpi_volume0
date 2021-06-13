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

#include "exec/asm-offsets.h"
#include "arch/bare-metal.h"
#include "arch/linux-extension.h"

.globl __cpu_switch_to
__cpu_switch_to:
    mov     x10, STRUCT_PROCESS_ARCH_CONTEXT_OFFSET
    add     x8, x0, x10
    mov     x9, sp
    stp     x19, x20, [x8], #16
    stp     x21, x22, [x8], #16
    stp     x23, x24, [x8], #16
    stp     x25, x26, [x8], #16
    stp     x27, x28, [x8], #16
    stp     x29, x9, [x8], #16
    str     x30, [x8]
    add     x8, x1, x10
    ldp     x19, x20, [x8], #16
    ldp     x21, x22, [x8], #16
    ldp     x23, x24, [x8], #16
    ldp     x25, x26, [x8], #16
    ldp     x27, x28, [x8], #16
    ldp     x29, x9, [x8], #16
    ldr     x30, [x8]
    mov     sp, x9
    msr     tpidr_el1, x1
    ret

.globl __current
__current:
    mrs     x0, tpidr_el1
    ret

.globl __memmap_switch
__memmap_switch:
    __MOV_Q     x3, LINEAR_ADDR_MASK
    adrp        x2, empty_zero_page
    and         x2, x3, x2
    msr			ttbr0_el1, x2
    isb
    tlbi        vmalle1
    bfi 		x0, x1, #48, #16
    msr 		ttbr0_el1, x0
    isb
    ret

.globl __schedule_current_init
__schedule_current_init:
    msr     tpidr_el1, x0
    ret
