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
#include "cake/string.h"
#include "arch/lock.h"
#include "arch/page.h"
#include "arch/smp.h"

#define PAGE_IS_TAIL(page)      ((page->pfn) & ((1 << ((page->current_order) + 1)) - 1))
#define PAGE_IS_HEAD(page)      (!(PAGE_IS_TAIL(page)))
#define HEAD_BUDDY_PFN(page)    ((page->pfn) - (1 << (page->current_order)))
#define TAIL_BUDDY_PFN(page)    ((page->pfn) + (1 << (page->current_order)))
#define HEAD_BUDDY(page)        (&(system_phys_page_dir[HEAD_BUDDY_PFN(page)]))
#define TAIL_BUDDY(page)        (&(system_phys_page_dir[TAIL_BUDDY_PFN(page)]))
#define SHOULD_COALESCE(page, buddy)                        \
    (!(buddy->allocated) &&                                 \
    ((page->current_order) == (buddy->current_order)) &&    \
    ((page->current_order) < (page->original_order))  &&    \
    ((buddy->current_order) < (buddy->original_order)))

#define LOG2(num)           ((unsigned) (8 * sizeof (unsigned long long) - \
    __builtin_clzll((num)) - 1))
#define LOG2_SAFE(num)      num == 0 ? 0 : LOG2(num)
#define PAGE_TO_PTR(page)   PFN_TO_PTR((page->pfn))
#define PTR_TO_PAGE(ptr)    system_phys_page_dir[PTR_TO_PFN((ptr))]

#define CPUCACHE_DATA(cache) ((void **) (((struct cpucache *) (cache)) + 1))
#define SLAB_FREE_STACK(slab) ((unsigned int *) (((struct slab *) (slab)) + 1))
#define OBJ_CACHE(ptr) (&(PTR_TO_PAGE(ptr)))->cache
#define OBJ_SLAB(ptr) (&(PTR_TO_PAGE(ptr)))->slab

extern void arch_populate_allocate_structures(struct page **system_phys_page_dir, 
    struct list *freelists);

static struct cpucache **alloc_cpucaches();
static unsigned int cake_alloc_index(unsigned long size);
static void free_object_to_cache_pool();
static void fill_cpucache();
static void *next_free_obj(struct cache *cache);
static unsigned int resize_batch(unsigned long numpages, unsigned long objsize);
static void setup_cache_cache();
static void setup_size_caches();

static struct list freelists[MAX_ORDER + 1];
static struct page *system_phys_page_dir;
static struct cache sizecaches[NUM_SIZE_CACHES];
static struct cache cache_cache = {
    .name = {'c', 'a', 'c', 'h', 'e', '\0'},
    .objsize = sizeof(struct cache),
    .batchsize = 64,
    .freecount = 0,
    .capacity = 0,
    .lock = {
        .owner = 0,
        .ticket = 0
    },
    .slabsfull = {
        .prev = &(cache_cache.slabsfull),
        .next = &(cache_cache.slabsfull)
    },
    .slabspart = {
        .prev = &(cache_cache.slabspart),
        .next = &(cache_cache.slabspart)
    },
    .slabsfree = {
        .prev = &(cache_cache.slabsfree),
        .next = &(cache_cache.slabsfree)
    }
};
static struct list cachelist = {
    .prev = &cachelist,
    .next = &cachelist
};

struct cache *alloc_cache(char *name, unsigned long objsize)
{
    log("Allocating %s cache\r\n", name);
    unsigned int lgrm, numpages, batchsize;
    struct cpucache **ref;
    struct list *li;
    struct cache *cache = alloc_cache_obj(&cache_cache);
    memset(cache, 0, sizeof(*cache));
    batchsize = 64;
    ref = alloc_cpucaches();
    numpages = (objsize * batchsize) / PAGE_SIZE;
    lgrm = LOG2_SAFE(numpages);
    lgrm = lgrm > MAX_ORDER ?  MAX_ORDER : lgrm;
    strcpy(cache->name, name);
    cache->objsize = objsize;
    cache->batchsize = resize_batch((1 << lgrm), objsize);
    cache->freecount = 0;
    cache->capacity = 0;
    cache->pageorder = lgrm;
    for(unsigned int i = 0; i < NUM_CPUS; i++) {
        cache->cpucaches[i] = ref[i];
    }
    li = &(cache->slabsfull);
    li->prev = li;
    li->next = li;
    li = &(cache->slabspart);
    li->prev = li;
    li->next = li;
    li = &(cache->slabsfree);
    li->prev = li;
    li->next = li;
    list_add(&(cachelist), &(cache->cachelist));
    return cache;
}

