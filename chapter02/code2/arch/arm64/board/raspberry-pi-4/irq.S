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

#include "arch/linux-extension.h"
#include "board/devio.h"
#include "board/gic.h"

.globl __irq_acknowledge
__irq_acknowledge:
    __MOV_Q         x0, GICC_IAR
    __DEV_READ_32   w0, x0
    ret

.globl __irq_end
__irq_end:
    __MOV_Q         x1, GICC_EOIR
    __DEV_WRITE_32  w0, x1
    ret
