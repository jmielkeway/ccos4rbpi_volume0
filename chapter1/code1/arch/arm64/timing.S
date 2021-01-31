.globl __delay
__delay:
    subs    x0, x0, #1
    bne     __delay
    ret