void *alloc_cache_obj(struct cache *cache)
{
    struct cpucache *cpucache;
    unsigned long cpuid = SMP_ID();
    cpucache = cache->cpucaches[cpuid];
    if(!(cpucache->free)) {
        SPIN_LOCK(&(cache->lock));
        fill_cpucache(cache, cpucache);
        SPIN_UNLOCK(&(cache->lock));
    }
    return CPUCACHE_DATA(cpucache)[--cpucache->free];
}

static struct cpucache **alloc_cpucaches()
{
    log("Allocating CPU caches...\r\n");
    struct cpucache *cpucache;
    unsigned int cpucache_capacity = CPUCACHE_CAPACITY + 1;
    unsigned long reference_size = NUM_CPUS * sizeof(struct cpucache *);
    unsigned long per_cpu_allocation = cpucache_capacity * sizeof(struct cpucache);
    struct cpucache **cpucaches = cake_alloc(reference_size);
    memset(cpucaches, 0, reference_size);
    for(unsigned int i = 0; i < NUM_CPUS; i++) {
        cpucache = cake_alloc(per_cpu_allocation);
        memset(cpucache, 0, per_cpu_allocation);
        cpucache->free = 0;
        cpucache->capacity = CPUCACHE_CAPACITY;
        cpucaches[i] = cpucache;
        log("Allocated cpucache object at address %x\r\n", cpucache);
    }
    return cpucaches;
}

struct page *alloc_pages(unsigned int order)
{
    struct list *freelist;
    struct page *buddy, *s;
    struct page *p = 0;
    unsigned long pfn, bpfn;
    unsigned int i = order;
    while(i <= MAX_ORDER) {
        freelist = &(freelists[i]);
        if(!list_empty(freelist)) {
            p = LIST_FIRST_ENTRY(freelist, struct page, pagelist);
            list_delete(&(p->pagelist));
            break;
        }
        i++;
    }
    if(p) {
        while(i > order) {
            --i;
            p->current_order = i;
            freelist = &(freelists[i]);
            list_add(freelist, &(p->pagelist));
            pfn = p->pfn;
            bpfn = pfn + (1 << i);
            buddy = &(system_phys_page_dir[bpfn]);
            buddy->valid = 1;
            buddy->original_order = i + 1;
            buddy->current_order = i;
            p = buddy;
        }
        for(i = p->pfn; i < (p->pfn + (1 << order)); i++) {
            s = &(system_phys_page_dir[i]);
            s->allocated = 1;
        }
    }
    return p;
}

static void allocate_cache_slab(struct cache *cache)
{
    unsigned long slab_overhead = sizeof(struct slab);
    unsigned long free_tracking_size = cache->batchsize * sizeof(unsigned int);
    struct page *page;
    struct slab *slab;
    void *memblock;
    page = alloc_pages(cache->pageorder);
    memblock = PAGE_TO_PTR(page);
    log("Creating slab for object of size %x\r\n", cache->objsize);
    if(cache->objsize > ONSLAB_DESCRIPTOR_SIZE) {
        slab = cake_alloc(slab_overhead + free_tracking_size);
    }
    else {
        slab = PAGE_TO_PTR(page);
        memblock += (slab_overhead + free_tracking_size);
        while((unsigned long) memblock & ((cache->objsize) - 1)) {
            memblock += sizeof(void *);
        }
    }
    slab->page = page;
    slab->cache = cache;
    slab->block = memblock;
    slab->usage = 0;
    slab->next_free = 0;
    for(unsigned int i = 0; i < cache->batchsize; i++) {
        SLAB_FREE_STACK(slab)[i] = i;
    }
    for(unsigned long i = page->pfn; i < page->pfn + (1 << cache->pageorder); i++) {
        struct page *p = &(system_phys_page_dir[i]);
        p->slab = slab;
        p->cache = cache;
    }
    list_add(&(cache->slabsfree), &(slab->slablist));
    cache->capacity += cache->batchsize;
    cache->freecount += cache->batchsize;
    log("Object size: %x\r\n", cache->objsize);
    log("Address of slab: %x\r\n", slab);
    log("Address of freetracking: %x\r\n", SLAB_FREE_STACK(slab));
    log("Address of stuff: %x\r\n", slab->block);
}

