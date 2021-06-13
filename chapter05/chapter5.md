*Previous Chapter* [Chapters[4]: Caches](../chapter4/chapter4.md) | *Next Chapter* [Chapters[6]: Memory Allocation](../chapter6/chapter6.md)  
*Previous Page* [Enabling the Caches](../chapter4/caches.md)  |  *Next Page* [Synchronization](synchronization.md)

## Chapters[5]: SMP

Any modern operating system worth its salt can support symmetric multiprocessing. CheesecakeOS should expect to be no different. Afterall, what is cheesecake without salt!? Supporting multiple processors at the lowest software level is a fraught exercise requiring advanced synchronization techniques and hours of spare time to spend debugging. We will not shy away from such a challenge.

- [Synchronization](synchronization.md) demonstrates the first hesitenet steps in running multiple processes, namely freeing them from their inital sleep loop.
- [The Cantankerous Cache Bug](cache-bug.md) is a nasty surprise we will encounter and overcome in our quest to fire on all processors.
- [Spinlocks](spinlocks.md) will be a crucial arrow in our CheesecakeOS spring-form pan. We will enjoy the implementation, and use it often.
- [Atomics and Ordering](atomics-ordering.md) are advanced concepts needing a good groking. Also a library of functions to serve us going forward.

*Previous Chapter* [Chapters[4]: Caches](../chapter4/chapter4.md) | *Next Chapter* [Chapters[6]: Memory Allocation](../chapter6/chapter6.md)  
*Previous Page* [Enabling the Caches](../chapter4/caches.md)  |  *Next Page* [Synchronization](synchronization.md)
