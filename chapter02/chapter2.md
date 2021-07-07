*Previous Chapter* [Chapters[1]: Hello, Cheesecake!](../chapter01/chapter1.md)  |  *Next Chapter* [Chapters[3]: Memory Management Unit](../chapter03/chapter3.md)  
*Previous Page* [Logging](../chapter01/logging.md)  |  *Next Page* [Introduction to ARM Processor Initialization](arm-init.md)

## Chapters[2]: Processor Initialization and Exceptions

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
│       │       │       ├── bare-metal.h
│       │       │       ├── devio.h
│       │       │       ├── gic.h
│       │       │       └── peripheral.h
│       │       ├── irq.S
│       │       ├── irq.c
│       │       ├── mini-uart.S
│       │       ├── mini-uart.c
│       │       ├── secure-boot.S
│       │       ├── timer.S
│       │       └── timer.c
│       ├── entry.S
│       ├── error.c
│       ├── exec
│       │   └── asm-offsets.c
│       ├── include
│       │   └── arch
│       │       ├── bare-metal.h
│       │       ├── irq.h
│       │       ├── linux-extension.h
│       │       └── process.h
│       ├── irq.S
│       ├── linker.ld
│       └── main.S
├── build.sh
├── include
│   └── cake
│       ├── log.h
│       └── types.h
└── src
    ├── cheesecake.c
    └── log.c
```

Our fledgling OS is pretty cool, epistemologically, but not particularly useful. The only recourse we have to get an instruction on a CPU is linear execution from the point of entry. We lack the means to _interrupt_ execution to jump to a different part of our image and do something cool. In this chapter, we lay down the graham-cracker-crust of CheesecakeOS. We get a bit ARM-y as we develop the infrastructure for processor interrupts - known colloquially as IRQs. We get a timer interrupt to fire at a pre-defined interval.

- [Introduction to ARM Processor Initialization](arm-init.md) gives a summary of the features and system registers forming the foundation of the chapter.
- [Setting the Exception Level in the ARM Stub](arm-stub.md) has us copying large sections of the Raspberry Pi ARM stub in order to set our processor at the correct ARM Exception Level (EL).
- In [Exceptions and the Vector Table](exception-vector-table.md) we take our first of multiple looks at the ARMv8 Vector Table for handling Exceptions.
- The [Generic Interrupt Controller](interrupt-controller.md) slice takes us deeper than we wanted to go into the ARM Generic Interrupt Controller (GIC), but not deeper than necessary.
- We conclude with [Timer Interrupts](timer-interrupts.md) as this chapter's apogee.

*Previous Page* [Logging](../chapter01/logging.md)  |  *Next Page* [Introduction to ARM Processor Initialization](arm-init.md)  
*Previous Chapter* [Chapters[1]: Hello, Cheesecake!](../chapter01/chapter1.md)  |  *Next Chapter* [Chapters[3]: Memory Management Unit](../chapter03/chapter3.md)
