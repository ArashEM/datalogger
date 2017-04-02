#ifndef CONFIG_H
#define CONFIG_H

/*
*  maximum nummber of logger in system
*  (each logger is about 150 byte)
*/
#define NR_LOGGER   20


/**
* rate of calling logger_task()
* (10Hz to 100Hz)
*/
#define TICK_RATE_HZ   10




#endif