void allocate_init()
{
    unsigned int count = 32;
    void *ptrs[count];
    for(unsigned int i = 0; i <= MAX_ORDER; i++) {
        struct list *freelist = &(freelists[i]);
        freelist->next = freelist;
        freelist->prev = freelist;
    }
    arch_populate_allocate_structures(&system_phys_page_dir, freelists);
    setup_size_caches();
    setup_cache_cache();
    for(int i = 0; i < count; i++) {
        ptrs[i] = cake_alloc(64);
    }
    for(int i = count - 1; i >= 0; i--) {
        cake_free(ptrs[i]);
    }
    for(int i = 0; i < count; i++) {
        ptrs[i] = cake_alloc(64);
    }
    for(int i = 0; i < count; i++) {
        cake_free(ptrs[i]);
    }
}

void *cake_alloc(unsigned long size)
{
    struct cache *sizecache;
    unsigned int index; 
    index = cake_alloc_index(size);
    sizecache = &(sizecaches[index]);
    return alloc_cache_obj(sizecache);
}

static unsigned int cake_alloc_index(unsigned long size)
{
    if(size <= 32) {
        return 0;
    }
    if(size <= 64) {
        return 1;
    }
    if(size <= 128) {
        return 2;
    }
    if(size <= 256) {
        return 3;
    }
    if(size <= 512) {
        return 4;
    }
    if(size <= 1024) {
        return 5;
    }
    if(size <= 2 * 1024) {
        return 6;
    }
    if(size <= 4 * 1024) {
        return 7;
    }
    if(size <= 8 * 1024) {
        return 8;
    }
    if(size <= 16 * 1024) {
        return 9;
    }
    if(size <= 32 * 1024) {
        return 10;
    }
    if(size <= 64 * 1024) {
        return 11;
    }
    if(size <= 128 * 1024) {
        return 12;
    }
    if(size <= 256 * 1024) {
        return 13;
    }
    if(size <= 512 * 1024) {
        return 14;
    }
    if(size <= 1024 * 1024) {
        return 15;
    }
    return -1;
}

void cake_free(void *obj)
{
    struct cpucache *cpucache;
    struct cache *cache = OBJ_CACHE(obj);
    unsigned long cpuid = SMP_ID();
    cpucache = cache->cpucaches[cpuid];
    CPUCACHE_DATA(cpucache)[cpucache->free++] = obj;
    log("Freed object: %x\r\n", obj);
    if(cpucache->free == CPUCACHE_CAPACITY) {
        SPIN_LOCK(&cache->lock);
        for(unsigned int i = CPUCACHE_CAPACITY; i > CPUCACHE_FILL_SIZE; i--) {
            free_object_to_cache_pool(cache, cpucache);
        }
        SPIN_UNLOCK(&cache->lock);
    }
}

