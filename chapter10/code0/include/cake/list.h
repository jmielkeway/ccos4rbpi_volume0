#ifndef _CAKE_LIST_H
#define _CAKE_LIST_H

#include "cake/cake.h"
#include "cake/compiler.h"

#define LIST_ENTRY(ptr, type, member)   \
    CONTAINER_OF(ptr, type, member)

#define LIST_FIRST_ENTRY(ptr, type, member)     \
        LIST_ENTRY((ptr)->next, type, member)

#define LIST_NEXT_ENTRY(ptr, member)                            \
        LIST_ENTRY((ptr)->member.next, typeof(*(ptr)), member)

#define LIST_FOR_EACH_ENTRY(loop_ptr, head, member)                     \
        for (loop_ptr = LIST_FIRST_ENTRY(head, typeof(*loop_ptr), member);   \
             &loop_ptr->member != (head);                               \
             loop_ptr = LIST_NEXT_ENTRY(loop_ptr, member))

#define LIST_FOR_EACH_ENTRY_SAFE(pos, n, head, member)          \
    for (pos = LIST_FIRST_ENTRY(head, typeof(*pos), member),    \
        n = LIST_NEXT_ENTRY(pos, member);           \
         &pos->member != (head);                    \
         pos = n, n = LIST_NEXT_ENTRY(n, member))

struct list {
    struct list *next;
    struct list *prev;
};

static inline void list_add(struct list *list, struct list *element)
{
    struct list *prev, *next;
    prev = list;
    next = list->next;
    next->prev = element;
    element->next = next;
    element->prev = prev;
    WRITE_ONCE(prev->next, element);
}

static inline int list_empty(struct list *list)
{
    return READ_ONCE(list->next) == list;
}

static inline void list_enqueue(struct list *list, struct list *element)
{
    struct list *prev, *next;
    prev = list->prev;
    next = list;
    next->prev = element;
    element->next = next;
    element->prev = prev;
    WRITE_ONCE(prev->next, element);
}

static inline void list_delete(struct list *element)
{
    struct list *next, *prev;
    next = element->next;
    prev = element->prev;
    prev->next = next;
    WRITE_ONCE(next->prev, prev);
    element->next = 0;
    element->prev = 0;
}

static inline void list_delete_reset(struct list *element)
{
    struct list *next, *prev;
    next = element->next;
    prev = element->prev;
    prev->next = next;
    WRITE_ONCE(next->prev, prev);
    element->next = element;
    element->prev = element;
}

#endif
