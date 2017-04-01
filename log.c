/*
*   early implementation of logger
*   log.c
*
*   Copyright (C): arash golgol (2017)
*/

#include <string.h>
#include "log.h"
#include "list.h"
#include "config.h"
#include <stdio.h>

/**
*   main array of loggers. it's only accessed via log.c functions
*/
struct logger logger_array[NR_LOGGER];


LIST_HEAD(logger_free_list);
LIST_HEAD(logger_running_list);
LIST_HEAD(logger_zombie_list);


/* register_logger()
*  add a logger structure to linked list of running logger
*  each tick ISR call logger_task() to iterate over all 
*  elements of list and apply logging stuf.
*  
*  @new:    logger element to add to data_logger list
*  return:  0 on success 
*/  
int register_logger(struct logger * new)
{
    /* CHECKS:
       in log_data structure:
         1- check for fptr 'init_capture' and 'get_data'
         2- check for ptr 'buff'
       in storage structure:
         1- check for fptr 'init_media', 'save_data' and 'close_media'
       in samplespec structure:
         1- check for none zero 'sample_interval'
       EXE:
         1- call init_capture() and check returned value
         2- call init_media() and check retured value
         3- calculate tick value based on CPUFreq and sampling_rate
         4- add 'new' to logger_running_list
    */
    return 0;
}


void   _init_logger_list(struct list_head * list, struct logger * array, int array_size)
{
       int i;
       for(i=0; i<array_size; i++){
                /* array->tick = (tick_t)i; */
                list_add(&array->l_list, list);
                array++;
       }
}

/** init_logger()
*   1- clean logger_array for safety
*   2- initialize linked list of logger_free_list
*
*   return:  0 on sucess
*/
int init_logger(void)
{
     /* clear logger_array via memset() */
     memset((void *)logger_array, 0, NR_LOGGER * sizeof(struct logger));

     /* creat linked list of free logger */
     _init_logger_list(&logger_free_list, logger_array, NR_LOGGER);

     return 0;
}

/**  misc_test()
*    used for log.c interal variable test
*/

void misc_test(void * parg)
{
      struct list_head * pos;
      struct logger    * logger;
      int i = 0;
      list_for_each_prev(pos, &logger_free_list){
            logger = list_entry(pos, struct logger, l_list);
            printf("tick = %d , i = %d\n", (int)logger->tick, i++);
      }

}



