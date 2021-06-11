*Chapter Top* [Chapters[3]: Memory Management Unit](chapter3.md)  |  *Next Chapter* [Chapters[4]: Caches](../chapter4/chapter4.md)
*Previous Page*  [Chapters[3]: Memory Management Unit](chapter3.md) |  *Next Page* [Establishing Boot Tables](boot-tables.md)

## ARM Page Tables

#### Address Spaces

Address spaces are covered in depth in [Computer Systems: A Programmer's Perspective](https://www.amazon.com/Computer-Systems-Programmers-Perspective-3rd/dp/013409266X) by Bryant and O'Hallaron). What follows here is a juvenile summary of the topic.

Fundamental to the _Virtual Memory Abstraction_ is the concept of an address space. An address space simply represents the linear range of all possible address on a system. A system has one physical address space, the memory map of the system. One of the main jobs of an operating system is to virtualize the physical address space, such that each process can have a map of the system that looks similar (or, at times, identical) to the map of any other process, yet avoid clobbering the other processes. The address space of a typical process might follow a scheme like:

|  ADDRESS | REGION |
|  :---    | :---   |
| 0x0000_FFFF_FFFE_0000 | Process Stack |
| 0x0000_00AC_0000_0000 | Shared Libraries |
| 0x0000_0000_0200_0000 | Process Heap |
| 0x0000_0000_0010_0000 | Process BSS |
| 0x0000_0000_000C_0000 | Process Data |
| 0x0000_0000_000A_0000 | Process Code |

Indeed, every process on a system can have the same address for where the code section is mapped. Yet many processes exist on a system in perfect harmony, without sharing memory with each other. This is because each process address is not a physical address, but a _virtual_ address managed by a combination of the operating system and processor hardware. Generally, the implementation of virtual addressing is accomplished through the use of _Page Tables_.

#### ARM Page Tables

Let's take a virtual address as an example, say `0xFFFF_0123_4567_89AB`. Let's also assume that our system in configured to use `4KB` pages and has `48` addressable bits. These are apropos assumptions, as it is the configuration CheesecakeOs will use. This virtual address can be broken down into several components.

The most siginifcant sixteen bits, bits 63-48 must all have the same value in the ARM architecture, either all `0`s or all `1`s. This selects the _Page Global Directory_ - _PGD_ for short - to use. All `1`s means the PGD stored in the `TTBR1` system register will be selected, while all `0`s means the PGD stored in the `TTBR0` system register will be used. These registers are configured by the operating system. `TTBR1` is generally reserved for the kernel page tables, and is setup in the boot and initialization phase. After setup, at least in CheesecakeOS, it does not change. TTBR0, however, is generally used to store the PGD of the currently running process, and is updated as a different process is scheduled and grabs hold of the processor. For now, we will only be dealing with the kernel page tables. User space page tables will be developed in Parts[2]. So, given our example virtual address, with `FFFF` as the most significant `16` bits, the base of the page table that will translate our example address is located in `TTBR1`.

Backing up a bit, a PGD is simply a page of memory, in our case, a 4KB chunk, aligned on a 4KB boundry. Let's assume, for our purposes, our PGD exists at physical address `0x0000_0000_0022_1000` Since we have 64-bit, or 8-byte addresses, the PGD can fit 512 pointers. The next nine most significant bits, bits 47-39, are the first addressable bits, and an index into the PGD. In the case of our example address, bits 47-39 are _000000010_, or a decimal value of 2. With respect to our PGD, this is address `0x0000_0000_0022_1000` + 2 * 8-byte pointers, or `0x0000_0000_0022_1010`. The value stored at this indexed address is itself an address, and, itself a page table - called the _Page Upper Directory_.

The Page Upper Directory - or PUD - is simlar to the PGD, only it is at refered to as being at _Translation Level 1_, while the PGD is at _Translation Level 0_. There are also 512 pointers in the PUD, and the nine bits 38-30 are an index into _this_ table. Let's imagine *`0x0000_0000_0022_0010` == `0x0000_0000_0022_2000`. Bits 38-30 of our example address are _010001101_, decimal value 141. So the indexed address is `0x0000_0000_0022_2000` + 141 * 8-byte pointers, or `0x0000_0000_0022_2468.`

