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

#include "config/config.h"
#include "cake/allocate.h"
#include "cake/atomic.h"
#include "cake/vm.h"

extern unsigned long page_global_dir[];

extern void free_user_memmap(struct memmap *mm);

struct memmap idle_memmap = {
    .users = NUM_CPUS + 1,
    .refcount = NUM_CPUS + 1,
    .pgd = page_global_dir,
    .lock = {
        .owner = 0,
        .ticket = 0
    }
};

void drop_memmap(struct memmap *mm)
{
    if(atomic_dec_and_test(&(mm->refcount))) {
        free_pages(&(PTR_TO_PAGE(mm->pgd)));
        cake_free(mm);
    }
}

void put_memmap(struct memmap *mm)
{
    if(atomic_dec_and_test(&(mm->users))) {
        free_user_memmap(mm);
        drop_memmap(mm);
    }
}
