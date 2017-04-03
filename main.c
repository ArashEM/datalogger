/*
*	logger test
*
*/
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "list.h"

int main(int argc, char * argv[])
{
	tick_t tick;
	struct samplespec sm;
	sm.sample_interval.m_sec = 700;
        sm.sample_interval.sec   = 0 ;

	tick = __calculate_tick(&sm);
	printf("tick = %ld\n",(unsigned long int)tick);

	exit(EXIT_SUCCESS);
}


/*
        struct logger * tmp;
	init_logger();
	printf("sizeof(logger) = %zd\n",sizeof(struct logger));
	misc_test(NULL);
        printf("----------------------------------------------\n");
        tmp = logger_alloc();
        misc_test(NULL);
        printf("----------------------------------------------\n");
        logger_free(tmp);
        misc_test(NULL);
*/
