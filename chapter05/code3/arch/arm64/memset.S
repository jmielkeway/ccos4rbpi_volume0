/*
 * Copyright (C) 2013 ARM Ltd.
 * Copyright (C) 2013 Linaro.
 *
 * This code is based on glibc cortex strings work originally authored by Linaro
 * and re-licensed under GPLv2 for the Linux kernel. The original code can
 * be found @
 *
 * http://bazaar.launchpad.net/~linaro-toolchain-dev/cortex-strings/trunk/
 * files/head:/src/aarch64/
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

#define L1_CACHE_SHIFT  (6)

dstin       .req    x0
val         .req    w1
count       .req    x2
tmp1        .req    x3
tmp1w       .req    w3
tmp2        .req    x4
tmp2w       .req    w4
zva_len_x   .req    x5
zva_len     .req    w5
zva_bits_x  .req    x6

A_l         .req    x7
A_lw        .req    w7
dst         .req    x8
tmp3w       .req    w9
tmp3        .req    x9

.weak memset
.globl memset
memset:
    mov     dst, dstin
    and     A_lw, val, #0xFF
    orr     A_lw, A_lw, A_lw, lsl #8
    orr     A_lw, A_lw, A_lw, lsl #16
    orr     A_l, A_l, A_l, lsl #32
    cmp     count, #15
    b.hi    .Lover16_proc
    tbz     count, #3, 1f
    str     A_l, [dst], #8
1:
    tbz     count, #2, 2f
    str     A_lw, [dst], #4
2:
    tbz     count, #1, 3f
    strh    A_lw, [dst], #2
3:
    tbz     count, #0, 4f
    strb    A_lw, [dst]
4:
    ret
.Lover16_proc:
    neg     tmp2, dst
    ands    tmp2, tmp2, #15
    b.eq    .Laligned
    stp     A_l, A_l, [dst]
    sub     count, count, tmp2
    add     dst, dst, tmp2
.Laligned:
    cbz     A_l, .Lzero_mem
.Ltail_maybe_long:
    cmp     count, #64
    b.ge    .Lnot_short
.Ltail63:
    ands    tmp1, count, #0x30
    b.eq    3f
    cmp     tmp1w, #0x20
    b.eq    1f
    b.lt    2f
    stp     A_l, A_l, [dst], #16
1:
    stp     A_l, A_l, [dst], #16
2:
    stp     A_l, A_l, [dst], #16
3:
    ands    count, count, #15
    cbz     count, 4f
    add     dst, dst, count
    stp     A_l, A_l, [dst, #-16]
4:
    ret
.p2align L1_CACHE_SHIFT
.Lnot_short:
    sub     dst, dst, #16
    sub     count, count, #64
1:
    stp     A_l, A_l, [dst, #16]
    stp     A_l, A_l, [dst, #32]
    stp     A_l, A_l, [dst, #48]
    stp     A_l, A_l, [dst, #64]!
    subs    count, count, #64
    b.ge    1b
    tst     count, #0x3f
    add     dst, dst, #16
    b.ne    .Ltail63
.Lexitfunc:
    ret
.Lzero_mem:
    cmp     count, #63
    b.le    .Ltail63
    cmp     count, #128
    b.lt    .Lnot_short 
    mrs     tmp1, dczid_el0
    tbnz    tmp1, #4, .Lnot_short
    mov     tmp3w, #4
    and     zva_len, tmp1w, #15
    lsl     zva_len, tmp3w, zva_len
    ands    tmp3w, zva_len, #63
    b.ne    .Lnot_short
.Lzero_by_line:
    cmp     count, zva_len_x
    b.lt    .Lnot_short
    sub     zva_bits_x, zva_len_x, #1
    neg     tmp2, dst
    ands    tmp2, tmp2, zva_bits_x
    b.eq    2f
    sub     tmp1, count, tmp2
    cmp     tmp1, #64
    ccmp    tmp1, zva_len_x, #8, ge
    b.lt    .Lnot_short
    mov     count, tmp1
1:
    stp     A_l, A_l, [dst]
    stp     A_l, A_l, [dst, #16]
    stp     A_l, A_l, [dst, #32]
    subs    tmp2, tmp2, #64
    stp     A_l, A_l, [dst, #48]
    add     dst, dst, #64
    b.ge    1b
    add     dst, dst, tmp2
2:
    sub     count, count, zva_len_x
3:
    dc      zva, dst
    add     dst, dst, zva_len_x
    subs    count, count, zva_len_x
    b.ge    3b
    ands    count, count, zva_bits_x
    b.ne    .Ltail_maybe_long
    ret
