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
	printf("size of tick = %zd\n",sizeof(struct logger));
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