static void fill_cpucache(struct cache *cache, struct cpucache *cpucache)
{
    while(cache->freecount < CPUCACHE_FILL_SIZE) {
        log("Need to allocate a new cache slab...\r\n");
        allocate_cache_slab(cache);
        log("Free slab objects now = %x\r\n", cache->freecount);
    }
    while(cpucache->free < CPUCACHE_FILL_SIZE) {
        CPUCACHE_DATA(cpucache)[cpucache->free++] = next_free_obj(cache);
        log(
            "Added obeject %x to cache at addr %x\r\n",
            CPUCACHE_DATA(cpucache)[cpucache->free - 1],
            &(CPUCACHE_DATA(cpucache)[cpucache->free - 1])
        );
    }
}

static void free_object_to_cache_pool(struct cache *cache, struct cpucache *cpucache)
{
    void *obj = CPUCACHE_DATA(cpucache)[--cpucache->free];
    struct slab *slab = OBJ_SLAB(obj);
    unsigned long index = (obj - (slab->block)) / cache->objsize;
    log("Returning object %x with index %x to the cache pool\r\n", obj, index);
    if(slab->usage == cache->batchsize) {
        list_delete(&(slab->slablist));
        list_add(&(cache->slabspart), &(slab->slablist));
    }
    SLAB_FREE_STACK(slab)[--slab->next_free] = (unsigned int) index;
    cache->freecount++;
    slab->usage--;
    if(!slab->usage) {
        list_delete(&(slab->slablist));
        list_add(&(cache->slabsfree), &(slab->slablist));
    }
}

void free_pages(struct page *page)
{
    struct page *buddy, *s;
    for(unsigned long i = page->pfn; i < (page->pfn + (1 << page->current_order)); i++) {
        s = &(system_phys_page_dir[i]);
        s->allocated = 0;
    }
    while(1) {
        if(PAGE_IS_HEAD(page)) {
            buddy = TAIL_BUDDY(page);
            if(SHOULD_COALESCE(page, buddy)) {
                list_delete(&(buddy->pagelist));
                buddy->valid = 0;
                buddy->current_order = 0;
                buddy->original_order = 0;
                page->current_order++;
                continue;
            }
        }
        else {
            buddy = HEAD_BUDDY(page);
            if(SHOULD_COALESCE(page, buddy)) {
                list_delete(&(buddy->pagelist));
                page->valid = 0;
                page->current_order = 0;
                page->original_order = 0;
                buddy->current_order++;
                page = buddy;
                continue;
            }
        }
        break;
    }
    list_add(&(freelists[page->current_order]), &(page->pagelist));
}

static void *next_free_obj(struct cache *cache)
{
    void *obj;
    unsigned int free_index;
    struct slab *slab;
    if(!list_empty(&(cache->slabspart))) {
        slab = LIST_FIRST_ENTRY(&(cache->slabspart), struct slab, slablist);
    }
    else {
        slab = LIST_FIRST_ENTRY(&(cache->slabsfree), struct slab, slablist);
        list_delete(&(slab->slablist));
        list_add(&(cache->slabspart), &(slab->slablist));
    }
    free_index = SLAB_FREE_STACK(slab)[slab->next_free++];
    obj = slab->block + (cache->objsize * free_index);
    slab->usage++;
    cache->freecount--;
    if(slab->usage == cache->batchsize) {
        list_delete(&(slab->slablist));
        list_add(&(cache->slabsfull), &(slab->slablist));
    }
    return obj;
}

static unsigned int resize_batch(unsigned long numpages, unsigned long objsize)
{
    unsigned long batchsize, tracking_overhead, slab_overhead, round_down_to_even_mask;
    if(objsize > ONSLAB_DESCRIPTOR_SIZE) {
        batchsize = (numpages * PAGE_SIZE) / objsize;
    }
    else {
        tracking_overhead = sizeof(unsigned int);
        slab_overhead = sizeof(struct slab);
        objsize += tracking_overhead;
        round_down_to_even_mask = ~(1UL);
        batchsize = (((numpages * PAGE_SIZE) - slab_overhead) / objsize) - 1;
        batchsize &= round_down_to_even_mask;
    }
    return batchsize;
}

