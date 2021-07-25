*Previous Chapter* [Chapters[5]: SMP](../chapter05/chapter5.md) | *Next Chapter* [Chapters[7]: Scheduling and Processes](../chapter07/chapter7.md)  
*Previous Page* [Atomics and Ordering](../chapter05/atomics-ordering.md)  |  *Next Page* [The Global Memory Map](global-memmap.md)

## Chapters[6]: Memory Allocation

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
│       │       └── smp.h
│       ├── irq.S
│       ├── linker.template
│       ├── lock.S
│       ├── lock.c
│       ├── main.S
│       ├── memset.S
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
│       ├── list.h
│       ├── lock.h
│       ├── log.h
│       └── types.h
└── src
    ├── allocate.c
    ├── cheesecake.c
    └── log.c
```

With our CheesecakeOS booted, we can begin to provide all kinds of kernel services. A memory allocator can be constructed independent of other services. Many other services cannot be constructed without access to dynamic memory allocation. So here we start!

- [The Global Memory Map](global-memmap.md) keeps track of all pages in the system. We start by implementing a list structure we will use ubiquitously.
- [Binary Buddies](binary-buddies.md) is an allocation system that allows free blocks of memory to be split in two to satisfy memory requests, then later coalesced.
- [Slab Allocator with Object Caches](slab.md) will bring our allocator to the next level...similar in concept to Linux!

*Previous Page* [Atomics and Ordering](../chapter05/atomics-ordering.md)  |  *Next Page* [The Global Memory Map](global-memmap.md)  
*Previous Chapter* [Chapters[5]: SMP](../chapter05/chapter5.md) | *Next Chapter* [Chapters[7]: Scheduling and Processes](../chapter07/chapter7.md)