The value stored at `0x0000_0000_0022_2468` is another page table, the _Page Middle Directory_ or the PMD. _Translation Level 2_. Let's assume the value of the PMD as `0x0000_0000_0022_3000`. Bits 30-22 are an index into the PMD. For our example, they come out as _000101011_, decimal 43. Index 43 is at byte offset 0x158, so `0x0000_0000_0022_3158` is the relevent address.

`0x0000_0000_0020_3158` stores the value of _Translation Level 3_, the final page table. It is called the _Page Table Entry_ or PTE. Bits `21-12` serve as an index into the PTE. In the example case, _001111000_, decimal `120`, offset `0x3C0`. Assuming our PTE is located at `0x0000_0000_0020_4000`, `0x0000_0000_0020_4C30` holds the address of a page of physical memory, say `0x0000_0000_0020_5000`. Finally, the least significant twelve bits, bits `11-0`, or `0x9AB` in our example are the index into this page of physical memory that represnts the final physical memory address. Thus, given this example, _virtual address_ `FFFF_0123_4567_89AB` maps to _physical address_ `0x0000_0000_0020_59AB`.

In this way, it is possible for many address spaces to share similar virtual addresses. Given that each address space will be rooted with a unique PGD, with a unique _page table walk_ for each, the same virutal address will map to unique physical addresses. It is also possible, in the case of shared libraries as one example, for different virtual addresses to act as aliases for the same physical address.

A picture of our example could be represented like:
```
+-------------------------------------------------------------------------------------------+
|                                       Virtual address                                     |
+-------------------------------------------------------------------------------------------+
| F    F    F    F      0    1    2       3    4       5    6       7   8     9   A   B     |
+-------------------------------------------------------------------------------------------+
| 1111 1111 1111 1111 | 0000 0001 0 | 010 0011 01 | 00 0101 011 | 0 011 100 |               |
+-------------------------------------------------------------------------------------------+
|     TTBR Select     |  PGD Index  |  PUD Index  |  PMD Index  | PTE Index |  Page offset  |
+-------------------------------------------------------------------------------------------+ 
| 63                  | 47          | 38          | 29          | 20        | 11            |
+-------------------------------------------------------------------------------------------+
|                     |             |             |             |                           |
|                     |             |             |             |                           |
|                     |             |             |             |                           |
|                     |             --->|         -------->|    ------------->|             |
+------------+        |                 |                  |                  |             |
|   TTBR1    |        |                 |                  |                  |             |
+------------+        +-------------+   |                  |                  |             >+------------------+
| 0x00221000 |------->|  PGD Lvl 0  |   |                  |                  |              | Physical Address |
+------------+        +-------------+   |                  |                  |              +------------------+
                      | Offset 0x10 |   |                  |                  |              |   Offset 0x9AB   |
                      +-------------+   +--------------+   |                  |              +------------------+
                      |  0x00222000 |-->|   PUD Lvl 1  |   |                  |              | ** 0x002259AB ** |
                      +-------------+   +--------------+   |                  |              +------------------+
                                        | Offset 0x468 |   |                  |              ^
                                        +--------------+   +--------------+   |              ^
                                        |  0x00223000  |-->|   PMD Lvl 2  |   |              ^
                                        +--------------+   +--------------+   |              ^
                                                           | Offset 0x158 |   |              ^
                                                           +--------------+   +--------------+
                                                           |  0x00224000  |-->|   PTE Lvl 3  |
                                                           +--------------+   +--------------+
                                                                              | Offset 0x3C8 |
                                                                              +--------------+
                                                                              |  0x00225000  |
                                                                              +--------------+
```

Given a `48` bit address space, each PGD adresses `256TB` of virtual memory. Each of the `512` indexes of a PGD, then, point to a PUD that spans `512GB`. Each of the indexes of a PUD point to a PMD that spans `1GB`. Each of the indexes of a PMD point to a PTE that maps `512` pages, each `4KB` in size, for a total of `2MB`. It is possible to configure our tables with fewer levels of transation. For example, a PMD can be configured to point not to a PTE, but to a `2MB` block of physical memory, where the least significant `21` bits are the offset into that block to represent the true physical address.

