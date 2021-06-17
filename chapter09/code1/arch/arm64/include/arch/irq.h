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

#ifndef _ARCH_IRQ_H
#define _ARCH_IRQ_H

#define IRQ_DISABLE         __irq_disable
#define IRQ_ENABLE          __irq_enable
#define WAIT_FOR_INTERRUPT  __wait_for_interrupt

extern void __irq_disable();
extern void __irq_enable();
extern void __wait_for_interrupt();

#endif
