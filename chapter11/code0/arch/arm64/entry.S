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
#include "arch/abort.h"
#include "user/syscall.h"

    .macro __VECTABLE_ENTRY, label
        .align 7
        b \ label
    .endm

    .macro __ENTRY_SAVE, el
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
        .if \el == 0
        mrs     x21, sp_el0
        .else
        add     x21, sp, #STRUCT_STACK_SAVE_REGISTERS_SIZE
        .endif
        mrs     x22, elr_el1
        mrs     x23, spsr_el1
        stp     x30, x21, [sp, #16 * 15]
        stp     x22, x23, [sp, #16 * 16]
    .endm

    .macro __ENTRY_RESTORE, el
        ldp     x22, x23, [sp, #16 * 16]
        ldp     x30, x21, [sp, #16 * 15]
        .if \el == 0
        msr     sp_el0, x21
        .endif
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
        add     sp, sp, #STRUCT_STACK_SAVE_REGISTERS_SIZE
        eret
    .endm

.align 11
.globl vectors
vectors:
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __sync_el1h
    __VECTABLE_ENTRY    __irq_el1h
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __sync_el064
    __VECTABLE_ENTRY    __irq_el064
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry

__invalid_entry:
    mrs     x0, esr_el1
    bl      log_invalid_exception
__exception_hang:
    wfe
    b       __exception_hang

__sync_el1h:
    __ENTRY_SAVE    1
    mrs             x1, esr_el1
    lsr             x24, x1, #ESR_ELx_EC_SHIFT
    cmp             x24, #ESR_ELx_EC_DABT_CUR
    b.eq            __el1_da
    cmp             x24, #ESR_ELx_EC_IABT_CUR
    b.eq            __el1_ia
    mrs              x0, esr_el1
    mrs              x1, far_el1
    bl              handle_sync
    b               __invalid_entry

__el1_ia:
__el1_da:
    mrs                 x0, far_el1
    mov                 x2, sp
    bl                  __irq_enable
    bl                  mem_abort
    bl                  __irq_disable
    __ENTRY_RESTORE     1

__irq_el1h:
    __ENTRY_SAVE        1
    bl                  handle_irq
    __ENTRY_RESTORE     1

__sync_el064:
    __ENTRY_SAVE    0
    mrs             x25, esr_el1
    lsr             x24, x25, #ESR_ELx_EC_SHIFT
    cmp             x24, #ESR_ELx_EC_SVC64
    b.eq            __el0_svc
    cmp             x24, #ESR_ELx_EC_DABT_LOW
    b.eq            __el0_da
    cmp             x24, #ESR_ELx_EC_IABT_LOW
    b.eq             __el0_ia
    mrs              x0, esr_el1
    mrs              x1, far_el1
    bl              handle_sync
    b               __invalid_entry

__el0_svc:
    adrp    x27, sys_call_table
    uxtw    x26, w8
    mov     x25, #NUM_SYSCALLS
    bl      __irq_enable
    cmp     x26, x25
    b.hs    1f
    ldr     x16, [x27, x26, lsl #3]
    blr     x16
    b       __ret_from_syscall
1:
    b       __invalid_entry

__ret_from_syscall:
    str     x0, [sp, #SSR_X0]
    b       __ret_to_user

__el0_ia:
__el0_da:
    mrs     x0, far_el1
    mov     x1, x25
    mov     x2, sp
    bl      __irq_enable
    bl      mem_abort
    b       __ret_to_user

__irq_el064:
    __ENTRY_SAVE    0
    bl              handle_irq
    bl              __irq_enable
    b               __ret_to_user

.globl __ret_from_fork
__ret_from_fork:
    bl                  schedule_tail
    cbz                 x19, __ret_to_user
    mov                 x0, x20
    blr                 x19
__ret_to_user:
    bl                  check_and_process_signals
    bl                  __irq_disable
    __ENTRY_RESTORE     0