#### ARM Page Table Attributes

Let us upon our example with a virtual address of `0xFFFF_0123_4567_89AB`, with our TTBR1 configured with a base address of `0x0000_0000_0022_1000`. We have seen that bits `47-38` represent an offset of `0x10` from the base address, and we indicated that `0x0000_0000_0022_2000`, the address of a PUD, was stored at the given offset. The clever conceivers of the ARM architecture noticed there was more they could do with this address. Since only the lower `48` bits of the address are used, the most significant `16` of this entry can be ignored for the sake of translation, and are free to store other information. Similarly, since each entry always points to a page, the least significant `12` bits are also available. In these bits, software stores attributes, such as read/write permissions. What attributes are available depends on the level of translation. Translation table format descriptor is the word ARM uses for a page table entry.

For _Level 0,1, and 2_ translations, the descriptor format depends on whether we want to point to the next level page table, of we want to point to a larger block of memory. For our purposes in this tutorial, a descriptor pointing to the next level page table will always have this format:

```
+------------------------------------------------------------------+
|                        Table Descriptor                          | 
+------------------------------------------------------------------+
| ALL 0s  | Next Level Table Addresss 4KB Aligned | ALL 0s | 1 | 1 |
+------------------------------------------------------------------+
| 63      | 47                                    | 11     | 1 | 0 |
+------------------------------------------------------------------+
```

Thus, the value stored at `0x0000_0000_0022_1010`, the pointer to the PUD will actually be `0x000_0000_0022_2003`, indicating the PUD is at physical address `0x0000_0000_0022_2000`, and it is a translation table, rather than a block of memory. Similarly, the PMD descriptor at `0x0000_0000_0022_2468` will be `0x000_0000_0022_3003`. Bit `0` in this case is the _valid_ bit, necessary to avoid a translation fault. Bit `1` determines whether the descriptor points to a table or a block. For a block descriptor, the format looks like:

```
+---------------------------------------------------------------------------------------------------------------+
|                                                Block Descriptor                                               |
+---------------------------------------------------------------------------------------------------------------+
| ALL 0s | SW USE | UXN | PXN | Next Level Table Address Block Aligned | nG | 1  | SH | AP | 0 | ATRIDX | 0 | 1 |
+---------------------------------------------------------------------------------------------------------------+
| 63     | 58     | 54  | 53  | 47                                     | 11 | 10 | 9  | 7  | 5 | 4      | 1 | 0 |
+---------------------------------------------------------------------------------------------------------------+
```

Since the block descriptor is the last level of translation, there is more configuration. The block descriptor is similar to the descriptor for _Level 3_ translations, which, also being the last translation level, must point to a `4KB` page. The difference is bit `1` is set:

```
+---------------------------------------------------------------------------------------------------------------+
|                                                Page Descriptor                                                |
+---------------------------------------------------------------------------------------------------------------+
| ALL 0s | SW USE | UXN | PXN | Next Level Table Address Block Aligned | nG | 1  | SH | AP | 0 | ATRIDX | 1 | 1 |
+---------------------------------------------------------------------------------------------------------------+
| 63     | 58     | 54  | 53  | 47                                     | 11 | 10 | 9  | 7  | 5 | 4      | 1 | 0 |
+---------------------------------------------------------------------------------------------------------------+
```

