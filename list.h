/**
*      double linked list based on linux kernel linked list
*      code is highly drived form include/linux/list.h
*      
*      note: some gcc specific feature like typeof() are 
*      removed.
*      Copyright(c):  (2017) Arash Golgol (arash.golgol@gmail.com)
*/

#ifndef LIST_H
#define LIST_H

#include <stdlib.h>


struct list_head {
       struct list_head * next;
       struct list_head * prev;
};

#define LIST_HEAD_INIT(name)  { &(name), &(name) }

#define LIST_HEAD(name) \
        struct list_head name = LIST_HEAD_INIT(name)


void __list_add(struct list_head * new, struct list_head * prev, 
                struct list_head * next);
void list_add(struct list_head *new, struct list_head *head);
void list_add_tail(struct list_head *new, struct list_head *head);


void __list_del(struct list_head * prev, struct list_head * next);
void list_del(struct list_head * entry);
void list_move(struct list_head *entry, struct list_head * head);

int list_empty(struct list_head * head);


#define offsetof(type, member) \
        (size_t)(&(((type *)0)->member) )

#define containter_of(ptr, type, member) \
        (type *)( (char *)ptr - offsetof(type, member) )

/**
 * list_entry - get the struct for this entry
 * @ptr:        the &struct list_head pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
        containter_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:        the list head to take the element from.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
        list_entry((ptr)->next, type, member)

/**
 * list_for_each        -       iterate over a list
 * @pos:        the &struct list_head to use as a loop cursor.
 * @head:       the head for your list.
 */
#define list_for_each(pos, head) \
        for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev   -       iterate over a list backwards
 * @pos:        the &struct list_head to use as a loop cursor.
 * @head:       the head for your list.
 */
#define list_for_each_prev(pos, head) \
        for (pos = (head)->prev; pos != (head); pos = pos->prev)


#endif
