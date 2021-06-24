*Previous Chapter* [Chapters[10]: The TTY Driver with File Abstraction](../chapter10/chapter10.md) | *Next Chapter* [Chapters[12]: The Shell](../chapter12/chapter12.md)  
*Previous Page* [Embryonic Shell](../chapter10/shell.md)  |  *Next Page* [Sending](sending.md)

## Chapters[11]: Signals

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
│       │   ├── include
│       │   │   └── user
│       │   │       ├── signal.h
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

As we left things in the previous chapter, our shell application and its child were both fighting for the same TTY resource. With no job control mechanism, there was no good way to let the shell know to yield the TTY input and output capabilities to the `cat` program. We are going to try our hand at rectifying the issue in this chapter.

We will be implementing signals. Singals give processes a way to talk to (or bludgeon) each other. One process, or the kernel, can request to another to stop or resume processing. The TTY module can keep track of the process that should control the I/O resource and ask others to suspend execution until signaled to resume.

- In [Sending](sending.md) we have the TTY send a stop signal to any process that tries to read and write when that process is not the session leader (TTY owner).
- In [Receiving](receiving.md) we demonstrate our baby shell is able to catch a `SIGCHLD` signal from a child process.

*Previous Chapter* [Chapters[10]: The TTY Driver with File Abstraction](../chapter10/chapter10.md) | *Next Chapter* [Chapters[12]: The Shell](../chapter12/chapter12.md)  
*Previous Page* [Embryonic Shell](../chapter10/shell.md)  |  *Next Page* [Sending](sending.md)
