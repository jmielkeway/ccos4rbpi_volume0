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

#include "cake/allocate.h"
#include "cake/list.h"
#include "cake/log.h"

extern void arch_populate_allocate_structures(struct list *freelists);

static struct list freelists[MAX_ORDER + 1];
struct page *system_phys_page_dir;

void allocate_init()
{
    for(unsigned int i = 0; i <= MAX_ORDER; i++) {
        struct list *freelist = &(freelists[i]);
        freelist->next = freelist;
        freelist->prev = freelist;
    }
    arch_populate_allocate_structures(freelists);
    log("Page dir location: %x\r\n", (unsigned long) system_phys_page_dir);
    for(unsigned long i = 0; i <= MAX_ORDER ; i++) {
        struct list *freelist = &(freelists[i]);
        struct page *p;
        log("Printing freelist for order: %x\r\n", i);
        LIST_FOR_EACH_ENTRY(p, freelist, pagelist) {
            log("{Page Details: index: %x, allocated: %x, valid: %x}\r\n", 
                p->pfn, p->allocated, p->valid);
            log("{List Details: next: %x, prev: %x}\r\n", 
                p->pagelist.next, p->pagelist.prev);
        }
    }
}

