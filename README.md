## CheesecakeOS for Raspberry Pi - Volume 1
### Parts[0]: Welcome
#### [Chapters[0]: Introduction](chapter0/chapter0.md)
  ##### [*Disclaimer*](chapter0/disclaimer.md)
  ##### [*Acknowledgements*](chapter0/acknowledgements.md)
  ##### *Prerequisites*
  ##### *Resources*
  ##### *Getting Started*
#### Parts[1]: Kernel Space
#### Chapters[1]: Hello, Cheesecake!
  ##### *Conventions and Design*
  ##### *Hello Cheesecake! Code*
  ##### *Building*
  ##### *Linking*
#### Chapters[2]: Processor Initiliazation and Exceptions
  ##### *ARM Stub*
  ##### *Priveledge Setup*
  ##### *Exceptions and the ARM Exception Vector Table*
  ##### *The Interrupt Controller*
#### Chapters[3]: Memory Management Unit
  ##### *ARM Page Tables*
  ##### *Page Attributes and Virtual Addresses*
  ##### *Establishing Boot Tables*
  ##### *The Bootmem Allocator*
  ##### *Creating Global Page Directory*
  ##### *Turning on the MMU*
#### Chapters[4]: Caches
  ##### *Preliminary ARM Requirements*
  ##### *Enabling the Cache*
#### Chapters[5]: SMP
  ##### *Synchronization*
  ##### *Spinlocks*
  ##### *Atomics*
  ##### *Turning on all CPUs*
#### Chapters[6]: Memory Allocation
  ##### *The Global Memory Map*
  ##### *List Data Structure*
  ##### *Freelists with Buddies*
  ##### *Slab Allocator with Object Caches*
#### Chapters[7]: Scheduling and Processes
  ##### *Process Struct*
  ##### *Bitmaps*
  ##### *Runqueues*
  ##### *Context Switching*
  ##### *Scheduling Algorithm*
  ##### *What the Fork!?*
#### Chapters[8]: Working and Waiting
  ##### *Waitqueues*
  ##### *Workqueues*
  ##### *First Kernel Thread*
#### Parts[2]: User Space
#### Chapters[9]: Virtual Memory and Fake Partition
  ##### *Building*
  ##### *Create Userspace Page Tables*
  ##### *Copy Page Tables*
  ##### *Copy-on-write and the Page Fault Handler*
  ##### *Moving to Userspace*
#### Chapters[10]: The TTY and File Abstraction
  ##### *Line Discipline*
  ##### *Low Level Driver*
  ##### *Reading*
  ##### *Writing*
  ##### *Super Simple Cat Program*
#### Chapters[11]: All The System Calls
  ##### *Read and Write*
  ##### *Fork and Execve and Exit*
  ##### *Getpid and Kill and Waitpid*
  ##### *Sbrk*
#### Chapters[12]: The Shell
  ##### *The Shell*
  ##### *Builtins*
  ##### *Utilities*
