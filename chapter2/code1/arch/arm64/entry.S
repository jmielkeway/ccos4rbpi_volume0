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
        b   __exception_hang
    .endm

    .macro __VECTABLE_ENTRY, label
        .align 7
        b \ label
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
    __HANDLE_INVALID_ENTRY  INVALID_IRQ_EL1H

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
