## Prerequisites

#### Books

- **C Programming Language by Brian Kernighan and Dennis Ritchie**

Like any self-respecting OS, CheesecakeOS is written mostly in `C`. If a pointer scares you, this material will be difficult. Review the classic `C` text.

- **Computer Systems: A Programmer's Perspective by Randall Bryant and David O'Hallaron**

An exemplary piece of literature, which gives a great feel for systems programming, including what an operating system should deliver. Topics include:

  \- Assembly language

  \- Stack discpline

  \- Procedure call standards

  \- Caches and the memory hierarchy

  \- Virtual memory and dynamic allocators

  \- The linking process

  \- Unix signals

  \- Unix I/O

Among others. Before starting on CheesecakeOS, absorb the lessons of this tome and work through the *_free_* labs for the authors' website. These labs are autograded so feedback on progress is available. Have I mentioned this book is a game changer?

- **Linux Kernel Development by Robert Love(\*)**

Not strictly necessary in my view, but a good resource to have on hand. This title and many other useful texts (including the aforementioned `K&R C Programming Language`) are available with a subscription to O'Reilly Online Learning, a service I use and love.

- **Understanding the Linux Kernel by Daniel Bovet and Marco Cesati(\*)**

Same story here as with `Linux Kernel Development by Robert Love`.


#### Hardware

- **Raspberry Pi 4, Model B**

The Raspberry Pi 4 is the greatest Raspberry Pi yet, featuring a four-core ARM Cortex A72 processor, and either 2GB, 4GB, or 8GB of RAM. To my knowledege, the main difference between the RPI4B models is this quantity of RAM. All three of these models will work seamlessly with the text. Raspberry Pi 3 can also be compatible, but has a different configuration, memory map, interrupt controller, and requires some updates to addresses and implementation details. Raspberry Pi's can be found pretty much anywhere, including `Amazon.com` if that is your favorite retailer. I got mine [here](https://www.kiwi-electronics.nl/raspberry-pi-4-model-b-2gb) and have been very happy.

- **USB to UART Cable**

In order to communicate with our OS (`I`) and have our OS communicate back with us (`O`) from a bare-metal context without the use of fancy things like USB or Ethernet drivers, we will use the GPIO Mini UART console. This console is accesible with a USB to UART (TTL) cable such as [this one](https://www.kiwi-electronics.nl/usb-to-ttl-serial-kabel). More information on getting setup with this cable and associated drivers is presented in the [Getting Started](getting_started.md) section.

- **Micro SD Card**

CheesecakeOS boots from a MicroSD card, just like the official operating systems promoted by the Raspberry Pi Foundation. A [16GB](https://www.kiwi-electronics.nl/toebehoren/opslag/microSD/transcend-16gb-microsd-ultimate-600x-class-10-uhs-i-adapter-mlc-90mbs) or 32GB card is my recommendation. [See also](https://www.raspberrypi.org/documentation/installation/sd-cards.md).

#### Software

- **Raspberry Pi Imager**

[Raspberry Pi Imager](https://www.raspberrypi.org/software/) will be used to format and test the SD card and connections.

- **Putty(\*)**

Probably only a requirement if you are using Windows. It is the software I use to access both AWS and the Raspberry Pi Mini UART console.

- **AWS Account(\*)**

Not required, but the [Getting Started](getting-started) tutorial assumes you will use an AWS Ubuntu Linux VM for development and building.

- **Docker(\*)**

Again, not required, but assumed in the [Getting Started](getting-started) guide.
