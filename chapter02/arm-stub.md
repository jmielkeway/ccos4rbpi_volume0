*Chapter Top* [Chapters[2]: Processor Initialization and Exceptions](chapter2.md)  |  *Next Chapter* [Chapters[3]: Memory Management Unit](../chapter03/chapter3.md)  
*Previous Page* [Introduction to ARM Processor Initialization](arm-init.md)  |  *Next Page* [Exceptions and the Vector Table](exception-vector-table.md)

## Setting the Exception Level in the ARM Stub ([chapter2/code0](code0))

#### ARM Processor Configuration

The [arch/arm64/include/arch/bare-metal.h](code0/arch/arm64/include/arch/bare-metal.h) begins with one convenience macro to set a bit in the system registers we will be writing in this slice:

```C
#ifndef _ARCH_BARE_METAL_H
#define _ARCH_BARE_METAL_H

#define BIT_SET(pos)                (1 << (pos))

#endif
```

Before diving in to the dirty details of the ARM Stub, let us whisk through a gentle restatement of the roles of the ARM Processor Exception Levels:

| Exception Level | Purpose |
|:---------------:|:-------:|
| EL3 | Most privileged; secure mode; the reset state |
| EL2 | Hypervisor;  Used to implement virtualization |
| EL1 | Guest OS; Where CheesecakeOS Kernel lives |
| EL0 | User space applications |

In order to properly initialize our processors, we will have to set several system registers with sane values at boot time. The board-specific [arch/arm64/board/raspberry-pi-4/secure-boot.S](code0/arch/arm64/board/raspberry-pi-4/secure-boot.S) source is responsible for initializing the board and processors such that the remainder of the boot process can be handled by the architecture:

```asm
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
```

`SCR_EL3`, the _Secure Configuration Register_, is described on `pg. D10-2648` of the `ARM ARM`. The least significant bit of the register is labeled as bit zero. Initialization shows setting these bits:
- `SCR_EL3_RW` determines that `EL1`, our target Exception Level, will execute in the `AArch64` execution state.
- `SCR_EL3_SMD` disables secure monitor calls - once we leave `EL3`, there will be no returning.
- `SCR_EL3_RES1_5` and `SCR_EL3_RES1_4` are required by the architecture to always be set, or, reserved one.
- `SCR_EL3_NS` is the non-secure bit, indicating that we will be running in the _non-secure_ state (no access to _secure_ world).

```asm
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
```

`SPSR_EL3`, the `EL3` _Saved Program Status Register_ (`pg. C5-389`) may be initialized such that the processor is executing in a specified mode after completing an `eret` instruction:
- `SPSR_EL3_D`, `SPSR_EL3_A`, `SPSR_EL3_I`, and `SPSR_EL3_F` mask all the maskable exceptions, the acronym _DAIF_ we learned about previously. These bits are masked on reset, and setting the bits in `SPSR_EL3` ensures they will remain masked after `eret`.
- The `SPSR_EL3_EL1h` setting in the `M`(ode) bits lets the system know to execute at `EL1` with its own designated stack pointer when leaving `EL3` via `eret`.

```asm
#define HCR_EL2_RW          BIT_SET(31)
#define HCR_EL2_VALUE       HCR_EL2_RW
```

`HCR_EL2`, the _Hypervisor Configuration Register_, is described on `pg. D10-2487`:
- `HCR_EL2_RW` determines `EL1`, our target Exception Level, will execute in the `AArch64` execution state.

```asm
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
```

`SCTLR_EL1`, the _System Control Register_, is described on `pg. D10-2654` of the `ARM ARM`. For now only the required RES1 bits are set. Notably, caches and the Memory Management Unit are left disabled for now.

#### The ARM Stub

Given the configuration - the CheesecakeOS config.txt disables command-line tags, loads the image at address `0x0`, and always uses the High Peripheral memory map - we must provide our own ARM stub. The board-specific aspects of our secure boot are mostly borrowed from the [Raspberryi Pi ARM Stub](https://github.com/raspberrypi/tools/blob/master/armstubs/armstub8.S):

```asm
#include "arch/bare-metal.h"

#define LOCAL_CONTROL       0x4c0000000
#define LOCAL_PRESCALER     0x4c0000008

#define OSC_FREQ            54000000

#define L2CTLR_EL1          s3_1_c11_c0_2
#define L2CTLR_EL1_VALUE    0b00100010

#define CPUECTLR_EL1        s3_1_c15_c2_1
#define CPUECTLR_EL1_SMPEN  BIT_SET(6)

.globl __secure_board_specific_setup
__secure_board_specific_setup:
    mov     x28, x30
    bl      __setup_local_control
    bl      __setup_cortex_a72_regs
    bl      __setup_arm_arch_regs
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
```

There is a notable subtlety in the `__secure_board_specific_setup`, the saving and restoring of `x30` to and from `x28`. Of course, `x30` is the link register which must be used to return to the caller of `__secure_board_specific_setup`. With no compiler to generate the instructions to save the chain of links on the stack, however, each `bl` instruction from within `__secure_board_specific_setup` itself will mangle the link register. We use this trick to ensure safe harbor for the return on completion.

When the `__secure_board_specific_setup` routine returns to [arch/arm64/main.S](code0/arch/arm64/main.S), the system registers are properly setup to execute the `eret` instruction and place the processor in `EL1`. The only requirement from the architecture is to load the `__el1entry` address into `ELR_EL3` (`pg C5-351`) so the processor begins execution from the correct instruction:

```asm
.globl __entry
__entry:
    bl      __secure_board_specific_setup
    adr     x0, __el1entry
    msr     elr_el3, x0
    eret
```

Building and running CheesecakeOS will not look materially different. Maybe the terminal _feels_ creamier now the processor has been initialized:

![Raspberry Pi EL Cheesecake](images/0201_rpi4_exceptionlevel.png)

*Previous Page* [Introduction to ARM Processor Initialization](arm-init.md)  |  *Next Page* [Exceptions and the Vector Table](exception-vector-table.md)  
*Chapter Top* [Chapters[2]: Processor Initialization and Exceptions](chapter2.md)  |  *Next Chapter* [Chapters[3]: Memory Management Unit](../chapter03/chapter3.md)
