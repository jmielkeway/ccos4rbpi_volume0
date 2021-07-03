*Next Chapter* [Chapters[1]: Hello, Cheesecake!](../chapter01/chapter1.md)  
*Previous Page* [Table of Contents](../README.md)  |  *Next Page* [Disclaimer](disclaimer.md)

## Chapters[0] Introduction

Welcome to **Cheesecake OS for Raspberry Pi 4**, a home-baked guide to bare-metal operating system development. CheesecakeOS is served in five delicious volumes:

[*Volumes[0]*: Booting, Processes, and Virtual Memory](https://github.com/jmielkeway/ccos4rbpi_volume0) (<--- YOU ARE HERE)  
*Volumes[1]*: File System and Drivers                    (Coming Soon!)  
*Volumes[2]*: Networking                                 (Coming Later!)  
*Volumes[3]*: Compiler and C Library                     (Coming Distantly!)  
*Volumes[4]*: The Rest!                                  (Don't Hold Your Breath!)

Together, we will build our own operating system from scratch, feature by feature, concept by concept. All we will have to start is the compiler as our oven, and the Raspberry Pi 4 Bootloader as our spring-form pan. The rest, we will provide ourselves. As anyone who has tried baking can attest, it is not for the faint of heart. We will encounter such curdling confections as goto statements, architecture-specific implementations, barrier instructions, and copious unnecessary allusions to cheesecake.

A volume is composed of several chapters. Each chapter has a focus - generally a concept or component important to the functioning of OS software. Each chapter is broken into one or more _slices_. A slice will most often correspond to some feature we will implement in order to support the focus of the chapter. In the case of earlier chapters, a slice may offer salient summaries of the build system or the architecture.

The recipe we will follow in Volumes[0]:

- [*Chapters[0]: Introduction*](chapter0.md) - We become acquainted with the tools of the volume, cover prerequisites, and setup the build environment.
- [*Chapters[1]: Hello, Cheesecake!*](../chapter01/chapter1.md) - Every worthy programming project needs a _Hello, World!_ We will establish the dependency design of the OS.
- [*Chapters[2]: Processor Initialization and Exceptions*](../chapter02/chapter2.md) - We learn a processor has a dry clump of configuration that needs initialization at boot time. Exceptions, and interrupt requests are tied in early to allow for non-linear instruction execution.
- [*Chapters[3]: Memory Management Unit*](../chapter03/chapter3.md) - We get our hands powdered statically allocating and populating the kernel page tables during the boot process. We turn on the processor's Memory Management Unit, allowing and requiring the kernel to use virtual addresses.
- [*Chapters[4]: Caches*](../chapter04/chapter4.md) - A short chapter where we enable the hardware caches. Modern processors are kind, and much of the difficult work is taken care of by the hardware.
- [*Chapters[5]: SMP*](../chapter05/chapter5.md) - Any self-respecting operating system is written in the `C` programming language. It also supports multiple CPUs. CheesecakeOS is worth its salt.
- [*Chapters[6]: Memory Allocation*](../chapter06/chapter6.md) - We build a memory allocator for the system. A page allocator that uses a binary buddy to break larger pages into smaller pages for dynamically sized allocations, then can coalesce them back together when the pages are freed. On top of the page allocator, we build a slab allocator with object caches.
- [*Chapters[7]: Scheduling and Processes*](../chapter07/chapter7.md) - We invent a structure to represent the execution of a program, the process. We then create the means for these processes to yield and control the processor.
- [*Chapters[8]: Working and Waiting*](../chapter08/chapter8.md) - we cook up two useful types of kernel queues, wait queues to allow processes to sleep while waiting for an event, and work queues for executing deferred work.
- [*Chapters[9]: Virtual Memory*](../chapter09/chapter9.md) - We begin encroaching on user space territory, implementing user space page tables, the page fault handler, and system calls. At the end of the chapter, we bring it all together and run our first user space program
- [*Chapters[10]: The TTY Driver with File Abstraction*](../chapter10/chapter10.md) - We put the 'I' in I/O, developing a generic TTY driver with support from the *N\_TTY* line discipline. We will be able to input lines from the keyboard.
- [*Chapters[11]: Signals*](../chapter11/chapter11.md) - Sending and receiving signals for fun and job control.
- [*Chapters[12]: The Shell*](../chapter12/chapter12.md) - We end Volumes[0] with a shell implementation, pulling together the features added in previous chapters, along with a sprinkle of new bits.
- [*Chapters[13]: Bonus*](../chapter13/chapter13.md) - We demonstrate how to port CheesecakeOS to another single board computer , the Raspberry Pi 3.

There are a few prerequisites for this volume. The remainder of this chapter includes these requirements along with some acknowledgements, resources, and a disclaimer.

- The [Disclaimer](disclaimer.md) cautions against the hazards of consuming CheesecakeOS or the contents of this volume.
- [Acknowledgements](acknowledgements.md) gives thanks.
- [Resources](resources.md) provides links to external manuals, sources, prerequisites, or useful articles, which may prove helpful throughout the development process.
- [Prerequisites](prerequisites.md) outlines what you should know or accomplish before continuing with CheesecakeOS.
- [Getting Started](getting-started.md) walks through the steps to set up our hardware and working environment.

*Previous Page* [Table of Contents](../README.md)  |  *Next Page* [Disclaimer](disclaimer.md)  
*Next Chapter* [Chapters[1]: Hello, Cheesecake!](../chapter01/chapter1.md)  
