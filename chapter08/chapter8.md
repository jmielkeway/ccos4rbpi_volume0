*Previous Chapter* [Chapters[7]: Scheduling and Processes](../chapter7/chapter7.md) | *Next Chapter* [Chapters[9]: Virtual Memory and Fake Partition](../chapter9/chapter9.md)  
*Previous Page* [What the Fork!?](../chapter7/fork.md)  |  *Next Page* [Waitqueues](waitqueues.md)

## Chapters[8]: Working and Waiting

#### What We're Baking With

```bash
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
│       ├── types.h
│       ├── wait.h
│       └── work.h
├── kernel8.img
└── src
    ├── allocate.c
    ├── cheesecake.c
    ├── fork.c
    ├── log.c
    ├── pid.c
    ├── schedule.c
    ├── wait.c
    └── work.c
```

In this chapter, we will expand the capabilities of our CheesecakeOS. At times a process will be waiting for an event. At other times - think IRQ context - it might be effcient or even neccessary to acknowledge that work needs to be done, but to defer that work until it can be scheduled. The wait module and the work module are the star additions.

- In [Waitqueues](waitqueues.md) we step through a waitqueue implementation allowing processes to change their state and yield the processor until a triggering event wakes them up to resume.
- [The Kernel Workqueue](workqueue.md) has us create our first bona-fide cake thread, a workqueue to allow functions to be deffered until they can be executed in process context.

*Previous Chapter* [Chapters[7]: Scheduling and Processes](../chapter7/chapter7.md) | *Next Chapter* [Chapters[9]: Virtual Memory and Fake Partition](../chapter9/chapter9.md)  
*Previous Page* [What the Fork!?](../chapter7/fork.md)  |  *Next Page* [Waitqueues](waitqueues.md)
