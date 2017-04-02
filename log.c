/*
*   early implementation of logger
*   log.c
*
*   Copyright (C): arash golgol (2017)
*/

#include <stdio.h>
#include <string.h>

#include "noerr.h"
#include "log.h"
#include "list.h"
#include "config.h"

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
       /* check for proper initialization */
       if( (!new->log.data.init_capture) || (!new->log.data.get_data))
               return -EINVAL;
       if( !new->log.data.buff )
               return -ENOMEM;
       if( (!new->log.storage_media.init_media)  ||
           (!new->log.storage_media.save_data)   ||
           (!new->log.storage_media.close_media) )
               return -EINVAL;
       if( (!new->log.sampling_rate.sample_interval.sec) &&
           (!new->log.sampling_rate.sample_interval.m_sec))
               return -EINVAL;
       /* initiate tick value. interval of sampling must be at 
          last '1000/TICK_RATE_HZ' */
       new->nr_tick = __calculate_tick(&new->log.sampling_rate);
       if(!new->nr_tick)
               return -EINVAL;

       /* initiate data capture sub system */
       if( !new->log.data.init_capture(new->log.data.private_data) )
               return -EIO;

       /* initiate media to save data */
       if( !new->log.storage_media.init_media(new->log.storage_media.storage_data) )
               return -EIO;


       /* add new to logger_running_list */
       /* TODO: shared resource access */
       list_add(&new->l_list, &logger_free_list);
       
    return 0;
}


void   __init_logger_list(struct list_head * list, struct logger * array, int array_size)
{
       int i;
       for(i=0; i<array_size; i++){
                array->nr_tick = (tick_t)i; 
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
     __init_logger_list(&logger_free_list, logger_array, NR_LOGGER);

     return 0;
}

/** logger_alloc(): get a new logger struct from logger_free_list
*   return:         pointer to allocated logger or NULL if it's not
*                   available.
*
*   TODO:           this function access SHARED resource. consider 
*                   locking mechanism.
*/
struct logger * logger_alloc(void)
{
       struct logger * tmp;
      
       if(list_empty(&logger_free_list))
               return NULL;
       /* get first logger element in list */
       tmp = list_first_entry(&logger_free_list, struct logger, l_list);      

       /* delete allocated logger from free list */
       list_del(&tmp->l_list);
       
       /* safty issue: clean tmp before return*/
       memset((void *)tmp, 0, sizeof(struct logger));

       return tmp;            
}

/** logger_free(): add logger struct to logger_free_list
*   @unused:       pointer to logger to attach to list   
*
*   NOTE:          @unused is valid pointer after calling logger_free()
*                  but it can be used by other thread of execution by call
*                  of logger_alloc(). So it's user responsibility not to
*                  use it after this call.
*
*   TODO:          SHARED resource access.
*/
void logger_free(struct logger * unused)
{
       if(!unused)
               return;
        
       /* add to logger_free_list */
       list_add(&unused->l_list, &logger_free_list);
}

/** logger_task()
*
*/
int logger_task(void)
{
       return 0;
}


/** __calculate_tick(): how many tick must passed between each sampling?
*
*   @interval:          pinter to sample descriptor structure
*/
tick_t __calculate_tick(struct samplespec * interval)
{
       int msec;
       msec = 1000 * (interval->sample_interval.sec) + interval->sample_interval.m_sec;
       return (tick_t)(msec * TICK_RATE_HZ / 1000 );
}
/**  misc_test()
*    used for log.c interal variable test
*/
void misc_test(void * parg)
{
      struct list_head * pos;
      struct logger    * logger;
      int i = 0;
      list_for_each(pos, &logger_free_list){
            logger = list_entry(pos, struct logger, l_list);
            printf("tick = %d , i = %d\n", (int)logger->nr_tick, i++);
      }

}



