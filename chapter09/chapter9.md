*Previous Chapter* [Chapters[8]: Working and Waiting](../chapter08/chapter8.md) | *Next Chapter* [Chapters[10]: The TTY Driver with File Abstraction](../chapter10/chapter10.md)  
*Previous Page* [The Kernel Workqueue](../chapter08/workqueue.md)  |  *Next Page* [User Space Page Tables](pagetables.md)

## Chapters[9]: Virtual Memory

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
│       │   ├── include
│       │   │   └── user
│       │   │       └── syscall.h
│       │   ├── libc.S
│       │   ├── libc.c
│       │   └── sayhello.c
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
│       ├── fork.h
│       ├── list.h
│       ├── lock.h
│       ├── log.h
│       ├── process.h
│       ├── schedule.h
│       ├── types.h
│       ├── vm.h
│       ├── wait.h
│       └── work.h
└── src
    ├── allocate.c
    ├── cheesecake.c
    ├── exec.c
    ├── exit.c
    ├── fork.c
    ├── log.c
    ├── pid.c
    ├── sayhello.c
    ├── schedule.c
    ├── vm.c
    ├── wait.c
    └── work.c
```

Welcome to user space! We have built up enough kernel muscle to focus on supporting user applications. This, after all, is the entire purpose of the OS. By the end of the chapter, we will have a process running in user space with its own virtual address space. Utilizing system calls, that process will verify success through output to the screen.

This chapter is (possibly) the longest and (also possibly) most difficult. In order to take advantage of virtual memory, we need to implement user space page tables. Those page tables are connected to a process. This requires table switching alongside context switching. To demonstrate this capability requires some output. All output is controlled through the kernel, so system calls are needed to access output devices. The coupling of these dependencies means the web of functionality must be delivered as an atomic unit.

- [User Space Page Tables](pagetables.md) begins by defining the structures used with user space page tables. We then modify the forking process to allocate a new page table to a process, and fill in that page table with virtual memory sections. The teardown functionality is outlined as well. Finally, we look to the Linux Kernel and pull in the logic to safely switch user space table mappings in the course of a context switch.
- [The Page Fault Handler](pagefault.md) provides the method by which a dynamically allocated, sparsely mapped page table is filled in with mappings.
- [System Calls](syscall.md) you may think do not belong in a chapter about virtual memory. However, the generic implementation is somewhat trivial, and establishing the interface is necessary.
- In [Executing Into User Space](exec.md) we take a kernel process and triumphantly move it into user space.

*Previous Page* [The Kernel Workqueue](../chapter08/workqueue.md)  |  *Next Page* [User Space Page Tables](pagetables.md)  
*Previous Chapter* [Chapters[8]: Working and Waiting](../chapter08/chapter8.md) | *Next Chapter* [Chapters[10]: The TTY Driver with File Abstraction](../chapter10/chapter10.md)
