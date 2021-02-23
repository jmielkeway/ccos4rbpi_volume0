/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "arch/bare-metal.h"
#include "arch/linux-extension.h"
#include "board/devio.h"
#include "board/peripheral.h"

#define UART_BASE_REG           (MAIN_PERIPH_BASE + 0x2215000)
#define AUX_MU_IO_REG           ((UART_BASE_REG) + 0x40)
#define AUX_MU_LSR_REG          ((UART_BASE_REG) + 0x54)
#define AUX_MU_LSR_REG_TISHIFT  (6)
#define AUX_MU_LSR_REG_TIFLAG   (1 << (AUX_MU_LSR_REG_TISHIFT))

.globl __uart_can_io
__uart_can_io:
    __MOV_Q         x0, PHYS_TO_VIRT(AUX_MU_LSR_REG)
    __DEV_READ_32   w0, x0
    and             w0, w0, AUX_MU_LSR_REG_TIFLAG
    ret

.globl __uart_putchar
__uart_putchar:
    __MOV_Q         x1, PHYS_TO_VIRT(AUX_MU_IO_REG)
    __DEV_WRITE_8   w0, x1
    ret
