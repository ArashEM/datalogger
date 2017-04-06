/*
*	logger test
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "list.h"


/* init_capture() */
int logger_init_capture(struct log_data * data)
{
       	data->buff = (char *)malloc(sizeof(int));
       	if(!data->buff)
		return -1;
	memset((void *)data->buff, 0, data->data_len);

	data->data_len = sizeof(int);
	return 0;
}

/* get_data() */
int logger_get_data(struct log_data * data)
{
	int * i = (int *)data->buff;
	(*i)++;
	return data->data_len;	
}

/* media */
int logger_init_media(void * data)
{
	return 0;
}
/* media */
int logger_save_data(struct log_data * data, void * storage_data)
{
	int * i = (int *)data->buff;
	return printf("data = %d\n",*i);
}
/* media */
int logger_close_media(struct log_data * data, void * storage_data)
{
	free(data->buff);
	printf("freeing buffer\n");
	return 0;
}
int main(int argc, char * argv[])
{
	struct logger * logger;
	int	i;
	
	init_logger();
	logger = logger_alloc();
	if(!logger){
		printf("alloca failed\n");
                goto err;
	}

	/* sampling rate */
        logger->log.sampling_rate.sample_interval.m_sec = 200;
	/* number of samples */
	logger->log.sampling_rate.nr_samples = 20;
	/* data related */
	logger->log.data.init_capture = logger_init_capture;
	logger->log.data.get_data = logger_get_data;
 
	/* media related */
	logger->log.storage_media.init_media  = logger_init_media;   
	logger->log.storage_media.save_data   = logger_save_data;   
	logger->log.storage_media.close_media = logger_close_media;   
	
	printf("logger_register() = %d\n",register_logger(logger));
	for(i=0; i<10; i++){
		logger_task();
		printf("nr_samples = %d\n",logger->log.sampling_rate.nr_samples);
	}
	printf("logger_unregister() = %d\n",unregister_logger(logger));
	exit(EXIT_SUCCESS);
err:
        exit(EXIT_FAILURE);
}


