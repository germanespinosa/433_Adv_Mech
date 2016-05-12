#include<xc.h>           // processor SFR definitions
#include "../timer/timer.h"

#define SPI_TIMEOUT     480000 // one millisecond

#define CS LATAbits.LATA0       // chip select pin

unsigned char SPI_io(unsigned char o);
unsigned short SPI_io_short(unsigned short o);
void SPI_init();
