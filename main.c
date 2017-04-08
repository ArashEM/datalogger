/*
*	logger test
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/sysinfo.h>

#include "log.h"
#include "list.h"


/* init_capture() */
int logger_init_capture(struct log_data * data)
{
       	data->buff = (char *)malloc(sizeof(struct sysinfo));
       	if(!data->buff)
		return -1;
	data->data_len = sizeof(struct sysinfo);
	memset((void *)data->buff, 0, data->data_len);
	return 0;
}

/* get_data() */
int logger_get_data(struct log_data * data)
{
	struct sysinfo * info = (struct sysinfo *)data->buff;
	if(!sysinfo(info))
		return data->data_len;	
	else	
		return 0;
}

/* media */
int logger_init_media(void * data)
{
	return 0;
}
/* media */
int logger_save_data(struct log_data * data, void * storage_data)
{
	struct sysinfo * info = (struct sysinfo *)data->buff;
	return printf("uptime = %ld\n", info->uptime);
}
/* media */
int logger_close_media(struct log_data * data, void * storage_data)
{
	if(!data->buff);
		free(data->buff);
	data->buff = NULL;
	printf("freeing buffer\n");
	return 0;
}
/* statistics */
int logger_get_stat(struct logger_stat * stat)
{
	printf("get_fails = %d\n", stat->nr_get_fails);
	printf("save_fails = %d\n", stat->nr_save_fails);
	return 1;
}


/* main */
int main(int argc, char * argv[])
{
	struct logger * logger;
	struct timeval  tv;
	
	init_logger();
	logger = logger_alloc();
	if(!logger){
		printf("alloca failed\n");
                goto err;
	}

	/* sampling rate */
        logger->log.sampling_rate.sample_interval.sec = 1;
	/* number of samples */
	logger->log.sampling_rate.nr_samples = 5;
	/* data related */
	logger->log.data.init_capture = logger_init_capture;
	logger->log.data.get_data = logger_get_data;
 
	/* media related */
	logger->log.storage_media.init_media  = logger_init_media;   
	logger->log.storage_media.save_data   = logger_save_data;   
	logger->log.storage_media.close_media = logger_close_media;   
	/* stat */
	logger->stat.get_stat = logger_get_stat;

	printf("logger_register() = %d\n",register_logger(logger));

	/* delay for 100ms (10Hz) */
	tv.tv_sec  = 0;
	tv.tv_usec = 100000;

	while(nr_running_logger() > 0){
		select(0, NULL, NULL, NULL, &tv);
		tv.tv_sec  = 0;
		tv.tv_usec = 100000;

		logger_task();
	}
	printf("logger_zombie_task()= %d\n",logger_zombie_task());
	printf("logger_unregister() = %d\n",unregister_logger(logger));
	exit(EXIT_SUCCESS);
err:
        exit(EXIT_FAILURE);
}

/*
	printf("free=%d, running=%d, zombie=%d\n",nr_free_logger(), nr_running_logger(), nr_zombie_logger());
	printf("free=%d, running=%d, zombie=%d\n",nr_free_logger(), nr_running_logger(), nr_zombie_logger());
*/
