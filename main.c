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
	init_logger();
	printf("sizeof(logger) = %zd\n",sizeof(struct logger));
	misc_test(NULL);
	exit(EXIT_SUCCESS);
}
