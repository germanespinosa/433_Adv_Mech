#include<xc.h>           // processor SFR definitions

#define TIMER_DONE 1
#define TIMER_PENDING 0

#define TIMER_MILLISECOND 24000

unsigned int timer_start();
unsigned char timer_timeout(unsigned int ts, unsigned int to);
