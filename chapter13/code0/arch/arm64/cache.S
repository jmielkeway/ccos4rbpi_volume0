/*
 *
 * Copyright (C) 2001 Deep Blue Solutions Ltd.
 * Copyright (C) 2012 ARM Ltd.
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

.globl __clean_and_inval_dcache_range
__clean_and_inval_dcache_range:
    add     x1, x1, x0
    mrs     x3, ctr_el0
    ubfm    x3, x3, #16, #19
    mov     x2, #4
    lsl     x2, x2, x3
1:
    dc      civac, x0
    add     x0, x0, x2
    cmp     x0, x1
    b.lo    1b
    dsb     sy
    isb
    ret

.globl __flush_icache_range
__flush_icache_range:
    mrs     x3, ctr_el0
    ubfm    x3, x3, #16, #19
    mov     x2, #4
    lsl     x2, x2, x3
    sub     x3, x2, #1
    bic     x4, x0, x3
1:
    dc      cvau, x4
    dc      civac,x4
    add     x4, x4, x2
    cmp     x4, x1
    b.lo    1b
    dsb     ish
    mrs     x3, ctr_el0
    and     x3, x3, #0b1111
    mov     x2, #4
    lsl     x2, x2, x3
    sub     x3, x2, #1
    bic     x4, x0, x3
2:
    ic      ivau, x4
    add     x4, x4, x2
    cmp     x4, x1
    b.lo    2b
    dsb     ish
    isb
    ret

.globl __invalidate_dcache_range
__invalidate_dcache_range:
    add     x1, x1, x0
    mrs     x3, ctr_el0
    ubfm    x3, x3, #16, #19
    mov     x2, #4
    lsl     x2, x2, x3
    sub     x3, x2, #1
    tst     x1, x3
    bic     x1, x1, x3
    b.eq    1f
    dc      civac, x1
1:  
    tst     x0, x3
    bic     x0, x0, x3
    b.eq    2f
    dc      civac, x0
    b       3f
2:  
    dc      ivac, x0
3:  
    add     x0, x0, x2
    cmp     x0, x1
    b.lo    2b
    dsb     sy
    ret

.globl __tlbi_aside1is
__tlbi_aside1is:
    dsb     ishst
    tlbi    aside1is, x0
    dsb     ish
    isb
    ret

.globl __tlbi_vmalle1
__tlbi_vmalle1:
    dsb     ishst
    tlbi    vmalle1
    dsb     ish
    isb
    ret
