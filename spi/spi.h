#include<xc.h>           // processor SFR definitions
#include "../timer/timer.h"


#define SPI_TIMEOUT 280000 


unsigned spi_write_byte(unsigned char *o);
unsigned short spi_write(unsigned char *o, unsigned char len);
unsigned spi_init();
