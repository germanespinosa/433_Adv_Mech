#include "../spi/spi.h"

#define DAC_CS LATAbits.LATA0       // chip select pin

#define DAC_CS_TRIS TRISAbits.TRISA0 

unsigned dac_init();
unsigned set_voltage (unsigned short channel, unsigned char voltage);
