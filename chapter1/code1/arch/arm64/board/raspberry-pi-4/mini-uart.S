#define MAIN_PERIPH_BASE        (0x47C000000)
#define UART_BASE_REG           (MAIN_PERIPH_BASE + 0x2215000)
#define AUX_MU_IO_REG           ((UART_BASE_REG) + 0x40)
#define AUX_MU_LSR_REG          ((UART_BASE_REG) + 0x54)
#define AUX_MU_LSR_REG_TISHIFT  (6)
#define AUX_MU_LSR_REG_TIFLAG   (1 << (AUX_MU_LSR_REG_TISHIFT))

    .macro __MOV_Q, reg, val
	    .if     (((\val) >> 31) == 0 || ((\val) >> 31) == 0x1ffffffff)
	    movz	\reg, :abs_g1_s:\val
	    .else
	    .if     (((\val) >> 47) == 0 || ((\val) >> 47) == 0x1ffff)
	    movz	\reg, :abs_g2_s:\val
	    .else
	    movz	\reg, :abs_g3:\val
	    movk	\reg, :abs_g2_nc:\val
	    .endif
	    movk	\reg, :abs_g1_nc:\val
	    .endif
	    movk	\reg, :abs_g0_nc:\val
	.endm

    .macro __DEV_READ_32, dst, src
        ldr     \dst, [\src]
    .endm

    .macro __DEV_WRITE_8, src, dst
        strb   \src, [\dst]
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
