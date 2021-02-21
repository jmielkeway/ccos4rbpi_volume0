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

#define ATOMIC_LONG_ANDNOT      __atomic64_andnot
#define ATOMIC_LONG_FETCH_OR   __atomic64_fetch_or
#define ATOMIC_LONG_OR          __atomic64_or

void __atomic64_andnot(volatile unsigned long *bitmap, unsigned long bit);
int __atomic64_fetch_or(volatile unsigned long *bitmap, unsigned long bit);
void __atomic64_or(volatile unsigned long *bitmap, unsigned long bit);

