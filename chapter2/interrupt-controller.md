*Chapter Top* [Chapters[2]: Processor Initialization and Exceptions](chapter2.md)  |  *Next Chapter* [Chapters[3]: Processor Initialization and Exceptions](../chapter3/chapter3.md)   
*Previous Page* [Exceptions and the ARM Exception Vector Table](exception-vector-table.md)  |  *Next Page* [Timer Interrupts](timer-interrupts.md)

## The Interrupt Controller ([chapter2/code2](code2))

#### The ARM GIC

Our RaspberryPi 4 is equipped with an ARM Generic Interrupt Controller. The `GIC` is a complex interface with its own [specification](https://developer.arm.com/documentation/ihi0048/b?lang=en). Important registers are paritioned between the _Distributor_ and  each _CPU interface_. The Distributor block performs interrupt prioritization and distribution to the CPU interface blocks that connect to the processors in the system. Each CPU interface block performs priority masking and preemption handling for a connected processor in the system.

Some of the CPU interface registers are banked. This means that the register is mapped uniquely for a given CPU. On these banked registers, if we desire a value to be set for all CPUs, we must write that value to the register on each CPU.

#### What We're Baking With
```bash
ccos4rbpi:~$ tree
├── Makefile
├── arch
│   └── arm64
│       ├── board
│       │   └── raspberry-pi-4
│       │       ├── config.txt
│       │       ├── include
│       │       │   └── board
│       │       │       ├── devio.h
│       │       │       ├── gic.h
│       │       │       └── peripheral.h
│       │       ├── irq.S
│       │       ├── irq.c
│       │       ├── mini-uart.S
│       │       ├── mini-uart.c
│       │       └── secure-boot.S
│       ├── entry.S
│       ├── error.c
│       ├── exec
│       │   └── asm-offsets.c
│       ├── include
│       │   └── arch
│       │       ├── bare-metal.h
│       │       ├── linux-extension.h
│       │       ├── process.h
│       │       └── timing.h
│       ├── linker.ld
│       ├── main.S
│       └── timing.S
├── build.sh
├── include
│   └── cake
│       ├── log.h
│       └── types.h
└── src
    ├── cheesecake.c
    └── log.c
```

Our baby OS is beginning to grow up! The device usage and address calculation macros have been moved into header files to be shared. Support for IRQ handling has been baked into exisiting sources.

#### Configuring the GIC During Boot

Many of the GIC registers need to be configured with secure access. Since we initialize our kernel to operate in non-secure `EL1`, this must happen at boot time before we leave secure `EL3` in the `__sercure_board_specific_setup` routine from [arch/arm64/board/secure-boot.S](code2/arch/arm64/board/secure-boot.S):

```asm
.globl __secure_board_specific_setup
__secure_board_specific_setup:
    mov     x28, x30
    bl      __setup_local_control
    bl      __setup_cortex_a72_regs
    bl      __setup_arm_arch_regs
    bl      __setup_gic
    mov     x30, x28
    ret
```

The `__setup_gic` routine is implemented as follows:

```asm
__setup_gic:
    mrs             x0, mpidr_el1
    tst             x0, #3
    __MOV_Q         x2, GICD_IGROUPR_OFFSET
    b.ne            2f
```

For all CPUs, the `GICD_IGFROUPR_OFFSET` is moved into register `x2`. In a multiprocessor implementation, GICD_IGROUP0 is banked for each processor.

```asm
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

```

`CPU 0` performs the GIC Distributor initliazation. The value `3` is written to `GICD_CTLR`, setting the two least signifcant bits. This enables all interrupts, both secure and insecure, to be forwarded to the CPU interfaces. Afterwards, the loop block sets all interrupts to be _Group 1_, or insecure. This means our CPUs will receive the interrupts after they have entered non-secure `EL1`. This is achieved by writing the value `0xFFFF` to each of the `8` GICD_IGROUPn registers of the Distributor. The RaspberryPi 4 [ARM stub](https://github.com/raspberrypi/tools/blob/master/armstubs/armstub8.S) shows the same logic, with `8` registers and `32` configurable interrupts per register, for a total of `256` interrupt requests.

```asm
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
```

Since _GICD_IGROUP0_ is banked for each CPU, all CPUs initialize interrupts 0-31 in _Group 1_. Each *GICC_CTLR* register is initalized, allowing non-secure interrupts. Finally, each *GICC_PMR* register is initialized with the lowest priority filter, allowing all interrupts through.

#### Handling IRQs

Before inundating our system with interrupt requests, let's update our vector table to handle the exception. Since our CheesecakeOS currently executes at `EL1` with the SPx stack pointer, we know which vector table entry to update in [arch/arm64/entry.S](code2/arch/arm64/entry.S):

```asm
.align 11
.global vectors
vectors:
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __irq_el1h
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry

__irq_el1h:
    __ENTRY_SAVE
    bl  handle_irq
    __ENTRY_RESTORE
```

The work of the exception handler is taken care of by the board-specific `handle_irq` function in [arch/arm64/board/raspberry-pi-4/irq.c](code2/arch/arm64/board/raspberry-pi-4/irq.c):

```C
extern unsigned int  __irq_acknowledge();
extern void __irq_end(unsigned int irq);

void handle_irq()
{
    unsigned int irq = __irq_acknowledge();
    __irq_end(irq);
}
```

The handler doesn't do anything interesting yet, viewing the low level assembly functions in [arch/arm64/board/raspberry-pi-4/irq.S](code2/arch/arm64/board/raspberry-pi-4/irq.S):

```asm
.globl __irq_acknowledge
__irq_acknowledge:
    __MOV_Q         x0, GICC_IAR
    __DEV_READ_32   w0, x0
    ret

.globl __irq_end
__irq_end:
    __MOV_Q         x1, GICC_EOIR
    __DEV_WRITE_32  w0, x1
    ret
```

Currently then, when receiving an IRQ, we simply query the ID of the request from the *GICC_IAR* register, and comply with the GIC architectural requirement to write the ID to the *GICC_EOIR* register.

#### Saving and Restoring State

Returning to the IRQ handler, the `handle_irq` function is bookended by two macros, `__ENTRY_SAVE` and `__ENTRY_RESTORE`:

```asm
__irq_el1h:
    __ENTRY_SAVE
    bl  handle_irq
    __ENTRY_RESTORE
```

When the processor branches to the `handle_irq` function, the link register, `x30` is automatically updated with the return address. Also, we can say for certain that other general purpose registers will be mangled within the `handle_irq` function itself. If after returning from `handle_irq` we simply executed an `eret` instruction to return to the previous instruction, our general purpose registers may well be in a good state to crash the system. Instead, we must carefully save some required state before running our handler, and restore it after. This is the service performed by the `__ENTRY_SAVE` and `__ENTRY_RESTORE` macros. The information saved and restored will be in the form of a new structure declared in [arch/arm64/include/arch/process.h](code2/arch/arm64/include/arch/process.h):

```C
#include "cake/types.h"

struct stack_save_registers {
    u64  regs[31];
    u64  sp;
    u64  pc;
    u64  pstate;
};
```

This structure is analegous to `pt_regs` in Linux. There is room for general purpose registers `x0`-`x30`, the stack pointer, the program counter, and the processor state. This information will be saved on the stack, in order - from bottom to top because the stack grows down:

| Address | Data |
| :---   | :--- |
| Prev SP - `0x008` | `pstate` |
| Prev SP - `0x010` | `pc` |
| Prev SP - `0x018` | `sp` |
| Prev SP - `0x020` | `x30` |
| ***  | ... |
| Prev SP - sizeof(struct stack_save_registers) + `0x018`  | `x3` |
| Prev SP - sizeof(struct stack_save_registers) + `0x010`  | `x2` |
| Prev SP - sizeof(struct stack_save_registers) + `0x008`  | `x1` |
| Prev SP - sizeof(struct stack_save_registers)  | `x0` |

The save operation is then a matter of creating some space on the stack and storing the data we care about, in order. The restore operation is about pulling that stored data off the stack into the correct registers, and freeing the allocated stack space:

```asm
    .macro __ENTRY_SAVE
        sub     sp, sp, #STRUCT_STACK_SAVE_REGISTERS_SIZE
        stp     x0, x1, [sp, #16 * 0]
        stp     x2, x3, [sp, #16 * 1]
        stp     x4, x5, [sp, #16 * 2]
        stp     x6, x7, [sp, #16 * 3]
        stp     x8, x9, [sp, #16 * 4]
        stp     x10, x11, [sp, #16 * 5]
        stp     x12, x13, [sp, #16 * 6]
        stp     x14, x15, [sp, #16 * 7]
        stp     x16, x17, [sp, #16 * 8]
        stp     x18, x19, [sp, #16 * 9]
        stp     x20, x21, [sp, #16 * 10]
        stp     x22, x23, [sp, #16 * 11]
        stp     x24, x25, [sp, #16 * 12]
        stp     x26, x27, [sp, #16 * 13]
        stp     x28, x29, [sp, #16 * 14]
        add     x21, sp, #STRUCT_STACK_SAVE_REGISTERS_SIZE
        mrs     x22, elr_el1
        mrs     x23, spsr_el1
        stp     x30, x21, [sp, #16 * 15]
        stp     x22, x23, [sp, #16 * 16]
    .endm

    .macro __ENTRY_RESTORE
        ldp     x22, x23, [sp, #16 * 16]
        msr     elr_el1, x22
        msr     spsr_el1, x23
        ldp     x0, x1, [sp, #16 * 0]
        ldp     x2, x3, [sp, #16 * 1]
        ldp     x4, x5, [sp, #16 * 2]
        ldp     x6, x7, [sp, #16 * 3]
        ldp     x8, x9, [sp, #16 * 4]
        ldp     x10, x11, [sp, #16 * 5]
        ldp     x12, x13, [sp, #16 * 6]
        ldp     x14, x15, [sp, #16 * 7]
        ldp     x16, x17, [sp, #16 * 8]
        ldp     x18, x19, [sp, #16 * 9]
        ldp     x20, x21, [sp, #16 * 10]
        ldp     x22, x23, [sp, #16 * 11]
        ldp     x24, x25, [sp, #16 * 12]
        ldp     x26, x27, [sp, #16 * 13]
        ldp     x28, x29, [sp, #16 * 14]
        ldp     x30, xzr, [sp, #16 * 15]
        add     sp, sp, #STRUCT_STACK_SAVE_REGISTERS_SIZE
        eret
    .endm
```
The pstate that is saved is the value of the *SPSR_EL1* register, which the program counter is the value of the *ELR_EL1* register. The fact these registers are saved with the rest of the program state allows for correct handling of nested exceptions. The values are restored before the `eret` instruction is executed in `__ENTRY_RESTORE`. Notice also that the `struct stack_save_registers` must have a size that is a multiple of `16` bytes in order to satisfy the ARM architectural requirement that the stack pointer always be `16`-byte aligned.

#### Host Program

Funny enough, grepping through the code for the definition of the `STACK_SAVE_REGISTERS_SIZE` macro does not yield any results:

```bash
ccos4rbpi:~$ grep -r "#define STRUCT_STACK_SAVE_REGISTERS_SIZE"
ccos4rbpi:~$
```

This is because we do not want to define the size of this structure to be a static value - what if want to add some more fields to be saved to the stack later? Also, because this value is used in the assembly code, there is no access to the `sizeof` operator normally available in `C`. Instead, we create what is known as a `Host Program` - a program that executes on the host styem as part of the image building process - to leverage the advantages of the `C` compiler and dynamically generate a compile-time constant value for this macro. The definitions are created in [arch/arm64/exec/asm-offsets.c](code2/arch/arm64/exec/asm-offsets.c):

```C
#include <stdio.h>
#include "arch/process.h"

int main()
{
    printf("#ifndef _EXEC_ASM_OFFSETS_H\n");
    printf("#define _EXEC_ASM_OFFSETS_H\n");
    printf("\n");
    printf("#define %s \t\t\t%lu\n", "STRUCT_STACK_SAVE_REGISTERS_SIZE", sizeof(struct stack_save_registers));
    printf("\n");
    printf("#endif\n");
    return 0;
}
```

Updates have been made to the [Makefile](code2/Makefile) to generate a special file and use it as part of the build process:

```
$(ARCH_OBJ_DIR)/%_s.o: $(ARCH_SRC_DIR)/%.S
    mkdir -p $(@D)
    $(ARMCCC)-gcc $(CFLAGS) \
        -I$(KERNEL_INCLUDE_DIR) \
        -I$(ARCH_INCLUDE_DIR)   \
        -I$(EXEC_GEN_DIR)       \
        -MMD -c $< -o $@

exec: $(EXEC_GEN_DIR)/exec/asm-offsets.h

$(EXEC_GEN_DIR)/exec/asm-offsets.h: $(EXEC_GEN_DIR)/asm-offsets
    mkdir -p $(@D)
    $< > $@

$(EXEC_GEN_DIR)/asm-offsets: $(EXEC_OBJ_DIR)/asm-offsets.o
    mkdir -p $(@D)
    gcc -MMD $< -o $@

$(EXEC_OBJ_DIR)/asm-offsets.o: $(EXEC_SRC_DIR)/asm-offsets.c
    mkdir -p $(@D)
    gcc -I$(KERNEL_INCLUDE_DIR) \
        -I$(ARCH_INCLUDE_DIR)   \
        -MMD -c $< -o $@
```

The `exec` phony Make target is executed before the primary `all` target to ensure the generated macros are available for the main compilation. Returning to the definition of the `struct stack_save_registers`, a final subtle point:

```C
#include "cake/types.h"

struct stack_save_registers {
    u64  regs[31];
    u64  sp;
    u64  pc;
    u64  pstate;
};
```

The need to calculate the size of this structure outside the main compilation process is the reason for using the specific `u64` types in the structure definition. If the program were compiled with the ARM cross-compiler, the `long` type could be used. However, it is not certain that the `long` type will also be `64` bits on the host architecture. Specifying the size explictly should make it so.

Having added these nifty features, we can now initalize an the board-specific IRQ module, build, and run the image. If everything has gone right, it should look a little something like:

![Raspberry Pi IRQ Cheesecake](images/0203_rpi4_irq.png)

*Chapter Top* [Chapters[2]: Processor Initialization and Exceptions](chapter2.md)  |  *Next Chapter* [Chapters[3]: Processor Initialization and Exceptions](../chapter3/chapter3.md)   
*Previous Page* [Exceptions and the ARM Exception Vector Table](exception-vector-table.md)  |  *Next Page* [Timer Interrupts](timer-interrupts.md)
