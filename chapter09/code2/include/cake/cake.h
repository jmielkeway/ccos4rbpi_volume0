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

#ifndef _CAKE_CAKE_H
#define _CAKE_CAKE_H

#define BYTE_SIZE           (8)
#define LOG2(num)           ((unsigned) (BYTE_SIZE * sizeof (unsigned long long) - \
    __builtin_clzll((num)) - 1))
#define LOG2_SAFE(num)      num == 0 ? 0 : LOG2(num)
#define BITMAP_SHIFT        (LOG2_SAFE((BYTE_SIZE) * sizeof(unsigned long)))
#define BITMAP_MASK         (((1) << BITMAP_SHIFT) - 1)
#define BITMAP_SIZE(num)    (((num + BITMAP_MASK) >> BITMAP_SHIFT))

#define ROUND_MASK(x, y)    ((__typeof__(x))((y)-1))
#define ROUND_DOWN(x, y)    ((x) & ~(ROUND_MASK(x, y)))
#define DIV_ROUND_UP(n, d)  (((n) + (d) - 1) / (d))

#define OFFSETOF(type, member)  __builtin_offsetof(type, member)

#define CONTAINER_OF(ptr, type, member) ({          \
    void *__mptr = (void *)(ptr);                   \
    ((type *)(__mptr - OFFSETOF(type, member))); })

#endif
