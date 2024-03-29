/*
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

#define L1_CACHE_SHIFT  (6)

dstin   .req    x0
src     .req    x1
count   .req    x2
tmp1    .req    x3
tmp1w   .req    w3
tmp2    .req    x4
tmp2w   .req    w4
dst     .req    x6

A_l     .req    x7
A_h     .req    x8
B_l     .req    x9
B_h     .req    x10
C_l     .req    x11
C_h     .req    x12
D_l     .req    x13
D_h     .req    x14

    .macro ldrb1 ptr, regB, val
    ldrb  \ptr, [\regB], \val
    .endm

    .macro strb1 ptr, regB, val
    strb \ptr, [\regB], \val
    .endm

    .macro ldrh1 ptr, regB, val
    ldrh  \ptr, [\regB], \val
    .endm

    .macro strh1 ptr, regB, val
    strh \ptr, [\regB], \val
    .endm

    .macro ldr1 ptr, regB, val
    ldr \ptr, [\regB], \val
    .endm

    .macro str1 ptr, regB, val
    str \ptr, [\regB], \val
    .endm

    .macro ldp1 ptr, regB, regC, val
    ldp \ptr, \regB, [\regC], \val
    .endm

    .macro stp1 ptr, regB, regC, val
    stp \ptr, \regB, [\regC], \val
    .endm

.weak memcpy
.globl memcpy
memcpy:
    mov     dst, dstin
    cmp     count, #16
    b.lo    .Ltiny15
    neg     tmp2, src
    ands    tmp2, tmp2, #15
    b.eq    .LSrcAligned
    sub     count, count, tmp2
    tbz     tmp2, #0, 1f
    ldrb1   tmp1w, src, #1
    strb1   tmp1w, dst, #1
1:
    tbz     tmp2, #1, 2f
    ldrh1   tmp1w, src, #2
    strh1   tmp1w, dst, #2
2:
    tbz     tmp2, #2, 3f
    ldr1    tmp1w, src, #4
    str1    tmp1w, dst, #4
3:
    tbz     tmp2, #3, .LSrcAligned
    ldr1    tmp1, src, #8
    str1    tmp1, dst, #8
.LSrcAligned:
    cmp     count, #64
    b.ge    .Lcpy_over64
.Ltail63:
    ands    tmp1, count, #0x30
    b.eq    .Ltiny15
    cmp     tmp1w, #0x20
    b.eq    1f
    b.lt    2f
    ldp1    A_l, A_h, src, #16
    stp1    A_l, A_h, dst, #16
1:
    ldp1    A_l, A_h, src, #16
    stp1    A_l, A_h, dst, #16
2:
    ldp1    A_l, A_h, src, #16
    stp1    A_l, A_h, dst, #16
.Ltiny15:
    tbz     count, #3, 1f
    ldr1    tmp1, src, #8
    str1    tmp1, dst, #8
1:
    tbz     count, #2, 2f
    ldr1    tmp1w, src, #4
    str1    tmp1w, dst, #4
2:
    tbz     count, #1, 3f
    ldrh1   tmp1w, src, #2
    strh1   tmp1w, dst, #2
3:
    tbz     count, #0, .Lexitfunc
    ldrb1   tmp1w, src, #1
    strb1   tmp1w, dst, #1
    b       .Lexitfunc
.Lcpy_over64:
    subs    count, count, #128
    b.ge    .Lcpy_body_large
    ldp1    A_l, A_h, src, #16
    stp1    A_l, A_h, dst, #16
    ldp1    B_l, B_h, src, #16
    ldp1    C_l, C_h, src, #16
    stp1    B_l, B_h, dst, #16
    stp1    C_l, C_h, dst, #16
    ldp1    D_l, D_h, src, #16
    stp1    D_l, D_h, dst, #16
    tst     count, #0x3f
    b.ne    .Ltail63
    b       .Lexitfunc
.p2align    L1_CACHE_SHIFT
.Lcpy_body_large:
    ldp1    A_l, A_h, src, #16
    ldp1    B_l, B_h, src, #16
    ldp1    C_l, C_h, src, #16
    ldp1    D_l, D_h, src, #16
1:
    stp1    A_l, A_h, dst, #16
    ldp1    A_l, A_h, src, #16
    stp1    B_l, B_h, dst, #16
    ldp1    B_l, B_h, src, #16
    stp1    C_l, C_h, dst, #16
    ldp1    C_l, C_h, src, #16
    stp1    D_l, D_h, dst, #16
    ldp1    D_l, D_h, src, #16
    subs    count, count, #64
    b.ge    1b
    stp1    A_l, A_h, dst, #16
    stp1    B_l, B_h, dst, #16
    stp1    C_l, C_h, dst, #16
    stp1    D_l, D_h, dst, #16
    tst     count, #0x3f
    b.ne    .Ltail63
.Lexitfunc:
    ret