static void setup_cache_cache()
{
    log("Setting up cache cache\r\n");
    unsigned int lgrm, numpages;
    struct cache *cache = &cache_cache;
    struct cpucache **ref;
    ref = alloc_cpucaches();
    numpages = (cache->objsize * cache->batchsize / PAGE_SIZE);
    lgrm = LOG2_SAFE(numpages);
    lgrm = lgrm > MAX_ORDER ? MAX_ORDER : lgrm;
    cache->batchsize = resize_batch((1 << lgrm), cache->objsize);
    cache->pageorder = lgrm;
    for(unsigned int i = 0; i < NUM_CPUS; i++) {
        cache->cpucaches[i] = ref[i];
    }
    list_add(&cachelist, &(cache->cachelist));
}

static void setup_size_caches()
{
    unsigned int lgrm, numpages, batchsize;
    unsigned long objsize;
    struct cache *sizecache;
    struct list *li;
    struct spinlock *lock;
    struct page *ppa, *ppb;
    struct cpucache *cpucache, **ref, *old, *new;
    void *cpucache_ptrs, *cpucache_blocks;
    ppa = alloc_pages(0);
    ppb = alloc_pages(3);
    cpucache_ptrs = PAGE_TO_PTR(ppa);
    cpucache_blocks = PAGE_TO_PTR(ppb);
    log("References: %x\r\n", cpucache_ptrs);
    log("Caches: %x\r\n", cpucache_blocks);
    for(unsigned int i = 0; i < NUM_SIZE_CACHES; i++) {
        sizecache = &(sizecaches[i]);
        ref = &(((struct cpucache **) cpucache_ptrs)[i * NUM_CPUS]);
        cpucache = (struct cpucache *) (((unsigned long) cpucache_blocks) + (i * 32 * 8));
        cpucache->free = 0;
        cpucache->capacity = CPUCACHE_CAPACITY;
        ref[0] = cpucache;
        objsize = 1 << (i + MIN_SIZE_CACHE_ORDER);
        batchsize = DEFAULT_SIZE_CACHE_BATCHSIZE;
        numpages = (objsize * batchsize) / PAGE_SIZE;
        lgrm = LOG2_SAFE(numpages);
        lgrm = lgrm > MAX_ORDER ? MAX_ORDER : lgrm;
        batchsize = resize_batch((1 << lgrm), objsize);
        sizecache->objsize = objsize;
        sizecache->batchsize = batchsize;
        sizecache->freecount = 0;
        sizecache->capacity = 0;
        sizecache->pageorder = lgrm;
        sizecache->cpucaches[0] = ref[0];
        lock = &(sizecache->lock);
        lock->owner = 0;
        lock->ticket = 0;
        li = &(sizecache->slabsfull);
        li->next = li;
        li->prev = li;
        li = &(sizecache->slabspart);
        li->next = li;
        li->prev = li;
        li = &(sizecache->slabsfree);
        li->next = li;
        li->prev = li;
    }
    for(unsigned int i = 0; i < NUM_SIZE_CACHES; i++) {
        sizecache = &(sizecaches[i]);
        log("Allocating CPU caches for size cache %x\r\n", sizecache->objsize);
        ref = alloc_cpucaches();
        old = sizecache->cpucaches[0];
        new = ref[0];
        new->free = old->free;
        log("Pointer to CPU caches: %x\r\n", ref);
        for(unsigned int j = 0; j < NUM_CPUS; j++) {
            log("Pointer to CPU cache %x: %x\r\n", j, ref[j]);
            sizecache->cpucaches[j] = ref[j];
        }
        log("Address of old cache: %x\r\n", old);
        log("Address of new cache: %x\r\n", new);
        for(unsigned int j = 0; j < new->free; j++) {
            log("Replacing old with new...\r\n");
            log(
                "Replacing addr %x with addr %x\r\n", 
                &(CPUCACHE_DATA(old)[j]), 
                &(CPUCACHE_DATA(new)[j])
            );
            CPUCACHE_DATA(new)[j] = CPUCACHE_DATA(old)[j];
        }
        log("Done with setup for cachesize %x\r\n", sizecache->objsize);
    }
    free_pages(ppa);
    free_pages(ppb);
}
