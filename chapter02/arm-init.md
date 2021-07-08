*Chapter Top* [Chapters[2]: Processor Initialization and Exceptions](chapter2.md)  |  *Next Chapter* [Chapters[3]: Memory Management Unit](../chapter03/chapter3.md)  
*Previous Page* [Chapters[2]: Processor Initialization and Exceptions](chapter2.md)  |  *Next Page* [Setting the Exception Level in the ARM Stub](arm-stub.md)

## Introduction to ARM Processor Initialization

#### ARM Processor Exception Levels

If you haven't spent much time working with the ARM architecture, the material in this chapter may not be intuitively accessible without a short introduction to the fundamental concepts built in to an ARMv8 processor. The best presentation of these concepts comes from the ARM documentation. The [ARM Cortex-A Programmer's Guide](https://developer.arm.com/documentation/den0024/a) is a great resource. 

In ARMv8, the processor is operating in one of four modes, called _exception levels_, or _EL_ for short. The mode is indicative of the privilege level of the processor. `EL0` is the lowest exception level with the least privilege and access. `EL3` is the highest exception level with the most privilege, and access to the entire system. At reset, the processor is executing at `EL3`.

There is a section on the [Fundamentals of ARMv8](https://developer.arm.com/documentation/den0024/a/Fundamentals-of-ARMv8) in the Programmer's Guide, which provides provides a summary of exception levels. To restate this documentation:

- `EL0` is the exception level for executing user programs.
- `EL1` is the exception level at which an OS kernel generally operates. It is the exception level at which CheesecakeOS operates.
- `EL2` is where a hypervisor will execute. A hypervisor is used to implement virtualization. So, if we wanted our OS to support guest operating systems (or virtual machines), a CheesecakeOS hypervisor host would live at `EL2`. We prefer to keep things _relatively_ simple, and do not support virtualization in this volume.
- `EL3` is where a secure monitor operates, and oversees the _secure_ and _non-secure_ worlds. If you are unfamiliar with secure vs non-secure, don't worry, so am I. CheesecakeOS will operate at non-secure `EL1`, and `EL3` does not appear in the Linux Kernel. We will ignore `EL3`, with the exception of the boot routine, because the processor begins in `EL3` after reset.

So, after we have configured our processor at boot time, we will be concerned only with `EL1` for our kernel, and `EL0` for user space applications. As explained in [Changing Exception levels](https://developer.arm.com/documentation/den0024/a/Fundamentals-of-ARMv8/Changing-Exception-levels), it is possible to decrease the exception level with use of the `eret` instruction, which we will examine further when discussing system registers. The only way to increase the exception level is through generating an exception. We will elaborate more on what exceptions are and how to generate them before the conclusion of this slice.

#### ARM Exception System Registers

Each ARM processor has a number of system registers, used to control and/or retrieve information from said processor. We have already encountered one system register, `MPIDR_EL1`, for determining a given processor's affinity, or ID. The trailing `EL1` in the register name indicates the minimum exception level at which the processor must be executing in order to access the system register. When executing in `EL0` there are only a handful of system registers the CPU can access. At `EL3` (and therefore at reset), the CPU can access any system register. Some system registers are read-only, such as `MPIDR_EL1`. Others allow software to write to them, applying or updating processor configuration.

In the course of dealing with exceptions and exception levels, we will encounter seven system registers:
1. The _Secure Configuration Register_, `SCR_EL3`.
2. The _Hypervisor Configuration Register_, `HCR_EL2`.
3. The _System Control Register_, `SCTLR_EL1`.
4. The _Saved Program Status Register_, `SPSR_ELx`.
5. The _Exception Link Register_, `ELR_ELx`.
6. The _Exception Syndrome Register_, `ESR_EL1`.
7. The _Vector Base Address Register_, `VBAR_EL1`.

The `ELx` indicates the version of the register from multiple exception levels, i.e. `SPSR_EL1` is distinct from `SPSR_EL3` and we encounter both. `SCR_EL3`, `HCR_EL2`, and `SCTLR_EL1` are configuration registers with various toggles and flags to setup our execution context. We will set their values at boot time, before branching to `cheesecake_main`. More intriguing to us at this moment are the other four system registers, all playing an essential role in implementing and processing exceptions.

An exception is an exceptional event from the processor's point of view, and there are a number of ways an exception is generated:
- An asynchronous event such as an interrupt request from peripheral hardware, such as when a user strikes a key at the keyboard.
- A synchronous fault such as accessing an unmapped region of memory.
- Executing instructions that are undefined or have been configured to be trapped.
- A system call from a user application. We add [system call functionality](../chapter09/syscall.md) in Chapter Nine.
- Others, but this list is sufficient for this volume.

When an exception is generated in a CheesecakeOS system, the current state of the processor is stored in the _Saved Program Status Register_, `SPSR_EL1`. The program counter is stored in the `Exception Link Register`, `ELR_EL1`. The _Exception Syndrome Register_, `ESR_EL1`, is loaded with a 32-bit binary encoding of the exception status or cause. Execution will jump to a special region configured and qualified to act as an exception handler. After handling the exception and when ready to return to the original context, the software issues the previously mentioned `eret` instruction. The `eret` instruction has the side effect of jumping to the instruction stored in the `ELR_EL1`, and restoring the processor state to what is stored in `SPSR_EL1`.

Two hints. First, the side effects of the `eret` instruction are not related to the exception context. The `eret` instruction can be used at any time to force the processor to take on the state in `SPSR_ELx`, and jump to the link in `ELR_ELx`. Second, both `SPSR_ELx` and `ELR_ELx` are writable. The least significant four bits in each `SPSR_ELx` register contain the mode of the processor. These bits can be adjusted such that when the `eret` instruction executes, the processor resumes in a _lower_ exception level.

#### ARM Exception Vector Table

Stepping back to exception generation, there are only four broad categories of exception: 
1. Synchronous exceptions, also called aborts, including invalid memory accesses, and system calls
2. Interrupt requests, known as IRQs
3. Fast IRQs, or high priority IRQs (not supported in CheesecakeOS or Linux)
4. Hardware faults (not supported in CheesecakeOS)

There are only four relative exception contexts for the ARM processor:
1. An exception from the current exception level when `SP0` is in use (not used in CheesecakeOS or Linux)
2. An exception from the current exception level when `SP1` is in use
3. An exception from a lower exception level executing in `AArch64`
4. An exception from a lower exception level executing in `AArch32` (not used in CheesecakeOS)

For a quick aside on `SP0` and `SP1`, `EL1` can be configured to use either the same stack pointer as `EL0` called `SP0`, or a separate dedicated stack pointer, `SP1`. We choose to have the separate exception levels use unique stack pointers in our implementation.

So, of the sixteen possible combinations of categories and contexts, CheesecakeOS support can be summarized as:

| Category | Context | Valid in CheesecakeOS? |
| :---     | :---    | :---:                  |
| Synchronous | Entry from Current Exception Level with `SP0` Stack Pointer | :x: |
| IRQ | Entry from Current Exception Level with `SP0` Stack Pointer | :x: |
| FIQ | Entry from Current Exception Level with `SP0` Stack Pointer | :x: |
| System Error | Entry from Current Exception Level with `SP0` Stack Pointer | :x: |
| Synchronous | Entry from Current Exception Level with `SPx` Stack Pointer | :heavy_check_mark: |
| IRQ | Entry from Current Exception Level with `SPx` Stack Pointer | :heavy_check_mark: |
| FIQ | Entry from Current Exception Level with `SPx` Stack Pointer | :x: |
| System Error | Entry from Current Exception Level with `SPx` Stack Pointer | :x: |
| Synchronous | Entry from Lower Exception Level Using `AArch64` | :heavy_check_mark: |
| IRQ | Entry from Lower Exception Level Using `AArch64` | :heavy_check_mark: |
| FIQ | Entry from Lower Exception Level Using `AArch64` | :x: |
| System Error | Entry from Lower Exception Level Using `AArch64` | :x: |
| Synchronous | Entry from Lower Exception Level Using `AArch32` | :x: |
| IRQ | Entry from Lower Exception Level Using `AArch32` | :x: |
| FIQ | Entry from Lower Exception Level Using `AArch32` | :x: |
| System Error | Entry from Lower Exception Level Using `AArch32` | :x: |

This table is exactly the structure ARM uses to determine where to jump when an exception is generated. The _Vector Base Address Register_,`VBAR_EL1` holds the base address of a 2KB aligned vector table of exception handlers. Each entry in the table is 128 bytes in size, or 32 instructions. Branching is allowed, though, so most entries are only a few instructions that end with a branch to the bona fide handler implementation. Depending on the combination of exception category and context, the processor will jump to an offset from `VBAR_EL1`. Our table can be redrawn with these offsets:

| VBAR\_EL1 Offset | Category | Context | Valid in CheesecakeOS? |
| :---             | :---     | :---    | :---:                  |
| 0x000 | Synchronous | Entry from Current Exception Level with `SP0` Stack Pointer | :x: |
| 0x080 | IRQ | Entry from Current Exception Level with `SP0` Stack Pointer | :x: |
| 0x100 | FIQ | Entry from Current Exception Level with `SP0` Stack Pointer | :x: |
| 0x180 | System Error | Entry from Current Exception Level with `SP0` Stack Pointer | :x: |
| 0x200 | Synchronous | Entry from Current Exception Level with `SPx` Stack Pointer | :heavy_check_mark: |
| 0x280 | IRQ | Entry from Current Exception Level with `SPx` Stack Pointer | :heavy_check_mark: |
| 0x300 | FIQ | Entry from Current Exception Level with `SPx` Stack Pointer | :x: |
| 0x380 | System Error | Entry from Current Exception Level with `SPx` Stack Pointer | :x: |
| 0x400 | Synchronous | Entry from Lower Exception Level Using `AArch64` | :heavy_check_mark: |
| 0x480 | IRQ | Entry from Lower Exception Level Using `AArch64` | :heavy_check_mark: |
| 0x500 | FIQ | Entry from Lower Exception Level Using `AArch64` | :x: |
| 0x580 | System Error | Entry from Lower Exception Level Using `AArch64` | :x: |
| 0x600 | Synchronous | Entry from Lower Exception Level Using `AArch32` | :x: |
| 0x580 | IRQ | Entry from Lower Exception Level Using `AArch32` | :x: |
| 0x700 | FIQ | Entry from Lower Exception Level Using `AArch32` | :x: |
| 0x780 | System Error | Entry from Lower Exception Level Using `AArch32` | :x: |

#### ARM Processor DAIF Mask

Each processor has a mask to toggle enabling and disabling certain exceptions. This mask is four bits wide and goes by the acronym `DAIF`, which stands for Debug Abort IRQ FIQ. On reset, all bits are set so all maskable exceptions are disabled. Exceptions of a given class can be enabled by clearing the corresponding _DAIF_ bit, and disabled by setting that same bit. There are two special purpose system registers to make the operation convenient: `DAIFClear` and `DAIFSet`. In CheesecakeOS we will only be concerned with IRQs. Enabling will go something like:

```asm
    msr     daifclr, #0b0010
```

And disabling:

```asm
    msr     daifset, #0b0010
```

When the processor takes an exception, the _DAIF_ mask is saved to `SPSR_EL1`. If the exception is an interrupt, the `I` flag of the processor will be masked. Thus, by default, when handling an interrupt request, further interrupts will be disabled unless we explicitly enable them. We can then carefully control when we want to allow for recursive IRQs. When executing an `eret` instruction, the _DAIF_ flags will be restored from `SPSR_EL1`, and if the `I` flag was cleared before the exception it will again be cleared at this time.

Let's now put these ideas into code. Refer back to this section, or, even better, the [ARM Cortex-A Programmer's Guide](https://developer.arm.com/documentation/den0024/a) as we move forward with the implementation.

*Previous Page* [Chapters[2]: Processor Initialization and Exceptions](chapter2.md)  |  *Next Page* [Setting the Exception Level in the ARM Stub](arm-stub.md)  
*Chapter Top* [Chapters[2]: Processor Initialization and Exceptions](chapter2.md)  |  *Next Chapter* [Chapters[3]: Memory Management Unit](../chapter03/chapter3.md)
