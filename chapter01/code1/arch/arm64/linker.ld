OUTPUT_ARCH(aarch64)
ENTRY(_start)
SECTIONS
{
    . = 0x0;
    .text.boot : {
        _start = .;
        *(.text.boot)
    }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x8);
    bss_begin = .;
    .bss : { *(.bss*) }
    bss_end = .;
    . = 0x400000;
    _end = .;
}
