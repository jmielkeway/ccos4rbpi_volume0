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

#include "cake/cake.h"
#include "cake/compiler.h"
#include "arch/atomic.h"

#define BITS_PER_BYTE   (BYTE_SIZE)
#define BITS_PER_LONG   (BITS_PER_BYTE * sizeof(unsigned long))
#define BIT_WORD(bit)   ((bit) / BITS_PER_LONG)
#define BIT_MASK(bit)   (1UL << ((bit) % BITS_PER_LONG))

static inline void clear_bit(volatile unsigned long *bitmap, unsigned long bit)
{
    bitmap += BIT_WORD(bit);
    ATOMIC_LONG_ANDNOT(bitmap, BIT_MASK(bit));
}

static inline void set_bit(volatile unsigned long *bitmap, unsigned long bit)
{
    bitmap += BIT_WORD(bit);
    ATOMIC_LONG_OR(bitmap, BIT_MASK(bit));
}

static inline int test_and_set_bit(volatile unsigned long *bitmap, unsigned long bit)
{
    long old;
    unsigned long mask = BIT_MASK(bit);
    bitmap += BIT_WORD(bit);
    if (READ_ONCE(*bitmap) & mask)
        return 1;
    old = ATOMIC_LONG_FETCH_OR(bitmap, mask);
    return !!(old & mask);
}
