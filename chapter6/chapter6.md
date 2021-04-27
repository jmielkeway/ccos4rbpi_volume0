*Previous Chapter* [Chapters[5]: SMP](../chapter5/chapter5.md) | *Next Chapter* [Chapters[7]: Scheduling and Processes](../chapter7/chapter7.md)  
*Previous Page* [Atomics and Ordering](../chapter5/atomics-ordering.md)  |  *Next Page* [The Global Memory Map](global-memmap.md)

## Chapters[6]: Memory Allocator

With our CheesecakeOS booted, we can begin to provide all kinds of services. A memory allocator can be constructed independent of other services. Other services cannot be constructed without access to dynamic memory allocation. So here we start!

- [The Global Memory Map](global-memmap.md) intializes our allocator by tracking all pages in the system. We start by implementing a list structure we will use ubiquitously.
- [Binary Buddies](binary-buddies.md) is an allocation system that allows free blocks of memory to be split in two to satisfy memory requests, then later coalesced.
- [Slab Allocator with Object Caches](slab.md) will bring our allocator to the next level...similar to Linux!

*Previous Chapter* [Chapters[5]: SMP](../chapter5/chapter5.md) | *Next Chapter* [Chapters[7]: Scheduling and Processes](../chapter7/chapter7.md)  
*Previous Page* [Atomics and Ordering](../chapter5/atomics-ordering.md)  |  *Next Page* [The Global Memory Map](global-memmap.md)
