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

.globl __atomic64_fetch_or
__atomic64_fetch_or:
    prfm    pstl1strm, [x0]
1:
    ldxr    x2, [x0]
    orr     x3, x2, x1
    stlxr   w4, x3, [x0]
    cbnz    w4, 1b
    dmb     ish
    mov     x0, x2
    ret

.globl __atomic64_or
__atomic64_or:
    prfm    pstl1strm, [x0]
1:
    ldxr    x2, [x0]
    orr     x2, x2, x1
    stxr    w3, x2, [x0]
    cbnz    w3, 1b
    ret

.globl __atomic64_andnot
__atomic64_andnot:
    prfm    pstl1strm, [x0]
1:
    ldxr    x2, [x0]
    bic     x2, x2, x1
    stxr    w3, x2, [x0]
    cbnz    w3, 1b
    ret

.globl __cmpxchg_relaxed
__cmpxchg_relaxed:
1:
    ldxr    x3, [x0]
    eor     x4, x3, x1
    cbnz    x4, 2f
    stxr    w4, x2, [x0]
    cbnz    w4, 1b
2:
    mov     x0, x3
    ret
