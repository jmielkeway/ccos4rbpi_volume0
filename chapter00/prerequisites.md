*Chapter Top* [Chapters[0]: Introduction](chapter0.md)  |  *Next Chapter* [Chapters[1]: Hello, Cheesecake!](../chapter01/chapter1.md)   
*Previous Page* [Resources](resources.md)  |  *Next Page* [Getting Started](getting-started.md)

## Prerequisites

### Books

**C Programming Language by Brian Kernighan and Dennis Ritchie**

Like any self-respecting OS, CheesecakeOS is written mostly in _C_. If a pointer scares you, this material will be difficult. Review the classic _C_ text.

**Computer Systems: A Programmer's Perspective by Randall Bryant and David O'Hallaron**

An exemplary piece of literature, which gives a great feel for systems programming, including what an operating system should deliver. Topics covered include assembly language, the stack discipline, procedure call standards, caches and the memory hierarchy, virtual memory and dynamic allocation, the linking process, Unix signals, system calls, Unix I/O, and others. Before starting on CheesecakeOS, absorb the lessons of this tome and work through the *_free_* labs from the authors' [website](http://csapp.cs.cmu.edu/3e/labs.html). These labs are autograded so feedback on progress and understanding is available. Have I mentioned this book is a game changer? It may take a few weeks or months to get through the material. It is worth the investment.

**Linux Kernel Development by Robert Love(\*)**

Not strictly necessary, but a good resource to have on hand. This title and many other useful texts (including the aforementioned `K&R C Programming Language`) are available with a subscription to O'Reilly Online Learning, a service I use and love.

**Understanding the Linux Kernel by Daniel Bovet and Marco Cesati(\*)**

Same story here as with `Linux Kernel Development` by Robert Love, above.

### ARM Tutorials

**Learn the architecture: AArch64 Instruction Set Architecture**

You will want to be well versed in the [basics](https://developer.arm.com/documentation/102374/0101) of the AArch64 instruction set architecture, including the names and uses for the general purpose registers, some system registers, and the most common instructions.

### Hardware and Materials

**Raspberry Pi 4, Model B**

The Raspberry Pi 4 is the greatest Raspberry Pi yet, featuring a four-core ARM Cortex A72 processor, and either 2GB, 4GB, or 8GB of RAM. To my knowledge, the main difference between the RPI4B models is this quantity of RAM. All three of these models will work seamlessly with the text. Raspberry Pi 3 can also be compatible, but has a different configuration, memory map, interrupt controller, and requires some updates to addresses and implementation details. Raspberry Pi's can be found pretty much anywhere, including `Amazon.com` if that is your favorite retailer. I got mine [here](https://www.kiwi-electronics.nl/raspberry-pi-4-model-b-2gb) and have been happy.

**USB to Serial UART Connection**

In order to communicate with our OS (`I`) and have our OS communicate back with us (`O`) from a bare-metal context without the use of fancy things like USB or Ethernet drivers, we will use the GPIO mini UART console. This console is accessible with a USB to UART (TTL) connection, such as [this cable](https://www.kiwi-electronics.nl/usb-to-ttl-serial-kabel). While the cables are convenient, I have discovered they break easily. I instead favor a chip like [this one](https://www.bol.com/nl/nl/p/cp2102-usb-naar-ttl-serial-uart-adapter-3-3v-5v/9300000004985800/), along with some female-female jumper cables to attach to the Raspberry Pi 4 GPIO pins. More information on getting setup with this connection and associated drivers is presented in the [Getting Started](getting-started.md) slice.

**Micro SD Card**

CheesecakeOS boots from a MicroSD card, just like the official operating systems promoted by the Raspberry Pi Foundation. A [16GB](https://www.kiwi-electronics.nl/toebehoren/opslag/microSD/transcend-16gb-microsd-ultimate-600x-class-10-uhs-i-adapter-mlc-90mbs) or 32GB card will work well. [See also](https://www.raspberrypi.org/documentation/installation/sd-cards.md).

### Software

**Raspberry Pi Imager**

[Raspberry Pi Imager](https://www.raspberrypi.org/software/) will be used to format and test the SD card and connections.

**Putty(\*)**

Probably only a requirement if you are using Windows. It is the software I use to access both AWS and the Raspberry Pi Mini UART console.

**AWS Account(\*)**

Not required, but the [Getting Started](getting-started.md) tutorial assumes you will use an AWS Ubuntu Linux VM for developing and building the project.

**Docker(\*)**

Again, not required, but assumed in the [Getting Started](getting-started.md) guide.

*Previous Page* [Resources](resources.md)  |  *Next Page* [Getting Started](getting-started.md)  
*Chapter Top* [Chapters[0]: Introduction](chapter0.md)  |  *Next Chapter* [Chapters[1]: Hello, Cheesecake!](../chapter01/chapter1.md)
