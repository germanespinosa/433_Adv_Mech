#define _SUPPRESS_PLIB_WARNING 
#define _DISABLE_OPENADC10_CONFIGPORT_WARNING

#include <plib.h>
#include<xc.h>           // processor SFR definitions

#define CS LATAbits.LATA0       // chip select pin

unsigned char SPI_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

// write len bytes to the ram, starting at the address addr
unsigned short SPI_io_short(unsigned short o) {
  CS = 0;                   // start the command
  unsigned short b = SPI_io((o & 0xFF00) >> 8 ); // most significant byte of address
  b =  (b << 8) + SPI_io(o & 0x00FF);          // the least significant address byte
  CS = 1;                   // finish the command
  return b;
}

// initialize spi1 
void SPI_init() {
  TRISAbits.TRISA0 = 0; // sets A0 for output
  RPB15Rbits.RPB15R = 0b0101;  // assigns clock to pin 15 
  RPA1Rbits.RPA1R = 0b0011;    // assigns  SDO1 to pin A1
  CS = 1;
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi 1
}