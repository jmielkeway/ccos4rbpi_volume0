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

.global __irq_disable
__irq_disable:
    msr     daifset, #2
    ret

.globl __irq_enable
__irq_enable:
    msr     daifclr, #2
    ret

.global __wait_for_interrupt
__wait_for_interrupt:
    wfi
    dsb     sy
    isb
    ret
