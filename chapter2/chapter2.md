*Previous Chapter* [Chapters[1]: Hello, Cheesecake!](../chapter1/chapter1.md)  |  *Next Chapter* [Chapters[3]: Memory Management Unit](../chapter3/chapter3.md)  
*Previous Page* [Logging](../chapter1/logging.md)  |  *Next Page* [Setting the Exception Level in the ARM Stub](arm-stub.md)

## Chapters[2]: Processor Initalization and Exceptions

Our little fledgling OS is pretty cool, epistemologically, but not particalarly useful. The only recourse we have to get an instruction on a CPU is to branch - we lack the means to _interrupt_ execution to jump to a different part of our image and do something cool. In this chapter, we are going to get a bit ARM-y as we develop the infrastructure for processor interrupts - known colloquially as IRQs.

- [Setting the Exception Level in the ARM Stub](arm-stub.md) has us copying large sections of the Raspberry Pi ARM stub in order to set our processor at the correct ARM Execution Level (EL).
- [Exceptions and the ARM Exception Vector Table](exception-vector-table.md) illustrates the initializtion of the ARMv8 Vector Table for handling ARM Exceptions.
- [The Interrupt Controller](interrupt-controller.md) material takes us deeper than we wanted to go into the ARM Generic Interrupt Controller (GIC), but not deeper than necessary.
- [Timer Interrupts](timer-interrupts.md) elucidate this chapter's raison d'être.

*Previous Chapter* [Chapters[1]: Hello, Cheesecake!](../chapter1/chapter1.md)  |  *Next Chapter* [Chapters[3]: Memory Management Unit](../chapter3/chapter3.md)  
*Previous Page* [Logging](../chapter1/logging.md)  |  *Next Page* [Setting the Exception Level in the ARM Stub](arm-stub.md)
