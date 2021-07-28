*Previous Chapter* [Chapters[9]: Virtual Memory](../chapter09/chapter09.md) | *Next Chapter* [Chapters[11]: Signals](../chapter11/chapter11.md)  
*Previous Page* [Executing Into User Space](../chapter09/exec.md)  |  *Next Page* [Writing](write.md)

## Chapters[10]: The TTY Driver with File Abstraction

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
│       │       ├── syscall.h
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
│       ├── smp.S
│       ├── smp.c
│       ├── sync.c
│       ├── syscall.c
│       ├── user
│       │   ├── cat.c
│       │   ├── include
│       │   │   └── user
│       │   │       └── syscall.h
│       │   ├── libc.S
│       │   ├── libc.c
│       │   └── shell.c
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
    ├── tty.c
    ├── vm.c
    ├── wait.c
    └── work.c
```

It's time to put the 'I' in I/O. Before this chapter, we have only been able to output information from our system. This chapter sees us through enabling input capability as well. By the conclusion we will have started work on a baby shell application. The shell will interact with a TTY driver, an abstraction to manage the mini UART communication we have so far been accessing through the `log` function/module. Supporting fixes and additional features for the shell will then be the focus of the remainder of the volume.

- The [Writing](write.md) section introduces the file and TTY abstractions we will be working with, then exposes the `write` system call to the system.
- [Reading](read.md) is a more involved process requiring interaction from the user, but once implemented we can already create a simple `cat` application.
- In [Embryonic Shell](shell.md) we combine reading, writing, and cloning, and execing to demo our first stab at the Cheesecake Shell.

*Previous Page* [Executing Into User Space](../chapter09/exec.md)  |  *Next Page* [Writing](write.md)  
*Previous Chapter* [Chapters[9]: Virtual Memory](../chapter09/chapter09.md) | *Next Chapter* [Chapters[11]: Signals](../chapter11/chapter11.md)
