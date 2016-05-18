#include "spi.h"

//returns 1 fail - 0 success
unsigned spi_write_byte(unsigned char *o)
{
    SPI1BUF = *o;
    unsigned int t = timer_start();
    while(!SPI1STATbits.SPIRBF && !timer_timeout(t,SPI_TIMEOUT)) { // wait to receive the byte
      ;
    }
    *o = SPI1BUF;
    return timer_timeout(t,SPI_TIMEOUT);
}

// write len bytes to the ram, starting at the address addr
unsigned short spi_write(unsigned char *o, unsigned char len) 
{
    int i=0;
    for (i=0;i<len;i++)
    {
        if (spi_write_byte(&o[i]))
            break;
    }
    return i;
}

// initialize spi1 
unsigned spi_init()
{
    static unsigned char started = 0;
    if (!started)
    {
        RPB15Rbits.RPB15R = 0b0101;  // assigns clock to pin 15 
        RPA1Rbits.RPA1R = 0b0011;    // assigns  SDO1 to pin A1
        SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
        SPI1CONbits.MSTEN = 1;    // master operation
        SPI1CONbits.ON = 1;       // turn on spi 1
        started = 1;
    }
    return started;
}


// write len bytes to the ram, starting at the address addr
unsigned spi_io_short(unsigned short *o) 
{
    unsigned char buffer[2] = {(*o & 0xFF00) >> 8, *o};
    unsigned r = spi_write(buffer,2);
    *o= (buffer[0] << 8) + buffer[1];
    return r;
}
