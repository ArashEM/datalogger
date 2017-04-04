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
       /* check for proper pointer initialization */
       if( (!new->log.data.init_capture) || (!new->log.data.get_data))
               return -EINVAL;
       if( !new->log.data.buff )
               return -ENOMEM;
       if( (!new->log.storage_media.init_media)  ||
           (!new->log.storage_media.save_data)   ||
           (!new->log.storage_media.close_media) )
               return -EINVAL;
       /* none zero sampling interval */
       if( (!new->log.sampling_rate.sample_interval.sec) &&
           (!new->log.sampling_rate.sample_interval.m_sec))
               return -EINVAL;
       /* none zero number of samples. (-1) is for infinite sampling*/
       if( (new->log.sampling_rate.nr_samples == 0 ) ||
           (new->log.sampling_rate.nr_samples < -1) )
               return -EINVAL;
       /* initiate tick value. interval of sampling must be at 
          least '1000/TICK_RATE_HZ' */
       new->nr_ticks = __calculate_tick(&new->log.sampling_rate);
       if(!new->nr_ticks)
               return -EINVAL;
      
       /* initiate counter to nr_tick */
       new->counter = new->nr_ticks;

       /* initiate data capture sub system */
       if( !new->log.data.init_capture(new->log.data.private_data) )
               return -EIO;

       /* initiate media to save data */
       if( !new->log.storage_media.init_media(new->log.storage_media.storage_data) )
               return -EIO;


       /* add new to logger_running_list */
       /* TODO: shared resource access */
       list_add(&new->l_list, &logger_running_list);
       
    return 0;
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

/** logger_task(): Iterate over logger_running_list and do logging
*                  if counter reach 0
*   TODO:          SHARED resource access.
*/
int logger_task(void)
{
       struct logger    * tmp;
       struct list_head * pos;

       list_for_each(pos, &logger_running_list){
               tmp = list_entry(pos, struct logger, l_list);
               
               /* register_logger checked value of counter before */
               tmp->counter--;
               /* is it time to sample? */
               if(!tmp->counter){
                       if(__log_get_data(&tmp->log) > 0){
                       /* successful sampling */
                               if(__log_save_data(&tmp->log) <= 0)
                               /* unsuccessfule saving! lost a sample */
                                         tmp->stat.nr_save_fails++;
                       }
                       else{
                               tmp->stat.nr_get_fails++;
                       }

                       /* check for nr_samples */
                       switch(tmp->log.sampling_rate.nr_samples){
                               case (-1):
                                       /* infinite sampling */
                                       tmp->counter = tmp->nr_ticks;
                                       break;
                               case (0):
                                       /* end of sampling */
                                       /* move from running to zombie list */
                                       list_move(&tmp->l_list, &logger_zombie_list);
                                       break;
                               default:
                                       /* go on */
                                       tmp->log.sampling_rate.nr_samples--;
                                       tmp->counter = tmp->nr_ticks;
                                       break;
                       } /* switch(tmp->...) */    
               } /* if(!tmp->counter) */
       }/*list_for_each(...) */
     return 0;
}

/** logger_zombie_task(): run to get statistics about ended logger 
*                         which (nr_samples == 0). after calling this
*                         functino all logger in zombie list will move
*                         to free list
*
*  return:                (0) if all zombies statistics are gotten successfully
*                         each logger which provides it statistics successfully
*                         is moved to logger_free_list.
*                         (-EIO) if at least one logger failed in providing 
*                         stats.
*/
int logger_zombie_task(void)
{
       struct list_head * pos;
       struct logger    * tmp;
       int    ret = 0;

       /* run over zombie_list */
       list_for_each(pos, &logger_zombie_list)
       {
               tmp = list_entry(pos, struct logger, l_list);
               if(__logger_get_stat(tmp) > 0){
                       /* getting successfully statistics */
                       list_move(&tmp->l_list, &logger_free_list);
               }
               else {
                       /* logger will stay in zobmie list because of error */
                       ret = -EIO;
               }       
       } /* list_for_each() */
       return ret;
}

/** __calculate_tick(): how many tick must passed between each sampling?
*
*   @interval:          pinter to sample descriptor structure
*/
tick_t __calculate_tick(struct samplespec * interval)
{
       int msec;
       msec = 1000 * (interval->sample_interval.sec) + 
                      interval->sample_interval.m_sec;

       return (tick_t)(msec * TICK_RATE_HZ / 1000 );
}

/** __init_logger_list(): create a linked list from array of logger
*
*   @list:                head of linked list
*   @array:               pointer to array of logger structure
*   @array_size:          number of element in array
*/
void   __init_logger_list(struct list_head * list, struct logger * array, int array_size)
{
       int i;
       for(i=0; i<array_size; i++){
                array->nr_ticks = (tick_t)i; 
                list_add(&array->l_list, list);
                array++;
       }
}

/** __log_get_data(): call get_data function inside log_data
*                     (used for abtraction of log_data internals)
*   @log:             pointer to log structure
*
*   NOTE:             This function check for valid pointer on 'get_data'
*/
int __log_get_data(struct log * log)
{
      if( !log->data.get_data )
               return -EINVAL;
      return log->data.get_data(log->data.buff, log->data.data_len, 
                                log->data.private_data);
}

/** __log_save_data(): call to save capture data (resident inside 
*                      log->data.buf) to sotrage media.
*
*   @log:             pointer to log structure
*/
int __log_save_data(struct log * log)
{
       if( !log->storage_media.save_data)
               return -EINVAL;
       return log->storage_media.save_data(&log->data, 
                                           log->storage_media.storage_data);
}

/** __logger_get_stat(): call to process statistics of logger
*
*   @logger:             pointer to logger for getting stats.
*/
int __logger_get_stat(struct logger * logger)
{
       if( !logger->stat.get_stat )
               return -EINVAL;
       return logger->stat.get_stat(&logger->stat);
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
            printf("tick = %d , i = %d\n", (int)logger->nr_ticks, i++);
      }

}