The `UXN` and `PXN` bits stand for _User Execute Never_ and _Priveledged Execute Never_. For kernel page tables, UXN is always set, while the value of PXN depends on whether the virtual addresses being mapped within the page are code or data. The `nG` bit stands for _non-Global_. For kernel page tables, this bit is left unset, while for user page tables it is set. Bit `10` is the _Access Flag` bit, and we always set it for valid descriptors. Bits `9-8`, the `SH` bits contain sharability attributes used by caches. The `AP` bits, `7-6` determine if a page can be read or written, and at what privelege level. The three `ATRIDX` bits , `4-2` are an index into the `MAIR` system register. Finally, for a page descriptor, bits `1` and `0` are always set. For our example virtual address, let's imagine we were mapping kernel data. So both the `UXN` and `PXN` bits are set. The `nG` bit is not set for kernel tables. Kernel data belongs to a memory set called _normal memory_ (the other set is _device_ memory, and the distinction I find to be self-explanatory). Let's assume we want this memory to be part of the _Inner Sharable_domain, so both `SH` bits should be set. We want the data to be readable and writeable by the kernel, but with no access from userspace, so both `AP` bits are left unset. Assuming we want some memory attributes at index `4` of the `MAIR` register, bits `4-2` would be _100_. After completing this somewhat arduous exercise, we can redraw our translation example:

```
+-------------------------------------------------------------------------------------------+
|                                       Virtual address                                     |
+-------------------------------------------------------------------------------------------+
| F    F    F    F      0    1    2       3    4       5    6       7   8     9   A   B     |
+-------------------------------------------------------------------------------------------+
| 1111 1111 1111 1111 | 0000 0001 0 | 010 0011 01 | 00 0101 011 | 0 011 100 |               |
+-------------------------------------------------------------------------------------------+
|     TTBR Select     |  PGD Index  |  PUD Index  |  PMD Index  | PTE Index |  Page offset  |
+-------------------------------------------------------------------------------------------+ 
| 63                  | 47          | 38          | 29          | 20        | 11            |
+-------------------------------------------------------------------------------------------+
|                     |             |             |             |                           |
|                     |             |             |             |                           |
|                     |             |             |             |                           |
|                     |             --->|         -------->|    ------------->|             |
+------------+        |                 |                  |                  |             |
|   TTBR1    |        |                 |                  |                  |             |
+------------+        +-------------+   |                  |                  |             >+------------------+
| 0x00221000 |------->|  PGD Lvl 0  |   |                  |                  |              | Physical Address |
+------------+        +-------------+   |                  |                  |              +------------------+
                      | Offset 0x10 |   |                  |                  |              |   Offset 0x9AB   |
                      +-------------+   +--------------+   |                  |              +------------------+
                      |  0x00222003 |-->|   PUD Lvl 1  |   |                  |              | ** 0x002259AB ** |
                      +-------------+   +--------------+   |                  |              +------------------+
                                        | Offset 0x468 |   |                  |                                 ^
                                        +--------------+   +--------------+   |                                 ^
                                        |  0x00223003  |-->|   PMD Lvl 2  |   |                                 ^
                                        +--------------+   +--------------+   |                                 ^
                                                           | Offset 0x158 |   |                                 ^
                                                           +--------------+   +--------------------+            ^
                                                           |  0x00224003  |-->|      PTE Lvl 3     |            ^
                                                           +--------------+   +--------------------+            ^
                                                                              |     Offset 0x3C8   |            ^
                                                                              +--------------------+            ^
                                                                              | 0x0060000000225713 |------------|
                                                                              +--------------------+
```

#### Memory Management System Registers

The `ATRIDX` bits, bits `4-2` of the block and page descriptors represent an index into the `64-bit` _Memory Attribute Indirection Register_, `pg. 2609` of the `ARM ARM`. If the `ATRIDX` is set to _000_, for example, bits `7-0` of the `MAIR` apply, while if the `ATRIDX` is _001_, bits `15-8` apply, etc. The `MAIR` attributes determine cacheability attributes for normal memory, and the ordering rules for device memory.

The _Translation Control Register_, `pg. 2685` of the `ARM ARM`, is used to configure various parameters for address translations. For example, we will use this register in CheesecakeOS to establish `4KB` pages, `48`-bit address spaces, and various cacheability and sharability attributes of translation table walks.

The Memory Management Unit itself needs to be enabled, after proper initalization, by setting the `M` bit, but `0` of the _System Control Register_, `SCLTR_EL1`.

*Chapter Top* [Chapters[3]: Memory Management Unit](chapter3.md)  |  *Next Chapter* [Chapters[4]: Caches](../chapter4/chapter4.md)
*Previous Page*  [Chapters[3]: Memory Management Unit](chapter3.md) |  *Next Page* [Establishing Boot Tables](boot-tables.md)
