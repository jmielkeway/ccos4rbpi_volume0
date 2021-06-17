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

#define TICKET_SHIFT    (32)
#define TICKET_VALUE    (1 << TICKET_SHIFT)

.globl __irq_restore
__irq_restore:
    msr  daif, x0
    ret

.globl __irq_save
__irq_save:
    mrs     x0, daif
    msr     daifset, #2
    ret

.globl __spin_lock
__spin_lock:
    prfm	pstl1strm, [x0]
1:
    ldaxr   x3, [x0]
    mov     x6, #TICKET_VALUE
    add     x1, x3, x6
    stxr    w2, x1, [x0]
    cbnz    w2, 1b
    eor     x1, x3, x3, ror #32
    cbz     x1, 3f
    sevl
2:
    wfe
    ldaxr   w2, [x0]
    eor     x1, x2, x3, lsr #32
    cbnz    x1, 2b
3:
    ret

.globl __spin_unlock
__spin_unlock:
    ldr     w1, [x0]
    add     w1, w1, #1
    stlr    w1, [x0]
    ret
