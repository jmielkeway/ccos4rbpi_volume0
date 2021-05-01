/*
 * Copyright (C) 1994-2002 Russell King
 * Copyright (C) 2003-2012 ARM Ltd.
 * Authors: Catalin Marinas <catalin.marinas@arm.com>
 *      Will Deacon <will.deacon@arm.com>
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

#include "config/config.h"
#include "arch/bare-metal.h"
#include "arch/linux-extension.h"
#include "arch/page.h"

    .macro __CREATE_TABLE_ENTRY, tbl, virt, shift, num, tmp1, tmp2
        lsr     \tmp1, \virt, #\shift
        and     \tmp1, \tmp1, #\num - 1
        add     \tmp2, \tbl, #PAGE_SIZE
        orr     \tmp2, \tmp2, #PAGE_TABLE_TABLE
        str     \tmp2, [\tbl, \tmp1, lsl #3]
        add     \tbl, \tbl, #PAGE_SIZE
    .endm

    .macro __CREATE_PGD, tbl, virt, tmp1, tmp2
        __CREATE_TABLE_ENTRY    \tbl, \virt, PGD_SHIFT, NUM_ENTRIES_PER_TABLE, \tmp1, \tmp2
        __CREATE_TABLE_ENTRY    \tbl, \virt, PUD_SHIFT, NUM_ENTRIES_PER_TABLE, \tmp1, \tmp2
    .endm

    .macro __BLOCK_MAP, tbl, flags, phys, start, end
        lsr     \start, \start, #PMD_SHIFT
        and     \start, \start, (NUM_ENTRIES_PER_TABLE - 1)
        lsr     \end, \end, #PMD_SHIFT
        and     \end, \end, (NUM_ENTRIES_PER_TABLE - 1)
        lsr     \phys, \phys, #PMD_SHIFT
        orr     \phys, \flags, \phys, lsl #PMD_SHIFT
     9998:
        str     \phys, [\tbl, \start, lsl #3]
        add     \start, \start, #1
        add     \phys, \phys, #SECTION_SIZE
        cmp     \start, \end
        b.ls    9998b
    .endm

    .macro __ZERO_PAGE_TABLE, start, end
        adrp    x0, \start
        adrp    x1, \end
        sub     x1, x1, x0
    9999:
        stp     xzr, xzr, [x0], #16
        stp     xzr, xzr, [x0], #16
        stp     xzr, xzr, [x0], #16
        stp     xzr, xzr, [x0], #16
        subs    x1, x1, #64
        b.ne    9999b
    .endm

    .macro __INVAL_D_CACHE, start, end
        adrp    x0, \start
        adrp    x1, \end
        sub     x1, x1, x0
        bl      __invalidate_dcache_range
    .endm

.section ".text.boot"

.globl __entry
__entry:
    bl      __secure_board_specific_setup
    adr     x0, __el1entry
    msr     elr_el3, x0
    eret

__el1entry:
    mrs     x0, mpidr_el1
    and     x0, x0, #MPIDR_HWID_MASK_LITE
    cbz     x0, __primary_init
    b       __secondary_spin

__secondary_spin:
    wfe
    adrp    x1, cpu_spin_pen
    ldr     x2, [x1, x0, lsl #3]
    cbz     x2, __secondary_spin
    b       __secondary_init

__run:
    __ADR_L     x0, vectors
    msr         vbar_el1, x0
    adrp        x13, init_stack
    add         sp, x13, #INIT_STACK_SIZE
    __ADR_L     x0, bss_begin
    __ADR_L     x1, bss_end
    sub         x1, x1, x0
    bl          __zerobss
    mov         x29, xzr
    mov         x30, xzr
    b           cheesecake_main

__secondary_run:
    __ADR_L     x0,vectors
    msr         vbar_el1, x0
    mrs         x0, mpidr_el1
    and         x0, x0, ALL_CPUS_MASK
    __ADR_L     x1, cpu_spin_pen
    mov         x2, CPU_INITIALIZED
    str         x2, [x1, x0, lsl #3]
    __ADR_L     x1, idle_stacks
    ldr         x2, [x1, x0, lsl #3]
    mov         sp, x2
    mov         x29, xzr
    mov         x30, xzr
    b           secondary_main

__zerobss:
    str     xzr, [x0], #8
    subs    x1, x1, #8
    b.hi    __zerobss
    ret

.section ".idmap.text"

__primary_init:
    bl      __create_page_tables
    bl      __setup_mem_attrs
    bl      __setup_trans_ctrl
    bl      __turnon_mmu
    ldr     x8, =__run
    br      x8

__secondary_init:
    bl      __setup_mem_attrs
    bl      __setup_trans_ctrl
    bl      __turnon_mmu
    ldr     x8, =__secondary_run
    br      x8

__create_page_tables:
    mov                 x28, x30
    __INVAL_D_CACHE     page_idmap_dir, end_page_idmap_dir
    __INVAL_D_CACHE     page_global_dir, end_page_global_dir
    __ZERO_PAGE_TABLE   page_idmap_dir, end_page_idmap_dir
    __ZERO_PAGE_TABLE   page_global_dir, end_page_global_dir
    mov                 x7, NORMAL_INIT_MMU_FLAGS
    adrp                x0, page_idmap_dir
    adrp                x3, _idmap_text_start
    __CREATE_PGD        x0, x3, x5, x6
    mov                 x5, x3
    __ADR_L             x6, _idmap_text_end
    __BLOCK_MAP         x0, x7, x3, x5, x6
    adrp                x0, page_global_dir
    __MOV_Q             x5, VADDR_START + TEXT_OFFSET
    __CREATE_PGD        x0, x5, x3, x6
    adrp                x6, _end_permenant_image
    adrp                x3, _start
    sub                 x6, x6, x3
    add                 x6, x6, x5
    __BLOCK_MAP         x0, x7, x3, x5, x6
    dmb                 sy
    tlbi                vmalle1
    dsb                 sy
    isb
    __INVAL_D_CACHE     page_idmap_dir, end_page_idmap_dir
    __INVAL_D_CACHE     page_global_dir, end_page_global_dir
    mov                 x30, x28
    ret

__setup_mem_attrs:
    __MOV_Q     x5, MAIR_REGISTER_VALUE
    msr         mair_el1, x5
    dsb         sy
    isb
    ret

__setup_trans_ctrl:
    __MOV_Q     x5, TCR_INIT_CONFIG
    msr         tcr_el1, x5
    dsb         sy
    isb
    ret

__turnon_mmu:
    adrp    x1, page_idmap_dir
    adrp    x2, page_global_dir
    msr     ttbr0_el1, x1
    msr     ttbr1_el1, x2
    dsb     sy
    isb
    mrs     x0, sctlr_el1
    orr     x0, x0, #MMU_M_FLAG
    orr     x0, x0, #CACHE_C_FLAG
    orr     x0, x0, #CACHE_I_FLAG
    msr     sctlr_el1, x0
    dsb     sy
    isb
    ic      iallu
    dsb     sy
    isb
    ret

