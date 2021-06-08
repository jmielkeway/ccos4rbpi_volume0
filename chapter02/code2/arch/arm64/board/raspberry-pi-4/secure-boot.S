/*
 * Copyright (c) 2016-2019 Raspberry Pi (Trading) Ltd.
 * Copyright (c) 2016 Stephen Warren <swarren@wwwdotorg.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of the copyright holder nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

#include "arch/bare-metal.h"
#include "arch/linux-extension.h"
#include "board/devio.h"
#include "board/gic.h"

#define LOCAL_CONTROL       0x4c0000000
#define LOCAL_PRESCALER     0x4c0000008

#define OSC_FREQ            54000000

#define L2CTLR_EL1          s3_1_c11_c0_2
#define L2CTLR_EL1_VALUE    0b00100010

#define CPUECTLR_EL1        s3_1_c15_c2_1
#define CPUECTLR_EL1_SMPEN  BIT_SET(6)

#define ACTLR_EL3_VALUE     (BIT_SET(6) | \
                             BIT_SET(5) | \
                             BIT_SET(4) | \
                             BIT_SET(1) | \
                             BIT_SET(0))

#define SCR_EL3_RW          BIT_SET(10)
#define SCR_EL3_SMD         BIT_SET(7)
#define SCR_EL3_RES1_5      BIT_SET(5)
#define SCR_EL3_RES1_4      BIT_SET(4)
#define SCR_EL3_NS          BIT_SET(0)
#define SCR_EL3_VALUE       (SCR_EL3_RW | \
                             SCR_EL3_SMD | \
                             SCR_EL3_RES1_5 | \
                             SCR_EL3_RES1_4 | \
                             SCR_EL3_NS)

#define SPSR_EL3_D          BIT_SET(9)
#define SPSR_EL3_A          BIT_SET(8)
#define SPSR_EL3_I          BIT_SET(7)
#define SPSR_EL3_F          BIT_SET(6)
#define SPSR_EL3_EL1h       0b0101
#define SPSR_EL3_VALUE      (SPSR_EL3_D | \
                             SPSR_EL3_A | \
                             SPSR_EL3_I | \
                             SPSR_EL3_F | \
                             SPSR_EL3_EL1h)

#define HCR_EL2_RW          BIT_SET(31)
#define HCR_EL2_VALUE       HCR_EL2_RW

#define SCTLR_EL1_RES1_29   BIT_SET(29)
#define SCTLR_EL1_RES1_28   BIT_SET(28)
#define SCTLR_EL1_RES1_23   BIT_SET(23)
#define SCTLR_EL1_RES1_22   BIT_SET(22)
#define SCTLR_EL1_RES1_20   BIT_SET(20)
#define SCTLR_EL1_RES1_11   BIT_SET(11)
#define SCTLR_EL1_VALUE     (SCTLR_EL1_RES1_29 | \
                             SCTLR_EL1_RES1_28 | \
                             SCTLR_EL1_RES1_23 | \
                             SCTLR_EL1_RES1_22 | \
                             SCTLR_EL1_RES1_20 | \
                             SCTLR_EL1_RES1_11)

.section ".text.boot"

.globl __secure_board_specific_setup
__secure_board_specific_setup:
    mov     x28, x30
    bl      __setup_local_control
    bl      __setup_cortex_a72_regs
    bl      __setup_arm_arch_regs
    bl      __setup_gic
    mov     x30, x28
    ret

__setup_local_control:
    ldr     x0, =LOCAL_CONTROL
    str     wzr, [x0]
    mov     w1, 0x80000000
    str     w1, [x0, #(LOCAL_PRESCALER - LOCAL_CONTROL)]
    dsb     sy
    isb
    ret

__setup_cortex_a72_regs:
    mrs     x0, L2CTLR_EL1
    mov     x1, L2CTLR_EL1_VALUE
    orr     x0, x0, x1
    msr     L2CTLR_EL1, x0
    mov     x0, #CPUECTLR_EL1_SMPEN
    msr     CPUECTLR_EL1, x0
    ldr     x0, =OSC_FREQ
    msr     cntfrq_el0, x0
    msr     cntvoff_el2, xzr
    mov     x0, 0x33ff
    msr     cptr_el3, x0
    mov     x0, #ACTLR_EL3_VALUE
    msr     actlr_el3, x0
    dsb     sy
    isb
    ret

__setup_arm_arch_regs:
    ldr     x0, =SCR_EL3_VALUE
    msr     scr_el3, x0
    ldr     x0, =HCR_EL2_VALUE
    msr     hcr_el2, x0
    ldr     x0, =SCTLR_EL1_VALUE
    msr     sctlr_el1, x0
    ldr     x0, =SPSR_EL3_VALUE
    msr     spsr_el3, x0
    dsb     sy
    isb
    ret

__setup_gic:
    mrs             x0, mpidr_el1
    tst             x0, #3
    __MOV_Q         x2, GICD_IGROUPR_OFFSET
    b.ne            2f
    mov             w0, #3
    __MOV_Q         x1, GICD_CTLR
    __DEV_WRITE_32  w0, x1
    mov             w0, #~(0)
    mov             w1, #(7 * 4)
1:
    subs            w1, w1, #4
    add             x3, x2, x1
    __DEV_WRITE_32  w0, x3
    b.ne            1b
2:
    mov             w0, #~(0)
    __DEV_WRITE_32  w0, x2
    mov             w0, #0x1e7
    __MOV_Q         x1, GICC_CTLR
    __DEV_WRITE_32  w0, x1
    mov             w0, #0xFF
    __MOV_Q         x1, GICC_PMR
    __DEV_WRITE_32  w0, x1
    dsb             sy
    isb
    ret

