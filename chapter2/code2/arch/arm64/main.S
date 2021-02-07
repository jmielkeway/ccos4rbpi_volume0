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

#include "arch/linux-extension.h"

.section ".text.boot"

.globl __entry
__entry:
    msr     daifset, 0b1111
    bl      __secure_board_specific_setup
    adr     x0, __el1entry
    msr     elr_el3, x0
    eret

__el1entry:
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, __run
    b       __sleep

__sleep:
    wfe
    b       __sleep

__run:
    __ADR_L     x0, vectors
    msr         vbar_el1, x0
    adrp        x13, _end
    mov         sp, x13
    adr         x0, bss_begin
    adr         x1, bss_end
    bl          __zerobss
    bl          cheesecake_main
    b           __sleep

__zerobss:
    sub     x1, x1, x0
    str     xzr, [x0], #8
    subs    x1, x1, #8
    b.gt    __zerobss
    ret
