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

#ifndef _CAKE_ALLOCATE_H
#define _CAKE_ALLOCATE_H

#include "config/config.h"
#include "cake/list.h"
#include "cake/lock.h"

#define MAX_ORDER                       (9)
#define MIN_SIZE_CACHE_ORDER            (5)
#define CPUCACHE_FILL_SIZE              (15)
#define CPUCACHE_CAPACITY               (31)
#define DEFAULT_SIZE_CACHE_BATCHSIZE    (64)
#define NUM_SIZE_CACHES                 (16)
#define ONSLAB_DESCRIPTOR_SIZE          (512)

struct page {
    unsigned long allocated: 1;
    unsigned long reserved: 1;
    unsigned long valid: 1;
    unsigned long current_order: 4;
    unsigned long original_order: 4;
    unsigned long pfn: 53;
    union {
        struct list pagelist;
        struct {
            struct slab *slab;
            struct cache *cache;
        };
    };
    unsigned long refcount;
};

struct slab {
    struct list slablist;
    struct page *page;
    struct cache *cache;
    void *block;
    unsigned int usage;
    unsigned int next_free;
};

struct cpucache {
    unsigned int free;
    unsigned int capacity;
};

struct cache {
    char name[32];
    struct list cachelist;
    unsigned long objsize;
    unsigned int batchsize;
    unsigned int freecount;
    unsigned int capacity;
    unsigned int pageorder;
    struct spinlock lock;
    struct cpucache *cpucaches[NUM_CPUS];
    struct list slabsfull;
    struct list slabspart;
    struct list slabsfree;
};

void *alloc_cache_obj(struct cache *cache);
struct page *alloc_pages(unsigned int order);
void *cake_alloc(unsigned long size);
void cake_free(void *obj);
void free_pages(struct page *page);

#endif
