/*
 * Copyright (C) 2012 ARM Ltd.
 * Authors:     Catalin Marinas <catalin.marinas@arm.com>
 *              Will Deacon <will.deacon@arm.com>
 *
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

#define INVALID_SYNC_EL1T       (0)
#define INVALID_IRQ_EL1T        (1)
#define INVALID_FIQ_EL1T        (2)
#define INVALID_ERROR_EL1T      (3)
#define INVALID_SYNC_EL1H       (4)
#define INVALID_IRQ_EL1H        (5)
#define INVALID_FIQ_EL1H        (6)
#define INVALID_ERROR_EL1H      (7)
#define INVALID_SYNC_EL0_64     (8)
#define INVALID_IRQ_EL0_64      (9)
#define INVALID_FIQ_EL0_64      (10)
#define INVALID_ERROR_EL0_64    (11)
#define INVALID_SYNC_EL0_32     (12)
#define INVALID_IRQ_EL0_32      (13)
#define INVALID_FIQ_EL0_32      (14)
#define INVALID_ERROR_EL0_32    (15)

    .macro __HANDLE_INVALID_ENTRY, idx
        mov     x0, \idx
        bl      log_invalid_exception
        b       __exception_hang
    .endm

    .macro __VECTABLE_ENTRY, label
        .align 7
        b \ label
    .endm

    .macro __ENTRY_SAVE
        sub     sp, sp, #STRUCT_STACK_SAVE_REGISTERS_SIZE
        stp     x0, x1, [sp, #16 * 0]
        stp     x2, x3, [sp, #16 * 1]
        stp     x4, x5, [sp, #16 * 2]
        stp     x6, x7, [sp, #16 * 3]
        stp     x8, x9, [sp, #16 * 4]
        stp     x10, x11, [sp, #16 * 5]
        stp     x12, x13, [sp, #16 * 6]
        stp     x14, x15, [sp, #16 * 7]
        stp     x16, x17, [sp, #16 * 8]
        stp     x18, x19, [sp, #16 * 9]
        stp     x20, x21, [sp, #16 * 10]
        stp     x22, x23, [sp, #16 * 11]
        stp     x24, x25, [sp, #16 * 12]
        stp     x26, x27, [sp, #16 * 13]
        stp     x28, x29, [sp, #16 * 14]
        add     x21, sp, #STRUCT_STACK_SAVE_REGISTERS_SIZE
        mrs     x22, elr_el1
        mrs     x23, spsr_el1
        stp     x30, x21, [sp, #16 * 15]
        stp     x22, x23, [sp, #16 * 16]
    .endm

    .macro __ENTRY_RESTORE
        ldp     x22, x23, [sp, #16 * 16]
        msr     elr_el1, x22
        msr     spsr_el1, x23
        ldp     x0, x1, [sp, #16 * 0]
        ldp     x2, x3, [sp, #16 * 1]
        ldp     x4, x5, [sp, #16 * 2]
        ldp     x6, x7, [sp, #16 * 3]
        ldp     x8, x9, [sp, #16 * 4]
        ldp     x10, x11, [sp, #16 * 5]
        ldp     x12, x13, [sp, #16 * 6]
        ldp     x14, x15, [sp, #16 * 7]
        ldp     x16, x17, [sp, #16 * 8]
        ldp     x18, x19, [sp, #16 * 9]
        ldp     x20, x21, [sp, #16 * 10]
        ldp     x22, x23, [sp, #16 * 11]
        ldp     x24, x25, [sp, #16 * 12]
        ldp     x26, x27, [sp, #16 * 13]
        ldp     x28, x29, [sp, #16 * 14]
        ldp     x30, xzr, [sp, #16 * 15]
        add     sp, sp, #STRUCT_STACK_SAVE_REGISTERS_SIZE
        eret
    .endm

__exception_hang:
    wfe
    b   __exception_hang

.align 11
.global vectors
vectors:
    __VECTABLE_ENTRY    sync_el1t
    __VECTABLE_ENTRY    irq_el1t
    __VECTABLE_ENTRY    fiq_el1t
    __VECTABLE_ENTRY    error_el1t
    __VECTABLE_ENTRY    sync_el1h
    __VECTABLE_ENTRY    irq_el1h
    __VECTABLE_ENTRY    fiq_el1h
    __VECTABLE_ENTRY    error_el1h
    __VECTABLE_ENTRY    sync_el0_64
    __VECTABLE_ENTRY    irq_el0_64
    __VECTABLE_ENTRY    fiq_el0_64
    __VECTABLE_ENTRY    error_el0_64
    __VECTABLE_ENTRY    sync_el0_32
    __VECTABLE_ENTRY    irq_el0_32
    __VECTABLE_ENTRY    fiq_el0_32
    __VECTABLE_ENTRY    error_el0_32

sync_el1t:
    __HANDLE_INVALID_ENTRY  INVALID_SYNC_EL1T

irq_el1t:
    __HANDLE_INVALID_ENTRY  INVALID_IRQ_EL1T

fiq_el1t:
    __HANDLE_INVALID_ENTRY  INVALID_FIQ_EL1T

error_el1t:
    __HANDLE_INVALID_ENTRY  INVALID_ERROR_EL1T

sync_el1h:
    __HANDLE_INVALID_ENTRY  INVALID_SYNC_EL1H

irq_el1h:
    __ENTRY_SAVE
    bl  handle_irq
    __ENTRY_RESTORE

fiq_el1h:
    __HANDLE_INVALID_ENTRY  INVALID_FIQ_EL1H

error_el1h:
    __HANDLE_INVALID_ENTRY  INVALID_ERROR_EL1H

sync_el0_64:
    __HANDLE_INVALID_ENTRY  INVALID_SYNC_EL0_64

irq_el0_64:
    __HANDLE_INVALID_ENTRY  INVALID_IRQ_EL0_64

fiq_el0_64:
    __HANDLE_INVALID_ENTRY  INVALID_FIQ_EL0_64

error_el0_64:
    __HANDLE_INVALID_ENTRY  INVALID_ERROR_EL0_64

sync_el0_32:
    __HANDLE_INVALID_ENTRY  INVALID_SYNC_EL0_32

irq_el0_32:
    __HANDLE_INVALID_ENTRY  INVALID_IRQ_EL0_32

fiq_el0_32:
    __HANDLE_INVALID_ENTRY  INVALID_FIQ_EL0_32

error_el0_32:
    __HANDLE_INVALID_ENTRY  INVALID_ERROR_EL0_32
