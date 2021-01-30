.section ".text.boot"

.globl __entry
__entry:
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
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
