/*   log.h     all API and data structures about log and logging
*              most of ideas are drived from linux kernel source.
*
*    Copyright (c):     (2017)  Arash Golgol (arash.golgol@gmail.com)                  
*/

#ifndef	_LOG_H_
#define _LOG_H_

#include <stdlib.h>

#include "list.h"

typedef unsigned int tick_t;
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
*   @init_capture:   initialize data aqusition sub system (e.g ADC, ...)
*   @get_data:       abstract all about capturing data and saving it in @buff
*                    return length of captured data on success (> 0) and (-1) 
*                    on error.
*   @buff_len:       length of buffer allocated by user in @buff
*   @data_len:       length of data captured          
*
*   TODO: is data_len neccesary?
*/
struct  log_data {
        char *   buff;
        void *   private_data;
        int      (*init_capture)(void * misc);
        int      (*get_data)(char * buff, size_t len, void * private_data);
        size_t   buff_len;
        size_t   data_len;
        enum data_type type;
};

/** struct storage:     abstract all about savind data (on NandFlash,
*                       LCD, Network and ...)
*
*   @name:              storage name (e.g. "Nand")
*   @init_media:        initiate media based on it's type (e.g create file
*                       file on nand flash, connect over network, etc)
*   @save_data:         do the actual saving of data
*   @close_media:       end saving session of media
*   @stroage_data:      general pointer for specific storage data (e.g ip addr)
*/
struct storage {
       char *   name;
       int      (*init_media)(struct log_data * data, void * storage_data);
       int      (*save_data)(struct log_data * data, void * storage_data);
       int      (*close_media)(struct log_data *data, void * storage_data);
       void *   storage_data;
};

struct log {
       struct  log_data data;
       struct  samplespec sampling_rate;
       struct  storage    storage_media;
};


struct logger_stat {
       unsigned int nr_get_fails;
       unsigned int nr_save_failes;
};

struct logger {
       const char *       name;
       struct log         log;
       struct logger_stat stat;
       tick_t             tick;
       struct list_head   l_list;
       
};

/**
*   general user fucntions
*/
int    init_logger(void);
int    register_logger(struct logger *);
int    unregister_logger(struct logger *);
int    logger_task(struct list_head * log_list);
struct logger * logger_alloc(void);
void   logger_free(struct logger *);

/**
*   internal functions
*/
void   _init_logger_list(struct list_head * list, struct logger * array, int array_size);
int    _add_logger(struct list_head * list, struct logger *);
int    _remove_logger(struct list_head * list, struct logger *);
void   misc_test(void *parg);

#endif
