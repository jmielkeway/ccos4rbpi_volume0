## Design and Conventions ([chapter1/code1](code1))

#### Design
Can present the tree after restructuring:

```bash
ccos4rbpi:~$ tree
.
├── Makefile
├── arch
│   └── arm64
│       ├── board
│       │   └── raspberry-pi-4
│       │       ├── config.txt
│       │       ├── mini-uart.S
│       │       └── mini-uart.c
│       ├── include
│       │   └── arch
│       │       └── timing.h
│       ├── linker.ld
│       ├── main.S
│       └── timing.S
├── build.sh
├── include
│   └── cake
│       └── log.h
└── src
    ├── cheesecake.c
    └── log.c
```

Some portability is possible, though it is not a primary goal. There is architecture specific code separated from the main source. Inside this there is board specific code. That is, code specific to the Raspberry Pi 4. It should be possible, for example, to use most of the architecuture and main source code with a Raspberry Pi 3 plugin. Some argument could be made that the mini uart code, along with some other sources that will end up in the board specific portion are actually devices or device drivers. In this book, they will be treated as a part of a board concept instead.

No extra executable code is added in this section, but the Makefile is updated. Building is the subject of the next section.

The include files do change to accomodate the design structure, as in, for example, `src/cheesecake.c`:

```C
#include "cake/log.h"
#include "arch/timing.h"
```

#### Conventions

1. All capitals are used for Macros
2. Double underscores for assembly functions
3. All capitals with double underscores for assembly macros
4. Inline assembly is frowned upon
