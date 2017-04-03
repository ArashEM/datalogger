/**  
*        linked list implementation based on
*        linux kernel include/linux/list.h
*
*/

#include "list.h"

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
void __list_add(struct list_head * new, struct list_head * prev, 
                struct list_head * next){

       next->prev = new;
       new->next  = next;
       new->prev  = prev;
       prev->next = new;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
void list_add(struct list_head *new, struct list_head *head){
      __list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
void list_add_tail(struct list_head *new, struct list_head *head)
{
        __list_add(new, head->prev, head);
}

/**
* Delete a list entry by making the prev/next entries
* point to each other.
*
* This is only for internal list manipulation where we know
* the prev/next entries already!
*/
void __list_del(struct list_head * prev, struct list_head * next)
{
       next->prev = prev;           
       prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
void list_del(struct list_head * entry)
{
       __list_del(entry->prev, entry->next);
       entry->next = NULL;
       entry->prev = NULL;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
int list_empty(struct list_head * head)
{
      return ((head->next == head) && (head->prev == head));
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
void list_move(struct list_head *entry, struct list_head * head)
{
       list_del(entry);
       list_add(entry, head);
} 



