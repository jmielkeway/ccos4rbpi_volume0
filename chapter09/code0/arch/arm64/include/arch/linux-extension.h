/*
 * Copyright (C) 1996-2000 Russell King
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

#ifndef _ARCH_LINUX_EXTENSION_H
#define _ARCH_LINUX_EXTENSION_H

    .macro __MOV_Q, reg, val
        .if     (((\val) >> 31) == 0 || ((\val) >> 31) == 0x1ffffffff)
        movz    \reg, :abs_g1_s:\val
        .else
        .if     (((\val) >> 47) == 0 || ((\val) >> 47) == 0x1ffff)
        movz    \reg, :abs_g2_s:\val
        .else
        movz    \reg, :abs_g3:\val
        movk    \reg, :abs_g2_nc:\val
        .endif
        movk    \reg, :abs_g1_nc:\val
        .endif
        movk    \reg, :abs_g0_nc:\val
    .endm

    .macro  __ADR_L, dst, sym
        adrp    \dst, \sym
        add     \dst, \dst, :lo12:\sym
    .endm

#endif
