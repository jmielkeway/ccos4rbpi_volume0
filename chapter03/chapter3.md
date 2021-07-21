*Previous Chapter* [Chapters[2]: Processor Initialization and Exceptions](../chapter02/chapter2.md) | *Next Chapter* [Chapters[4]: Caches](../chapter04/chapter4.md)  
*Previous Page* [Timer Interrupts](../chapter02/timer-interrupts.md)  |  *Next Page* [ARM Page Tables](arm-page-tables.md)

## Chapters[3]: Memory Management Unit

#### What We're Baking With

```bash
ccos4rbpi:~$ tree
.
├── Makefile
├── arch
│   └── arm64
│       ├── allocate.c
│       ├── barrier.S
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
│       │       ├── memmap.c
│       │       ├── mini-uart.S
│       │       ├── mini-uart.c
│       │       ├── secure-boot.S
│       │       ├── timer.S
│       │       └── timer.c
│       ├── cache.S
│       ├── entry.S
│       ├── error.c
│       ├── exec
│       │   └── asm-offsets.c
│       ├── include
│       │   └── arch
│       │       ├── bare-metal.h
│       │       ├── cache.h
│       │       ├── irq.h
│       │       ├── linux-extension.h
│       │       ├── memory.h
│       │       ├── page.h
│       │       ├── process.h
│       │       └── prot.h
│       ├── irq.S
│       ├── linker.template
│       ├── main.S
│       ├── memset.S
│       └── sync.c
├── build.sh
├── cheesecake.conf
├── config
│   └── config.py
├── include
│   └── cake
│       ├── log.h
│       └── types.h
└── src
    ├── cheesecake.c
    └── log.c
```

Virtual memory is a fundamental abstraction of an operating system. But the efficient use of the virtual memory abstraction requires hardware support. In this chapter, we setup the ARM Memory Management Unit. By the end of the chapter, our system's MMU will be turned on, and all subsequent memory references will be to virtual addresses.

- [ARM Page Tables](arm-page-tables.md) covers the basics of how virtual memory is implemented and configured in ARM - through the use of page tables.
- [Establishing Boot Tables](boot-tables.md) has our CheesecakeOS bootstrapping the MMU with the first statically allocated page tables.
- In [Linear Mapping the Entire Physical Address Space](linear-mapping.md) we establish a boot-time allocator to handle primitive memory requests, and create a complete linear mapping of the BCM2711 memory map.
- [Turning on the MMU](mmu.md) will be the gelatinous reward for our hard work in this difficult chapter.

*Previous Page* [Timer Interrupts](../chapter02/timer-interrupts.md)  |  *Next Page* [ARM Page Tables](arm-page-tables.md)  
*Previous Chapter* [Chapters[2]: Processor Initialization and Exceptions](../chapter02/chapter2.md) | *Next Chapter* [Chapters[4]: Caches](../chapter04/chapter4.md)
