*Chapter Top* [Chapters[3]: Memory Management Unit](chapter3.md)  |  *Next Chapter* [Chapters[4]: Caches](../chapter04/chapter4.md)  
*Previous Page*  [Chapters[3]: Memory Management Unit](chapter3.md) |  *Next Page* [Establishing Boot Tables](boot-tables.md)

## ARM Page Tables

#### MMU Documentation

We will fumble our way through an introduction to the ARM Memory Management Unit, like butchers in a bake-shop. The ARM organization provides crucial documentation to start:

- The ARM Programmer's Guide section on the [Memory Management Unit](https://developer.arm.com/documentation/den0024/a/The-Memory-Management-Unit). 
- The ARMv8 [Memory Model Documentation](https://developer.arm.com/documentation/102376/latest)
- AArch64 [Memory Management Documentation](https://developer.arm.com/documentation/101811/0101)

#### Address Spaces

Address spaces are covered in depth in [Computer Systems: A Programmer's Perspective](https://www.amazon.com/Computer-Systems-Programmers-Perspective-3rd/dp/013409266X) by Bryant and O'Hallaron). What follows here is a quick summary.

Fundamental to the _Virtual Memory Abstraction_ is the concept of an address space. An address space is simply the linear range of all possible addresses in a system. A system has one physical address space, its memory map. One of the tasks of an operating system is to virtualize the physical address space, such that each process may be granted the illusion of having exclusive use of the system's memory resource. The memory map of a typical process might follow a scheme like:

|  ADDRESS | REGION |
|  :---    | :---   |
| 0x0000\_FFFF\_FFFE\_0000 | Process Stack |
| 0x0000\_00AC\_0000\_0000 | Shared Libraries |
| 0x0000\_0000\_0200\_0000 | Process Heap |
| 0x0000\_0000\_0010\_0000 | Process BSS |
| 0x0000\_0000\_000C\_0000 | Process Data |
| 0x0000\_0000\_000A\_0000 | Process Code |

Every process on the system is able to have the same address for where the code section starts, for example. Yet many processes exist on a system in perfect harmony, without getting in each other's way. This is because the addresses processes use are not physical addresses, but _virtual_ addresses managed by a combination of the operating system and processor hardware. In most architectures, the implementation of virtual addressing is accomplished through the use of _page tables_. The `arm64` architecture is not an exception.

#### ARM Page Tables

Let's take a virtual address as an example, say 0xFFFF\_0123\_4567\_89AB. Let's also assume that our system is configured to use 4KB pages and has 48 addressable bits. This just happens to be the configuration for CheesecakeOS. Our example virtual address is comprised of several sub-components.

The most significant sixteen bits of a virtual address, bits 63-48, must all have the same value in the ARM architecture. Either all zeroes or all ones. These two possible configurations are used to select between two registers: `TTBR1_EL1` and `TTBR0_EL1`. Both registers contain the base address of an ARM translation table, or, colloquially, page tables. The translation table enables the translation from virtual address to physical address. The Linux term for the base address of a translation table is _Page Global Directory_ - _PGD_ for short. If the most significant bits contain all ones, as in our example, the processor will translate the virtual address based on the _PGD_ stored in `TTBR1_EL1`. When all zeroes, the processor knows to translate from the _PGD_ stored in `TTBR0_EL1`.

`TTBR1_EL1` is used for the kernel page tables, and is setup in the boot and initialization phase. After setup, at least in CheesecakeOS, it does not change. `TTBR0_EL1`, however, is used to store the _PGD_ of a running user process. It will be replaced whenever a new user process is scheduled and grabs hold of the processor. The two separate system registers allow the system to more efficiently handle the separation between kernel space and user space. Until [Chapter Nine](../chapter09/chapter9.md), we will only be dealing with the kernel page tables.

A _PGD_ is simply a page of memory, in our case, a 4KB chunk, aligned on a 4KB boundary. Assume our _PGD_, the address stored in `TTBR1_EL1`, exists at physical address 0x0000\_0000\_0022\_1000. Since we have 64-bit, or 8-byte addresses, the _PGD_ can fit 512 pointers within its 4KB. The next nine most significant bits, bits 47-39, are the first addressable bits, and an index into the _PGD_'s array of pointers. In the case of our example address, bits 47-39 are 0b000000010, or a decimal value of two. With respect to our _PGD_, this is address 0x0000\_0000\_0022\_1000 + 2 * 8-byte pointers, or 0x0000\_0000\_0022\_1010. The value stored at this indexed address is itself a physical address, and a page table - in Linux called the _Page Upper Directory_.

The Page Upper Directory - or _PUD_ -  is similar to the _PGD_, only it is referred to as being at _Translation Level 1_, while the _PGD_ is at _Translation Level 0_. There are also 512 pointers in the _PUD_, and the nine bits 38-30 are an index into _this_ table. Let's imagine \*0x0000\_0000\_0022\_0010 == 0x0000\_0000\_0022\_2000. Bits 38-30 of our example address are 0b010001101, decimal value 141. So the indexed address into the _PUD_ is 0x0000\_0000\_0022\_2000 + 141 * 8-byte pointers, or 0x0000\_0000\_0022\_2468.

The value stored at 0x0000\_0000\_0022\_2468 is another page table, another physical address, the _Page Middle Directory_ or the _PMD_. _Translation Level 2_. Let's assume the value of the PMD as 0x0000\_0000\_0022\_3000. Bits 30-22 are an index into the _PMD_. For our example, they come out as 0b000101011, decimal 43. Index 43 is at byte offset 0x158, so 0x0000\_0000\_0022\_3158 is the relevant address.

0x0000\_0000\_0020\_3158 stores the value of _Translation Level 3_, the final page table. In Linux, it is called a _Page Table Entry_ or _PTE_. Bits 21-12 serve as an index into the _PTE_. In the example case, 0b001111000, decimal 120, offset 0x3C0. Assuming our PTE is located at 0x0000\_0000\_0020\_4000, 0x0000\_0000\_0020\_4C30 holds the address of a page of physical memory, say 0x0000\_0000\_0020\_5000. The least significant twelve bits, bits 11-0, or 0x9AB in our example are the index into this page of physical memory that represents the target physical memory address. Therefore, in this example _virtual address_ FFFF\_0123\_4567\_89AB maps to _physical address_ 0x0000\_0000\_0020\_59AB.

A picture of our example could be drawn like:

```
┌─────────────────────────────────────────────────────────────────────────────────────────────┐
│                                        Virtual Address                                      │
├─────────────────────────────────────────────────────────────────────────────────────────────┤
│ F    F    F    F      0    1    2       3    4       5    6       7    8      9   A   B     │
├─────────────────────┬─────────────┬─────────────┬─────────────┬─────────────┬───────────────┤
│ 1111 1111 1111 1111 │ 0000 0001 0 │ 010 0011 01 │ 00 0101 011 │ 0 0111 1000 │               │
├─────────────────────┼─────────────┼─────────────┼─────────────┼─────────────┼───────────────┤
│     TTBR Select     │  PGD Index  │  PUD Index  │  PMD Index  │  PTE Index  │  Page offset  │
├─────────────────────┼─────────────┼─────────────┼─────────────┼─────────────┼───────────────┤ 
│ 63                  │ 47          │ 38          │ 29          │ 20          │ 11            │
├─────────────────────┼─────────────┼─────────────┼─────────────┼─────────────┴───────────────┤
│                     │             │             │             │                             │
│                     │             │             │             │                             │
│                     │             │             │             │                             │
│                     │             └───┐         └────────┐    └─────────────┐               │
├────────────┐        │                 │                  │                  │               │
│   TTBR1    │        │                 │                  │                  │               │
├────────────┤        ├─────────────┐   │                  │                  │               ├──────────────────┐
│ 0x00221000 ├────────►  PGD Lvl 0  │   │                  │                  │               │ Physical Address │
└────────────┘        ├─────────────┤   │                  │                  │               ├──────────────────┤
                      │ Offset 0x10 │   │                  │                  │               │   Offset 0x9AB   │
                      ├─────────────┤   ├──────────────┐   │                  │               ├──────────────────┤
                      │  0x00222000 ├───►   PUD Lvl 1  │   │                  │               │ ** 0x002259AB ** │
                      └─────────────┘   ├──────────────┤   │                  │               └──▲───────────────┘
                                        │ Offset 0x468 │   │                  │                  │
                                        ├──────────────┤   ├──────────────┐   │                  │
                                        │  0x00223000  ├───►   PMD Lvl 2  │   │                  │
                                        └──────────────┘   ├──────────────┤   │                  │
                                                           │ Offset 0x158 │   │                  │
                                                           ├──────────────┤   ├──────────────┐   │
                                                           │  0x00224000  ├───►   PTE Lvl 3  │   │
                                                           └──────────────┘   ├──────────────┤   │
                                                                              │ Offset 0x3C8 │   │
                                                                              ├──────────────┤   │
                                                                              │  0x00225000  ├───┘
                                                                              └──────────────┘
```

Given a 48-bit address space, each _PGD_ addresses 256TB of virtual memory. Each of the 512 indexes of a _PGD_, then, point to a _PUD_ that spans 512GB. Each of the indexes of a _PUD_ point to a _PMD_ that spans 1GB. Each of the indexes of a _PMD_ point to a _PTE_ that maps 512 pages, each 4KB in size, for a total of 2MB. It is possible to configure our tables with fewer levels of translation. For example, a _PMD_ can be configured to point not to a _PTE_, but to a 2MB block of physical memory, where the least significant 21 bits are the offset into that block to calculate the true physical address.

In this way, it is possible for many processes to use identical virtual addresses. Given that each process will have an address rooted with a unique _PGD_, with a distinct _page table walk_ for each, the same virtual address will map to unique physical addresses. It is also possible, in the case of shared libraries as one example, for different virtual addresses to act as aliases for the same physical address.

Notice, before moving on, each address used in the page table walk to translate the virtual address is a physical address. Ultimately, virtual addresses will be used by our software, but the translation requires a physical backing.

#### ARM Page Table Attributes

Continuing our example with virtual address 0xFFFF\_0123\_4567\_89AB, and `TTBR1_EL1` configured with a base address of 0x0000\_0000\_0022\_1000. We have seen that bits 47-38 give an offset of 0x10 bytes from the base address, and we assumed that 0x0000\_0000\_0022\_2000, the physical address of a _PUD_, was stored at that given offset. The clever conceivers of the ARM architecture noticed there was more they could do with this address. Since only the lower 48 bits are used for addressing, the sixteen most significant bits of this entry can be ignored for the sake of translation, and are free to store other information. Further, since each translation table entry always points to an aligned block of 4KB or larger, the least significant twelve bits are also available. In these bits, software stores attributes, such as read/write permissions. What attributes are available - and their interpretation - depends on the level of translation. Translation table format descriptor is the word ARM uses for a page table entry.

For _Level 0,1, and 2_ translations, the descriptor format depends on whether we want to point to the next level page table, of we want to point to a block of memory. For our purposes in this tutorial, a descriptor pointing to the next level page table will always have this format:

```
┌─────────────────────────────────────────────────────────────────┐
│                        Table Descriptor                         │ 
├─────────┬──────────────────────────────────────┬────────┬───┬───┤
│ ALL 0s  │ Next Level Table Address 4KB Aligned │ ALL 0s │ 1 │ 1 │
├─────────┼──────────────────────────────────────┼────────┼───┼───┤
│ 63      │ 47                                   │ 11     │ 1 │ 0 │
└─────────┴──────────────────────────────────────┴────────┴───┴───┘
```

Thus, the value stored at address 0x0000\_0000\_0022\_1010, the index pointing to the _PUD_, will be 0x000\_0000\_0022\_2003, indicating the _PUD_ is at physical address 0x0000\_0000\_0022\_2000, and is a translation table instead of a block of memory. Similarly, the _PMD_ descriptor at 0x0000\_0000\_0022\_2468 will be 0x000\_0000\_0022\_3003. Bit `0` in this case is the _valid_ bit, necessary to avoid a translation fault. Bit `1` determines whether the descriptor points to a table or a block. For a block descriptor, the format looks like:

```
┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                           Block Descriptor                                         │
├────────┬────────┬─────┬─────┬─────────────────────────────┬────┬────┬────┬────┬───┬────────┬───┬───┤
│ ALL 0s │ SW USE │ UXN │ PXN │ Block Address Block Aligned │ nG │ 1  │ SH │ AP │ 0 │ ATRIDX │ 0 │ 1 │
├────────┼────────┼─────┼─────┼─────────────────────────────┼────┼────┼────┼────┼───┼────────┼───┼───┤
│ 63     │ 58     │ 54  │ 53  │ 47                          │ 11 │ 10 │ 9  │ 7  │ 5 │ 4      │ 1 │ 0 │
└────────┴────────┴─────┴─────┴─────────────────────────────┴────┴────┴────┴────┴───┴────────┴───┴───┘
```

Since the block descriptor is the last level of translation, there is more configuration. The block descriptor is similar to the descriptor for _Level 3_ translations, which, also being the last translation level, must point to a `4KB` page. The difference is bit `1` is set for a page descriptor:

```
┌───────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                              Page Descriptor                                              │
├────────┬────────┬─────┬─────┬────────────────────────────────────┬────┬────┬────┬────┬───┬────────┬───┬───┤
│ ALL 0s │ SW USE │ UXN │ PXN │ Physical Page Address Page Aligned │ nG │ 1  │ SH │ AP │ 0 │ ATRIDX │ 1 │ 1 │
├────────┼────────┼─────┼─────┼────────────────────────────────────┼────┼────┼────┼────┼───┼────────┼───┼───┤
│ 63     │ 58     │ 54  │ 53  │ 47                                 │ 11 │ 10 │ 9  │ 7  │ 5 │ 4      │ 1 │ 0 │
└────────┴────────┴─────┴─────┴────────────────────────────────────┴────┴────┴────┴────┴───┴────────┴───┴───┘
```

The `UXN` and `PXN` bits stand for _User Execute Never_ and _Privileged Execute Never_. For kernel page tables, `UXN` is always set, while the value of `PXN` depends on whether the virtual addresses being mapped within the page are code or data. The `nG` bit stands for _non-Global_. For kernel page tables, this bit is left unset, while for user page tables it is set. Bit `10` is the _Access Flag_ bit, and we always set it for valid descriptors. Bits `9-8`, the `SH` bits contain shareability attributes used by caches. The `AP` bits, `7-6`, determine if a page can be read or written, and at what privilege level. The three `ATRIDX` bits , `4-2` are an index into the `MAIR_EL1` system register. Finally, for a page descriptor, bits `1` and `0` are always set. For our example virtual address, let us imagine we were mapping kernel data. So both the `UXN` and `PXN` bits are set. The `nG` bit is not set for kernel tables. Kernel data belongs to a memory set called _normal memory_ (the other set is _device_ memory). Let's assume we want this memory to be part of the _Inner Shareable_ domain, so both `SH` bits should be set. We want the data to be readable and writeable by the kernel, but with no access from user space, so both `AP` bits are left unset. Assuming we want some memory attributes at index four of the `MAIR_EL1` register, bits `4-2` would be 0b100. After completing this somewhat arduous exercise, we can redraw our translation example:

```
┌─────────────────────────────────────────────────────────────────────────────────────────────┐
│                                        Virtual Address                                      │
├─────────────────────────────────────────────────────────────────────────────────────────────┤
│ F    F    F    F      0    1    2       3    4       5    6       7    8      9   A   B     │
├─────────────────────┬─────────────┬─────────────┬─────────────┬─────────────┬───────────────┤
│ 1111 1111 1111 1111 │ 0000 0001 0 │ 010 0011 01 │ 00 0101 011 │ 0 0111 1000 │               │
├─────────────────────┼─────────────┼─────────────┼─────────────┼─────────────┼───────────────┤
│     TTBR Select     │  PGD Index  │  PUD Index  │  PMD Index  │  PTE Index  │  Page offset  │
├─────────────────────┼─────────────┼─────────────┼─────────────┼─────────────┼───────────────┤ 
│ 63                  │ 47          │ 38          │ 29          │ 20          │ 11            │
├─────────────────────┼─────────────┼─────────────┼─────────────┼─────────────┴───────────────┤
│                     │             │             │             │                             │
│                     │             │             │             │                             │
│                     │             │             │             │                             │
│                     │             └───┐         └────────┐    └─────────────┐               │
├────────────┐        │                 │                  │                  │               │
│   TTBR1    │        │                 │                  │                  │               │
├────────────┤        ├─────────────┐   │                  │                  │               ├──────────────────┐
│ 0x00221000 ├────────►  PGD Lvl 0  │   │                  │                  │               │ Physical Address │
└────────────┘        ├─────────────┤   │                  │                  │               ├──────────────────┤
                      │ Offset 0x10 │   │                  │                  │               │   Offset 0x9AB   │
                      ├─────────────┤   ├──────────────┐   │                  │               ├──────────────────┤
                      │  0x00222003 ├───►   PUD Lvl 1  │   │                  │               │ ** 0x002259AB ** │
                      └─────────────┘   ├──────────────┤   │                  │               └───────▲──────────┘
                                        │ Offset 0x468 │   │                  │                       │ 
                                        ├──────────────┤   ├──────────────┐   │                       │  
                                        │  0x00223003  ├───►   PMD Lvl 2  │   │                       │   
                                        └──────────────┘   ├──────────────┤   │                       │    
                                                           │ Offset 0x158 │   │                       │     
                                                           ├──────────────┤   ├────────────────────┐  │      
                                                           │  0x00224003  ├───►      PTE Lvl 3     │  │       
                                                           └──────────────┘   ├────────────────────┤  │        
                                                                              │     Offset 0x3C8   │  │         
                                                                              ├────────────────────┤  │         
                                                                              │ 0x0060000000225713 ├──┘
                                                                              └────────────────────┘
```

If you found this section confusing, the ARM ARM contains the full explanation of descriptors, beginning on `pg. D4-2143`.

#### Memory Management System Registers

While studying the ARM MMU, we will work with four system registers:
1. The _Translation Table Base Register 0_, `TTBR0_EL1`.
2. The _Translation Table Base Register 1_, `TTBR1_EL1`.
3. The _Translation Control Register_, `TCR_EL1`.
4. The _Memory Attribute Indirection Register_, `MAIR_EL1`.

We have already assessed the significance of the translation table base registers in this slice. 

The _Translation Control Register_ is used to configure various parameters for address translations. For example, we will use this register in CheesecakeOS to set the page size to 4KB, the number of addressable bits to 48, and enable various cacheability and shareability attributes of translation table walks.

The `ATRIDX` bits, bits `4-2` of the block and page descriptors represent an index into the 64-bit _Memory Attribute Indirection Register_. If the `ATRIDX` is set to 0b000, for example, bits `7-0` of the `MAIR_EL1` apply, while if the `ATRIDX` is 0b001, bits `15-8` apply, etc. The `MAIR_EL1` establishes cacheability attributes for normal memory, and the ordering rules for device memory.

The Memory Management Unit itself needs to be enabled, after proper initialization, by setting the `M` bit, bit `0` of the _System Control Register_, `SCLTR_EL1`.

More on cacheability, shareability, and memory attributes in the ARM Programmer's Guide section on the [Memory Management Unit](https://developer.arm.com/documentation/den0024/a/The-Memory-Management-Unit).

*Previous Page*  [Chapters[3]: Memory Management Unit](chapter3.md) |  *Next Page* [Establishing Boot Tables](boot-tables.md)  
*Chapter Top* [Chapters[3]: Memory Management Unit](chapter3.md)  |  *Next Chapter* [Chapters[4]: Caches](../chapter04/chapter4.md)
