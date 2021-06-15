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

#define TIMER_BASE_REG  (MAIN_PERIPH_BASE + 0x2003000)
#define TIMER_CS        ((TIMER_BASE_REG) + 0x0)
#define TIMER_CLO       ((TIMER_BASE_REG) + 0x4)
#define TIMER_C3        ((TIMER_BASE_REG) + 0x18)

#define TIMER_CHANNEL   (3)
#define TIMER_CS_M3     (1 << (TIMER_CHANNEL))

.globl __timer_clock_low
__timer_clock_low:
    __MOV_Q         x0, PHYS_TO_VIRT(TIMER_CLO)
    __DEV_READ_32   w0, x0
    ret

.globl __timer_control_reset
__timer_control_reset:
    mov             w0, TIMER_CS_M3
    __MOV_Q         x1, PHYS_TO_VIRT(TIMER_CS)
    __DEV_WRITE_32  w0, x1
    ret

.globl __timer_set_compare
__timer_set_compare:
    __MOV_Q         x1, PHYS_TO_VIRT(TIMER_C3)
    __DEV_WRITE_32  w0, x1
    ret
