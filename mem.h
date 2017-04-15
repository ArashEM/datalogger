/**
*    simple memory allocator for embedded application
*    memory is allocated based on static arrary and 
*    a linked list
*
*    Idea is from uC-OS implementation
*
*    Copyright(C)    2017   Arash Golgol (arash.golgol@gmail.com)
*/
#ifndef MEM_H
#define MEM_H

#include "list.h"
#include "config.h"


/** struct mem_list
*   @mem:             pointer to memory array
*   @m_list:          linked list elements
*/
struct mem_list {
       char *   mem;
       struct list_head m_list;
};


void  __init_mem_list(struct list_head *, char *, size_t, size_t);

char * mem_get(void);
void   mem_put(char *);



#endif
