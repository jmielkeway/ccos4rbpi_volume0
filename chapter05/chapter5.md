*Previous Chapter* [Chapters[4]: Caches](../chapter04/chapter4.md) | *Next Chapter* [Chapters[6]: Memory Allocation](../chapter06/chapter6.md)  
*Previous Page* [Enabling the Caches](../chapter04/caches.md)  |  *Next Page* [Synchronization](synchronization.md)

## Chapters[5]: SMP

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
│       ├── bitops.h
│       ├── compiler.h
│       ├── lock.h
│       ├── log.h
│       └── types.h
└── src
    ├── cheesecake.c
    └── log.c
```

Any modern operating system worth its salt can support symmetric multiprocessing. We expect nothing less from CheesecakeOS. Afterall, what is cheesecake without salt!? Supporting multiple processors at the lowest software level is a fraught exercise requiring advanced synchronization techniques and hours of spare time to spend debugging. We will not shy away from such a challenge. Strap in for the behind-the-scenes baking show down they won't carry on premium streaming services.

- In the [Synchronization](synchronization.md) slice, our secondary CPUs will be released from their sleep loop by the primary CPU, all working together to coordinate a safe startup.
- [The Cantankerous Cache Bug](cache-bug.md) is a nasty surprise we will understand and overcome in our quest to fire on all processors.
- [Spinlocks](spinlocks.md) will be a crucial arrow in our CheesecakeOS oven mitts. We will enjoy the implementation, and use it often.
- [Atomics and Ordering](atomics-ordering.md) are advanced concepts needing a good grokking. We will begin a library of functions and routines to serve us going forward.

*Previous Page* [Enabling the Caches](../chapter04/caches.md)  |  *Next Page* [Synchronization](synchronization.md)  
*Previous Chapter* [Chapters[4]: Caches](../chapter04/chapter4.md) | *Next Chapter* [Chapters[6]: Memory Allocation](../chapter06/chapter6.md)
