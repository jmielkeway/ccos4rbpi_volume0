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

.global __tlbi_vmalle1
__tlbi_vmalle1:
    dsb     ishst
    tlbi    vmalle1
    dsb     ish
    isb
    ret
