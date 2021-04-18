*Previous Chapter* [Chapters[2]: Processor Initialization and Exceptions](../chapter2/chapter2.md) | *Next Chapter* [Chapters[4]: Caches](../chapter4/chapter4.md)  
*Previous Page* [Timer Interrupts](../chapter2/timer-interrupts.md)  |  *Next Page* [ARM Page Tables](arm-page-tables.md)

## Chapters[3]: Memory Mangement Unit

Virtual Memory is a fundamental abstration of a modern operating system. In this chapter, we explore the ARM Memory Mangement Unit and related concepts. By the end of the chapter, our systems's MMU will be turned on, and our kernel will be accessing virtual - not physical - addresses.

- [ARM Page Tables](arm-page-tables.md) covers the basics of how virtual memory is implemented and configured in ARM - through the use of page tables. :muscle:
- [Establishing Boot Tables](boot-tables.md) shows our CheesecakeOS bootstrapping the MMU with the first statically allocated page tables. :cake:
- [Linear Mapping the Entire Physical Address Space](linear-mapping.md) is the penultimate low-level kernel memory-managment step, establishing a boot-time allocator, and linear mapping the entire memory of the system. :cake:
- [Turning on the MMU](mmu.md) will be this chapter's victory lap! :cake:

*Previous Chapter* [Chapters[2]: Processor Initialization and Exceptions](../chapter2/chapter2.md) | *Next Chapter* [Chapters[4]: Caches](../chapter4/chapter4.md)  
*Previous Page* [Timer Interrupts](../chapter2/timer-interrupts.md)  |  *Next Page* [ARM Page Tables](arm-page-tables.md)
