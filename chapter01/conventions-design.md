*Chapter Top* [Chapters[1]: Hello, Cheesecake!](chapter1.md)  |  *Next Chapter* [Chapters[2]: Processor Initialization and Exceptions](../chapter2/chapter2.md)  
*Previous Page* [Hello, Cheesecake!](hello-cheesecake.md)  |  *Next Page* [Building and Linking](building-and-linking.md)

## Design and Conventions ([chapter1/code1](code1))

#### What We're Baking with

After some minor partioning and refactoring, our project structure looks a little more evolved:

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

Notably, source files are seperated into three distinct locations - [src](code1/src), [arch/arm64](code1/arch/arm64), and [arch/arm64/board/raspberry-pi-4](code1/arch/arm64/board/raspberry-pi-4). There are also seperate include directories for the main sources, [include/cake](code1/include/cake), and the architecture specific sources, [arch/arm64/include/arch](code1/arch/arm64/include/arch). 

While not a primary goal, some portability is possible. Other architectures besides `arm64` could theoretically be plugged in, with the main source code generically available as a library for all. Architecture specific code can be reused to support multiple boards - think a RaspberryPi 3 plugin. 

It could also be argued the mini UART code, for example, is in fact a device driver. For this tutorial, however, we will leave it in the board-specific directory.

#### The Design Implementation

In order to facilitate this paritioning, a layer of indirection has been introduced, and a console structure has been defined in [include/cake/log.h](code1/include/cake/log.h):

```C
#ifndef _CAKE_LOG_H
#define _CAKE_LOG_H

struct console {
    void (*write)(char *s);
};

void log(char *s);
void register_console(struct console *c);

#endif
```

The `cheesecake_main` function, still in [src/cheesecake_main.c](code1/src/cheesecake_main.c), initializes the log module with a call to `log_init` from the new `init` function:

```C
#include "cake/log.h"
#include "arch/timing.h"

extern void log_init();

static void init();

void cheesecake_main(void)
{
    char *version = "Version: 0.1.1.2\r\n";
    init();
    log("Hello, Cheesecake!\r\n");
    while (1) {
        log(version);
        log("\r\n");
        DELAY(20000000);
    }
}

static void init()
{
    log_init();
    log("LOG MODULE INITIALIZED\r\n");
}
```

Notice also the original `__delay` routine has been replaced with a `DELAY` macro, supplied by the architecture specific include ["arch/timing.h"](code1/arch/arm64/include/arch/timing.h): 

```C
#ifndef _ARCH_TIMING_H
#define _ARCH_TIMING_H

#define DELAY __delay

void __delay(unsigned long delay);

#endif
```

The idea is any architecture could be implemented, and could supply this macro, which would, in turn, resolve to whatever function the architecture needed it to.

Additionally, a log module has been created, [src/log.c](code1/src/log.c):

```C
extern void console_init();

static struct console *console;

void log(char *s)
{
    console->write(s);
}

void log_init()
{
    console_init();
}

void register_console(struct console *c)
{
    console = c;
}
```

The log module indicates there is a function, `console_init` defined elsewhere, and called by the `log_init` function. The module provides the `register_console` callback, such that whoever does implement the `console_init` function can set the static `console` variable, do be dereferenced when the `log` function is called. We come full circle as this functionality is provided in [arch/arm64/board/raspberry-pi-4/mini-uart.c](code1/arch/arm64/board/raspberry-pi-4/mini-uart.c):

```C
static void uart_puts(char *s);

static struct console console = {
    .write = uart_puts
};

void console_init()
{
    register_console(&console);
}
```

This is quite a bit of extra code and complexity. The only change in the executable path is a slow-down due to an extra pointer hop to get to the `uart_puts` function. Such is the price to be paid for design and a little bit of flexibility.

#### Conventions

Many of the conventions used throughout this tutorial have already been demonstrated:
- Module layout
  - Code that is generic to any archiecture goes in `src/`
  - Code that is instruction set archiecture specific goes in `arch/{ARCH}`
  - Code that is board or chip specific goes in `arch/{ARCH}/board/{BOARD}`
- Header files
  - Generic header files begin with `_CAKE_`, and belong in `include/cake`
  - Instruction set architecture specific header files begin with `_ARCH_`, and belong in `arch/{ARCH}/include/arch`
  - Board or chip specific header files begin with `_BOARD_`, and belong in `arch/{ARCH}/board/{BOARD}/include/board`
- Low-level assembly routines or functions are named beginning with two underscores (`__`)
- All macros are defined in UPPERCASE
- No low-level assembly routines are allowed in the main `src/` code, but they can be hidden by macros
- Within a module, or source file, the order of declaration is
  - External global variables in alphabetical order
  - External functions in alphabetical order
  - Internal type definitions in alphabetical order
  - Internal functions in alphabetical order
  - Internal static variables in alphabetical order
  - The static inline function definitions in alphabtical order
  - Functions, a mix of global and static, in alphabetical order

*Chapter Top* [Chapters[1]: Hello, Cheesecake!](chapter1.md)  |  *Next Chapter* [Chapters[2]: Processor Initialization and Exceptions](../chapter2/chapter2.md)  
*Previous Page* [Hello, Cheesecake!](hello-cheesecake.md)  |  *Next Page* [Building and Linking](building-and-linking.md)
