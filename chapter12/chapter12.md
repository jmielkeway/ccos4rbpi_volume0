*Previous Chapter* [Chapters[11]: Signals](../chapter11/chapter11.md) | *Next Chapter* [Chapters[13]: Bonus](../chapter13/chapter13.md)  
*Previous Page* [Receiving](../chapter11/receiving.md)  |  *Next Page* [The Shell](shell.md)

## Chapters[12]: The Shell

#### What We're Baking With
```
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
│       │       ├── filesystem.c
│       │       ├── include
│       │       │   └── board
│       │       │       ├── bare-metal.h
│       │       │       ├── devio.h
│       │       │       ├── filesystem.h
│       │       │       ├── gic.h
│       │       │       └── peripheral.h
│       │       ├── irq.S
│       │       ├── irq.c
│       │       ├── memmap.c
│       │       ├── mini-uart.S
│       │       ├── mini-uart.c
│       │       ├── secure-boot.S
│       │       ├── timer.S
│       │       ├── timer.c
│       │       └── user.c
│       ├── cache.S
│       ├── entry.S
│       ├── error.c
│       ├── event.S
│       ├── exec
│       │   └── asm-offsets.c
│       ├── fault.c
│       ├── include
│       │   └── arch
│       │       ├── abort.h
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
│       │       ├── smp.h
│       │       ├── start.h
│       │       ├── user.h
│       │       └── vm.h
│       ├── irq.S
│       ├── linker.template
│       ├── lock.S
│       ├── main.S
│       ├── memcpy.S
│       ├── memset.S
│       ├── process.c
│       ├── schedule.S
│       ├── signal.c
│       ├── smp.S
│       ├── smp.c
│       ├── sync.c
│       ├── syscall.c
│       ├── user
│       │   ├── cat.c
│       │   ├── fault.c
│       │   ├── hello.c
│       │   ├── include
│       │   │   └── user
│       │   │       ├── cpu.h
│       │   │       ├── fork.h
│       │   │       ├── signal.h
│       │   │       ├── syscall.h
│       │   │       └── wait.h
│       │   ├── infinity.c
│       │   ├── libc.S
│       │   ├── libc.c
│       │   ├── shell.c
│       │   └── showcpus.c
│       └── vm.c
├── build.sh
├── cheesecake.conf
├── config
│   └── config.py
├── include
│   └── cake
│       ├── allocate.h
│       ├── atomic.h
│       ├── bitops.h
│       ├── cake.h
│       ├── compiler.h
│       ├── error.h
│       ├── file.h
│       ├── fork.h
│       ├── list.h
│       ├── lock.h
│       ├── log.h
│       ├── process.h
│       ├── schedule.h
│       ├── signal.h
│       ├── tty.h
│       ├── types.h
│       ├── user.h
│       ├── vm.h
│       ├── wait.h
│       └── work.h
└── src
    ├── allocate.c
    ├── cheesecake.c
    ├── exec.c
    ├── exit.c
    ├── file.c
    ├── filesystem.c
    ├── fork.c
    ├── log.c
    ├── pid.c
    ├── schedule.c
    ├── signal.c
    ├── tty.c
    ├── vm.c
    ├── wait.c
    └── work.c
```

Just take a look at all those ingredients! I hope you have your dessert fork ready, because this chapter is our victory lap. We will focus on wrapping up the CheesecakeOS Shell application, adding or refining the few kernel services necessary to achieve the cool smoothness otherwise found at your favorite delicatessen.

- [The Shell](shell.md) is our last hurrah, the finale in this installment of CheesecakeOS for Raspberry Pi 4.

*Previous Page* [Receiving](../chapter11/receiving.md)  |  *Next Page* [The Shell](shell.md)
*Previous Chapter* [Chapters[11]: Signals](../chapter11/chapter11.md) | *Next Chapter* [Chapters[13]: Bonus](../chapter13/chapter13.md)  
