*Previous Chapter* [Chapters[6]: Memory Allocation](../chapter06/chapter6.md) | *Next Chapter* [Chapters[8]: Working and Waiting](../chapter08/chapter8.md)  
*Previous Page* [Slab Allocator with Object Caches](../chapter06/slab.md)  |  *Next Page* [Processes and PIDs](processes.md)

## Chapters[7]: Scheduling and Processes

#### What We're Baking With

```bash
ccos4rbpi:~$ tree
.
├── Makefile
├── arch
│   └── arm64
│       ├── allocate.c
│       ├── atomic.S
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
│       ├── event.S
│       ├── exec
│       │   └── asm-offsets.c
│       ├── include
│       │   └── arch
│       │       ├── allocate.h
│       │       ├── atomic.h
│       │       ├── bare-metal.h
│       │       ├── barrier.h
│       │       ├── cache.h
│       │       ├── irq.h
│       │       ├── linux-extension.h
│       │       ├── lock.h
│       │       ├── memory.h
│       │       ├── page.h
│       │       ├── process.h
│       │       ├── prot.h
│       │       ├── schedule.h
│       │       └── smp.h
│       ├── irq.S
│       ├── linker.template
│       ├── lock.S
│       ├── main.S
│       ├── memcpy.S
│       ├── memset.S
│       ├── process.c
│       ├── schedule.S
│       ├── smp.S
│       ├── smp.c
│       └── sync.c
├── build.sh
├── cheesecake.conf
├── config
│   └── config.py
├── include
│   └── cake
│       ├── allocate.h
│       ├── bitops.h
│       ├── cake.h
│       ├── compiler.h
│       ├── error.h
│       ├── fork.h
│       ├── list.h
│       ├── lock.h
│       ├── log.h
│       ├── process.h
│       ├── schedule.h
│       └── types.h
└── src
    ├── allocate.c
    ├── cheesecake.c
    ├── fork.c
    ├── log.c
    ├── pid.c
    └── schedule.c
```

By the conclusion of this chapter, we will have coded an operational scheduler capable of context switching. We will also have working kernel implementation of process cloning, allowing us to exhibit the birth of new processes.

- In [Processes and PIDs](processes.md) we introduce the structures used by the scheduler, as well as a pid-assignment module.
- [Scheduling with Runqueues](scheduler.md) takes us through the scheduling and context-switching algorithms.
- [What the Fork!?](fork.md) is the culmination of much of the volume thus far, producing a cloning function to dynamically create new processes.

*Previous Page* [Slab Allocator with Object Caches](../chapter06/slab.md)  |  *Next Page* [Processes and PIDs](processes.md)  
*Previous Chapter* [Chapters[6]: Memory Allocation](../chapter06/chapter6.md) | *Next Chapter* [Chapters[8]: Working and Waiting](../chapter08/chapter8.md)
