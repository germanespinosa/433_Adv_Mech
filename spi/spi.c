#include "spi.h"

//returns 1 success - 0 fail
unsigned spi_send(unsigned char *o) 
{
  SPI1BUF = *o;
  unsigned int t = timer_start();
  while(!SPI1STATbits.SPIRBF && !timer_timeout(t,SPI_TIMEOUT)) { // wait to receive the byte
    ;
  }
  *o=SPI1BUF;
  return SPI1STATbits.SPIRBF;
}

unsigned spi_write(unsigned char *data, int lenght)
{
    int i = 0;
    for (i=0;i<lenght;i++)
    {
        spi_send(data);
        //if (!spi_send(data))
          //  break;
        data ++;
    }
    return i;
    
}

// initialize spi1 
void SPI_init() {
  TRISAbits.TRISA0 = 0; // sets A0 for output
  RPB15Rbits.RPB15R = 0b0101;  // assigns clock to pin 15 
  RPA1Rbits.RPA1R = 0b0011;    // assigns SDO1 to pin A1
  CS = 1;
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi 1
}