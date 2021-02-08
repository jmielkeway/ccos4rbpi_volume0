## Hello, Cheesecake! ([code0](code0))

#### Up and Running

Goal of this section is to get up and going ASAP. After the Hello, Cheesecake! OS functionality is running, there is the opportunity to explain what goes into making it work.

Navigate to the [chapter1/code0](code0) and build the project. Note that the `./build.sh` scripts should work if you have setup docker as described in the [Getting Started](../chapter0/getting-started.md) section.

```bash
ccos4rbpi:~$ ./build.sh
mkdir -p .build
aarch64-linux-gnu-gcc -Wall -nostdlib -nostartfiles -ffreestanding -mgeneral-regs-only -MMD -c src/cheesecake.c -o .build/cheesecake_c.o
mkdir -p .build
aarch64-linux-gnu-gcc -Wall -nostdlib -nostartfiles -ffreestanding -mgeneral-regs-only -MMD -c src/mini-uart.c -o .build/mini-uart_c.o
mkdir -p .build
aarch64-linux-gnu-gcc -Wall -nostdlib -nostartfiles -ffreestanding -mgeneral-regs-only -MMD -c src/mini-uart.S -o .build/mini-uart_s.o
mkdir -p .build
aarch64-linux-gnu-gcc -Wall -nostdlib -nostartfiles -ffreestanding -mgeneral-regs-only -MMD -c src/timing.S -o .build/timing_s.o
mkdir -p .build
aarch64-linux-gnu-gcc -Wall -nostdlib -nostartfiles -ffreestanding -mgeneral-regs-only -MMD -c src/main.S -o .build/main_s.o
aarch64-linux-gnu-ld -T linker.ld -o .build/kernel8.elf .build/cheesecake_c.o .build/mini-uart_c.o .build/mini-uart_s.o .build/timing_s.o .build/main_s.o
aarch64-linux-gnu-objdump -d .build/kernel8.elf > .build/kernel8.dsa
aarch64-linux-gnu-nm -n .build/kernel8.elf > .build/kernel8.map
aarch64-linux-gnu-objcopy .build/kernel8.elf -O binary kernel8.img
```

As a result of building, there should be a `kernel8.img` file created in the [chapter1/code0](code0) directory. Copy that file onto the `boot` partition of your imaged micro SD card, possibly overwriting another `kernel8.img` file if one already exists. Also, copy the `config.txt` file from the [chapter1/code0](code0) directory, and overwrite if already exists. For more context on what each of these config options means, check the [Raspberry Pi config.txt Documentation](https://www.raspberrypi.org/documentation/configuration/config-txt/). We will not revisit this config.txt, and it will remain just like this for the remainder of our time together in this book.

```bash
ccos4rbpi:~$ cat config.txt
arm_64bit=1
arm_peri_high=1
disable_commandline_tags=1
enable_gic=1
enable_uart=1
kernel_old=1
```

Insert the micro SD card into your Raspberry Pi 4. Connect the USB to mini UART. Connect your terminal emulator. Connect Raspberry Pi 4 to power source (USB or console, not both!). Wait about 15 seconds. If all has gone well, you should see something like:

$$$ PICTURE HERE $$$


#### The Code

Can show here an image of the tree

```bash
ccos4rbpi:~$ tree
.
├── Makefile
├── build.sh
├── config.txt
├── linker.ld
└── src
    ├── cheesecake.c
    ├── main.S
    ├── mini-uart.S
    ├── mini-uart.c
    └── timing.S
```

Start with `main.S`, line by line

```asm
#define MPIDR_HWID_MASK_LITE    0xFFFFFF

.section ".text.boot"

.globl __entry
__entry:
    mrs     x0, mpidr_el1
    and     x0, x0, #MPIDR_HWID_MASK_LITE
    cbz     x0, __run
    b       __sleep

__sleep:
    wfe
    b       __sleep

__run:
    adr     x0, bss_begin
    adr     x1, bss_end
    bl      __zerobss
    adrp    x13, _end
    mov     sp, x13
    bl      cheesecake_main
    b       __sleep

__zerobss:
    sub     x1, x1, x0
    str     xzr, [x0], #8
    subs    x1, x1, #8
    b.gt    __zerobss
    ret
```

Then `cheescake.c`:

```C
extern void __delay(unsigned long delay);
extern void uart_puts(char *s);

void cheesecake_main(void)
{
    char *version = "Version: 0.1.1\r\n";
    uart_puts("Hello, Cheesecake!\r\n");
    while (1) {
        uart_puts(version);
        uart_puts("\r\n");
        __delay(20000000);
    }
}

```

Moving on to `mini-uart.c`:

```C
extern int  __uart_can_io();
extern void __uart_putchar(char c);

static inline int check_ready()
{
    return __uart_can_io();
}

static inline void uart_putchar(char c)
{
    while(!check_ready()) {
    }
    return __uart_putchar(c);
}

void uart_puts(char *s)
{
    if(s) {
        for(char *str = s; *str != '\0'; str++) {
            uart_putchar(*str);
        }
    }
}
```

Which calls into `mini-uart.S`:

```asm
#define MAIN_PERIPH_BASE        (0x47C000000)
#define UART_BASE_REG           (MAIN_PERIPH_BASE + 0x2215000)
#define AUX_MU_IO_REG           ((UART_BASE_REG) + 0x40)
#define AUX_MU_LSR_REG          ((UART_BASE_REG) + 0x54)
#define AUX_MU_LSR_REG_TISHIFT  (6)
#define AUX_MU_LSR_REG_TIFLAG   (1 << (AUX_MU_LSR_REG_TISHIFT))

    .macro __MOV_Q, reg, val
        .if     (((\val) >> 31) == 0 || ((\val) >> 31) == 0x1ffffffff)
        movz    \reg, :abs_g1_s:\val
        .else
        .if     (((\val) >> 47) == 0 || ((\val) >> 47) == 0x1ffff)
        movz    \reg, :abs_g2_s:\val
        .else
        movz    \reg, :abs_g3:\val
        movk    \reg, :abs_g2_nc:\val
        .endif
        movk    \reg, :abs_g1_nc:\val
        .endif
        movk    \reg, :abs_g0_nc:\val
    .endm

.globl __uart_can_io
__uart_can_io:
    __MOV_Q         x0, AUX_MU_LSR_REG
    __DEV_READ_32   w0, x0
    and             w0, w0, AUX_MU_LSR_REG_TIFLAG
    ret

.globl __uart_putchar
__uart_putchar:
    __MOV_Q         x1, AUX_MU_IO_REG
    __DEV_WRITE_8   w0, x1
    ret
```

And finally wrap up with `timing.S`:

```asm
.globl __delay
__delay:
    subs    x0, x0, #1
    bne     __delay
    ret
```
