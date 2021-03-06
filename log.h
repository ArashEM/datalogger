/*   log.h     all API and data structures about log and logging
*              most of ideas are drived from linux kernel source.
*
*    Copyright (c):     (2017)  Arash Golgol (arash.golgol@gmail.com)                  
*/

#ifndef	_LOG_H_
#define _LOG_H_

#include <stdlib.h>

#include "list.h"

typedef unsigned long long int tick_t;
typedef unsigned int atomic_t;


enum    data_type {
        INT,
        DOUBLE,
        ARRAY
};

struct  timesec {
        unsigned int    sec;
        unsigned int    m_sec;
};

/** struct samplespec: about 'How offten' take sample (sample_interval)
*                      and 'How many' sample to take
*   @sample_interavl:  time between each sample. in milisecond resolution
*   @nr_sample:        how many sample is needed. it can be (-1) for 
*                      infinite sampling.
*/
struct  samplespec {
        struct   timesec sample_interval;
        int      nr_samples;
};

/** struct log_data: abstract all about data which is going to 
*                    be logged.
*   @buff:           pointer to buffer for saving data. User must provide
*                    enough buffer for data
*   @private_data:   general pointer to be used for some odd data aqusition!
*   @init_capture:   initialize data aqusition sub system (e.g ADC, ...). 
*                    return (0) on sueccess.
*   @get_data:       abstract all about capturing data and saving it in @buff
*                    return length of captured data on success (> 0) and (-1) 
*                    on error.
*   @data_len:       length of data to capture. equal to length of buffer in
*                    @buff          
*/
struct  log_data {
        char *   buff;
        void *   private_data;
        int      (*init_capture)(struct log_data * data);
        int      (*get_data)(struct log_data * data);
        size_t   data_len;
        enum data_type type;
};

/** struct storage:     abstract all about savind data (on NandFlash,
*                       LCD, Network and ...)
*
*   @name:              storage name (e.g. "Nand")
*   @init_media:        initiate media based on it's type (e.g create file
*                       file on nand flash, connect over network, etc). return
*                       (0) on sueccess.
*   @save_data:         do the actual saving of data
*   @close_media:       end saving session of media. return (0) on success.
*   @stroage_data:      general pointer for specific storage data (e.g ip addr)
*/
struct storage {
       char *   name;
       int      (*init_media)(void * storage_data);
       int      (*save_data)(struct log_data * data, void * storage_data);
       int      (*close_media)(struct log_data *data, void * storage_data);
       void *   storage_data;
};

struct log {
       struct  log_data data;
       struct  samplespec sampling_rate;
       struct  storage    storage_media;
};

/** struct logger_stat:  Abtract about run time statistics of specific logger
*
*   @nr_get_fails:       How many times get_data() return value (<= 0)
*   @nr_save_fails:      How many times save_data() return value (<= 0)
*   @get_sata:           Function pointer to proper statistics analyzer
*/
struct logger_stat {
       unsigned int nr_get_fails;
       unsigned int nr_save_fails;
       int (*get_stat)(struct logger_stat * stat);
};

struct logger {
       const char *       name;
       struct log         log;
       struct logger_stat stat;
       tick_t             nr_ticks;
       tick_t             counter;
       struct list_head   l_list;
       
};

/**
*   general user fucntions
*/
int    init_logger(void);
int    register_logger(struct logger *);
int    unregister_logger(struct logger *);
int    logger_task(void);
int    logger_zombie_task(void);
struct logger * logger_alloc(void);
void   logger_free(struct logger *);

unsigned int nr_free_logger(void);
unsigned int nr_running_logger(void);
unsigned int nr_zombie_logger(void);

/**
*  simple wrapper around struct logger to initialize it's internals
*/
void logger_set_nr_samples(struct logger *, int);
int  logger_get_nr_samples(struct logger *);



/**
*  general MACROs
*/
#define __not_safe             /* simple indicator for thread unsafe functions */

/**
*   internal functions
*/
void   __init_logger_list(struct list_head * list, struct logger * array, int array_size);
tick_t __calculate_tick(struct samplespec * interval);
int    __log_get_data(struct log *);
int    __log_save_data(struct log *);
int    __log_close_media(struct log *);
int    __logger_get_stat(struct logger *);
unsigned int __nr_logger(struct list_head *);
void   misc_test(void *parg);

#endif
