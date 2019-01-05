#ifndef __LISTS_H__
#define __LISTS_H__
#include <common/piedef.h>

#ifdef __cplusplus
extern "C"{
#endif

#define LIST_HEAD_INIT(name)    {&(name), &(name)}
#define LIST_HEAD(name)\
        struct list_head name = LIST_HEAD_INIT(name)

#define LIST_INIT(name)\
        struct list_head name = LIST_HEAD_INIT(name)

#define list_for_each(pos, head)\
        for(pos=(head)->next;pos!=(head);pos=pos->next)
            
#define list_for_each_safe(pos, n, head)\
        for(pos = (head)->next,n=pos->next;pos!=(head);\
            pos=n,n=pos->next)

/**
 * @brief get the struct for this entry
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define list_entry(node, type, member) \
    ((type *)((char *)(node) - (unsigned long)(&((type *)0)->member)))

/**
 * list_for_each_entry  -   iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)              \
    for (pos = list_entry((head)->next, typeof(*pos), member);  \
         &pos->member != (head);    \
         pos = list_entry(pos->member.next, typeof(*pos), member))
         
/**
 * list_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

void list_del(list_t *entry);
void list_add(list_t *head, list_t *node);
int list_empty(const list_t *head);

#define SLIST_HEAD(name)\
        slist_t name = {NULL}

#define SLIST_INIT(name)\
        slist_t name = {NULL}


#ifdef __cplusplus
}
#endif
#endif