*Previous Chapter* [Chapters[12]: The Shell](../chapter12/chapter12.md)  
*Previous Page* [The Shell](../chapter12/shell.md)  |  *Next Page* [Raspberry Pi 3 Support](rbpi3.md)

## Chapters[13]: Bonus

#### What We're Baking With

```bash
ccos4rbpi:~$ tree arch/arm64/board/raspberry-pi-3/
arch/arm64/board/raspberry-pi-3/
├── config.txt
├── filesystem.c
├── include
│   └── board
│       ├── bare-metal.h
│       ├── devio.h
│       ├── filesystem.h
│       └── peripheral.h
├── irq.S
├── irq.c
├── memmap.c
├── mini-uart.S
├── mini-uart.c
├── secure-boot.S
├── timer.S
├── timer.c
└── user.c
```

This bonus chapter explores adding support for another ARM64 board, the Raspberry Pi 3. The sources needed for implementation are substantially similar to those of the Raspberry Pi 4.

- [Raspberry Pi 3 Support](rbpi3.md) adds support for the Raspberry Pi 3 single board computer.

*Previous Page* [The Shell](../chapter12/shell.md)  |  *Next Page* [Raspberry Pi 3 Support](rbpi3.md)   
*Previous Chapter* [Chapters[12]: The Shell](../chapter12/chapter12.md)
