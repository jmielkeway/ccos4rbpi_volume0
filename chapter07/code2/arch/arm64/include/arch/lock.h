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
#include "cake/schedule.h"

#define SPIN_LOCK               spin_lock
#define SPIN_LOCK_BOOT          __spin_lock
#define SPIN_LOCK_IRQSAVE       spin_lock_irqsave
#define SPIN_UNLOCK             spin_unlock
#define SPIN_UNLOCK_BOOT        __spin_unlock
#define SPIN_UNLOCK_IRQRESTORE  spin_unlock_irqrestore

void __irq_restore(unsigned long flags);
unsigned long __irq_save();
void __spin_lock(struct spinlock *lock);
void __spin_unlock(struct spinlock *lock);

static inline void spin_lock(struct spinlock *lock)
{
    PREEMPT_DISABLE();
    __spin_lock(lock);
}

static inline unsigned long spin_lock_irqsave(struct spinlock *lock)
{
    unsigned long flags;
    PREEMPT_DISABLE();
    flags = __irq_save();
    __spin_lock(lock);
    return flags;
}

static inline void spin_unlock(struct spinlock *lock)
{
    __spin_unlock(lock);
    PREEMPT_ENABLE();
}

static inline void spin_unlock_irqrestore(struct spinlock *lock, unsigned long flags)
{
    __spin_unlock(lock);
    __irq_restore(flags);
    PREEMPT_ENABLE();
}

#endif
