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

#define BITS_PER_BYTE                   (BYTE_SIZE)
#define BITS_PER_LONG                   (BITS_PER_BYTE * sizeof(unsigned long))
#define BITS_TO_LONGS(nr)               (DIV_ROUND_UP(nr, (BITS_PER_LONG)))
#define BIT_WORD(bit)                   ((bit) / BITS_PER_LONG)
#define BIT_MASK(bit)                   (1UL << ((bit) % BITS_PER_LONG))
#define BITMAP_FIRST_WORD_MASK(start)   (~0UL << ((start) & (BITS_PER_LONG - 1)))

extern void memset(void *dest, int c, unsigned long count);

static inline void bitmap_fill(unsigned long *bitmap, unsigned long size)
{
    unsigned int len = BITS_TO_LONGS(size) * sizeof(unsigned long);
    memset(bitmap, 0xFF, len);
}

static inline void bitmap_zero(unsigned long *bitmap, unsigned long size)
{
    unsigned int len = BITS_TO_LONGS(size) * sizeof(unsigned long);
    memset(bitmap, 0, len);
}

static inline void clear_bit(volatile unsigned long *bitmap, unsigned long bit)
{
    bitmap += BIT_WORD(bit);
    ATOMIC_LONG_ANDNOT(bitmap, BIT_MASK(bit));
}

static inline unsigned long find_next_bit(volatile unsigned long *bitmap,
    unsigned long start, unsigned long nbits)
{
    unsigned long tmp;
    if (start >= nbits) {
        return nbits;
	}
    tmp = bitmap[start / BITS_PER_LONG];
    tmp &= BITMAP_FIRST_WORD_MASK(start);
    start = ROUND_DOWN(start, BITS_PER_LONG);
    while (!tmp) {
        start += BITS_PER_LONG;
        if (start >= nbits) {
            return nbits;
		}
        tmp = bitmap[start / BITS_PER_LONG];
    }
    tmp = start + __builtin_ctzl(tmp);
    return tmp < nbits ? tmp : nbits;
} 

static inline unsigned long find_next_zero_bit(volatile unsigned long *bitmap, 
	unsigned long start,
	unsigned long nbits)
{
    unsigned long tmp;
	unsigned long invert = ~0UL;
    if (start >= nbits) {
        return nbits;
	}
    tmp = bitmap[start / BITS_PER_LONG];
    tmp ^= invert;
    tmp &= BITMAP_FIRST_WORD_MASK(start);
    start = ROUND_DOWN(start, BITS_PER_LONG);
    while (!tmp) {
        start += BITS_PER_LONG;
        if (start >= nbits) {
            return nbits;
		}
        tmp = bitmap[start / BITS_PER_LONG];
        tmp ^= invert;
    }
    tmp = start + __builtin_ctzl(tmp);
    return tmp < nbits ? tmp : nbits;
}

static inline void set_bit(volatile unsigned long *bitmap, unsigned long bit)
{
    bitmap += BIT_WORD(bit);
    ATOMIC_LONG_OR(bitmap, BIT_MASK(bit));
}

static inline int test_and_clear_bit(volatile unsigned long *bitmap, unsigned long bit)
{
    long old;
    unsigned long mask = BIT_MASK(bit);
    bitmap += BIT_WORD(bit);
    if (!(READ_ONCE(*bitmap) & mask))
        return 0;
    old = ATOMIC_LONG_FETCH_ANDNOT(bitmap, mask);
    return !!(old & mask);
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

static inline int test_bit(volatile unsigned long *bitmap, unsigned long bit)
{
    unsigned long mask = BIT_MASK(bit);
    bitmap += BIT_WORD(bit);
    return !!(READ_ONCE(*bitmap) & mask);
}
