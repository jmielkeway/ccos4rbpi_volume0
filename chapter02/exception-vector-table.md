*Chapter Top* [Chapters[2]: Processor Initialization and Exceptions](chapter2.md)  |  *Next Chapter* [Chapters[3]: Processor Initialization and Exceptions](../chapter3/chapter3.md)   
*Previous Page* [Setting the Exception Level in the ARM Stub](arm-stub.md)  |  *Next Page* [The Interrupt Controller](interrupt-controller.md)

## Exceptions and the Vector Table ([chapter2/code1](code1))

#### ARM Exception Handling Review

In the ARM architecture, exceptions are conditions or system events that require some action by privileged software - an _exception handler_ - to ensure correct functioning of the system. Reviewing what we have learned, in ARMv8 there are four categories of exceptions that occur in four different contexts. A handler is defined for each of the sixteen combinations. Each handler can be up to 32 instructions (128 bytes). Space is not a problem, however, as most handlers are only one instruction, which branches to a dedicated routine for doing the work. The handlers are placed at specified offsets from a base address. This base address is termed the _exception table_, or the _vector table_. Whenever the system encounters an exceptional event, the processor jumps to the correct offset based on the category of exception and context, and executes the instruction at that offset.

The base address is written to `VBAR_EL1`. The _Vector Base Address Register_ is explained on `pg. D10-2745` of the `ARM ARM`. The least significant eleven bits of the address are `RES0`, so the table must be installed at a 2KB, or 2048-byte aligned address. The math works as there are sixteen handlers multiplied by 128-bytes-per-handler. For further review, the layout of the table looks like:

| VBAR\_EL1 Offset | Exception Category | Context |
| :---        | :---           | :---    |
| 0x000 | Synchronous | Entry from Current Exception Level with `SP0` Stack Pointer |
| 0x080 | IRQ | Entry from Current Exception Level with `SP0` Stack Pointer |
| 0x100 | FIQ | Entry from Current Exception Level with `SP0` Stack Pointer |
| 0x180 | System Error | Entry from Current Exception Level with `SP0` Stack Pointer |
| 0x200 | Synchronous | Entry from Current Exception Level with `SPx` Stack Pointer |
| 0x280 | IRQ | Entry from Current Exception Level with `SPx` Stack Pointer |
| 0x300 | FIQ | Entry from Current Exception Level with `SPx` Stack Pointer |
| 0x380 | System Error | Entry from Current Exception Level with `SPx` Stack Pointer |
| 0x400 | Synchronous | Entry from Lower Exception Level Using `AArch64` |
| 0x480 | IRQ | Entry from Lower Exception Level Using `AArch64` |
| 0x500 | FIQ | Entry from Lower Exception Level Using `AArch64` |
| 0x580 | System Error | Entry from Lower Exception Level Using `AArch64` |
| 0x600 | Synchronous | Entry from Lower Exception Level Using `AArch32` |
| 0x580 | IRQ | Entry from Lower Exception Level Using `AArch32` |
| 0x700 | FIQ | Entry from Lower Exception Level Using `AArch32` |
| 0x780 | System Error | Entry from Lower Exception Level Using `AArch32` |

When handling exceptions, software can make use of three special registers setup by the architecture on exception entry:
1. `SPSR_EL1`, the _Saved Program Status Register_ explained on `pg. C5-377`, holds the saved process state when an exception is taken. The saved values include the DAIF exception mask, the values of the NCZV flags, and the Exception Level/Stack Pointer combination of the processor right before the exception. 
2. `ESR_EL1`, the _Exception Syndrome Register_ from `pg. D10-2436`, contains information about the exceptional condition or state. The software can use this information to determine which action to take next from within an exception handler. 
3. `ESR_EL1`, the _Exception Link Register_, holds the address of the return instruction to jump to after executing the `eret` instruction to end the handler routine.

#### Vector Table Definition

The [arch/arm64/entry.S](code1/arch/arm64/entry.S) source file is the primary additional ingredient in this slice:

```asm
    .macro __VECTABLE_ENTRY, label
        .align 7
        b \ label
    .endm

.align 11
.global vectors
vectors:
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
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry
    __VECTABLE_ENTRY    __invalid_entry

__invalid_entry:
__exception_hang:
    wfe
    b   __exception_hang
```

We define the simplest possible vector table, where all sixteen table entries branch to a degenerate infinite loop. At this juncture, the points of interest in the source file are the `.align` directives. The vector table itself is aligned on a 2048-byte boundary, and each table entry is aligned on a 128-byte boundary, thus satisfying the architectural requirements.

Installing the vector table happens back in [arch/arm64/main.S](arch/arm64/main.S):

```asm
    .macro __ADR_L, dst, sym
        adrp    \dst, \sym
        add     \dst, \dst, :lo12:\sym
    .endm

__run:
    __ADR_L     x0, vectors
    msr         vbar_el1, x0
    adrp        x13, _end
    mov         sp, x13
    adr         x0, bss_begin
    adr         x1, bss_end
    bl          __zerobss
    bl          cheesecake_main
    b           __sleep
```

The `__ADR_L` macro, like the `__MOV_Q` macro, is borrowed from [Linux](https://github.com/torvalds/linux/blob/v4.20/arch/arm64/include/asm/assembler.h#L211). It supports loading a pc-relative address that is not page-aligned, but with a larger range than the stand-alone `adr` instruction.

Sanity checking the memory map of our build, the `vectors` symbol is confirmed at the correct alignment. The next symbol after `vectors` is placed four bytes after the sixteenth table entry, due to the single four-byte instruction at that entry:

```bash
ccos4rbpi:~$ grep -A1 vectors .build/kernel8.map
0000000000000800 T vectors
0000000000000f84 t __exception_hang
```

Again, building and running doesn't display materially different output, but we've made another step towards having a recognizable computational machine:

![Raspberry Pi Vector Cheesecake](images/0202_rpi4_vector.png)


*Chapter Top* [Chapters[2]: Processor Initialization and Exceptions](chapter2.md)  |  *Next Chapter* [Chapters[3]: Processor Initialization and Exceptions](../chapter3/chapter3.md)   
*Previous Page* [Setting the Exception Level in the ARM Stub](arm-stub.md)  |  *Next Page* [The Interrupt Controller](interrupt-controller.md)
