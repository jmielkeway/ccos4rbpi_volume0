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

#ifndef _ARCH_LOCK_H
#define _ARCH_LOCK_H

#include "cake/lock.h"

#define SPIN_LOCK               __spin_lock
#define SPIN_LOCK_IRQSAVE       spin_lock_irqsave
#define SPIN_UNLOCK             __spin_unlock
#define SPIN_UNLOCK_IRQRESTORE  spin_unlock_irqrestore

void __spin_lock(struct spinlock *lock);
void __spin_unlock(struct spinlock *lock);
unsigned long spin_lock_irqsave(struct spinlock *lock);
void spin_unlock_irqrestore(struct spinlock *lock, unsigned long flags);

#endif
