/* 
 * File:   main.c
 * Author: German
 *
 * Created on April 5, 2016, 2:37 PM
 */ 

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#define _SUPPRESS_PLIB_WARNING 
#define _DISABLE_OPENADC10_CONFIGPORT_WARNING
#include <plib.h>
#include <math.h>

//#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))

// DEVCFG0
#pragma config DEBUG = 0 // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = FRCPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on
/*
*/
// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

/*
 * 
 */

#define CS LATAbits.LATA0       // chip select pin

#define NU32USER PORTDbits.RD13
#define SYS_FREQ 80000000           // 80 million Hz

#define DESIRED_BAUDRATE_NU32 230400 // Baudrate for RS232

// send a byte via spi and return the response


unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

// write len bytes to the ram, starting at the address addr
unsigned short spi_io_short(unsigned short o) {
  unsigned short b = spi_io((o & 0xFF00) >> 8 ); // most significant byte of address
  return (b << 8) + spi_io(o & 0x00FF);          // the least significant address byte
}

// initialize spi1 
void init_spi() {
  // set up the chip select pin as an output
  // when a command is beginning (clear CS to low) and when it
  // is ending (set CS high)
  TRISAbits.TRISA0 = 0; // sets A0 for output
  RPB15Rbits.RPB15R = 0b0101;  // assigns clock to pin 15 
  RPA1Rbits.RPA1R = 0b0011;    // assigns  SDO1 to pin A1
  CS = 1;

  // Master - SPI4, pins are: SDI4(F4), SDO4(F5), SCK4(F13).  
  // we manually control SS4 as a digital output (F12)
  // since the pic is just starting, we know that spi is off. We rely on defaults here
  
  // setup spi4
//  SPI1CON = 0;              // turn off the spi module and reset it
//  SPI1BUF = 0;              // clear the rx buffer by reading from it

//  SPI1CON = 0x3;            // turn off the spi module and reset it
//  SPI1BRG = 0x3;            // baud rate to 10 MHz [SPI4BRG = (80000000/(2*desired))-1]
//  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi 1

  //SPI1CON = 0;              // turn off the spi module and reset it
}

void set_voltage (unsigned short channel, unsigned char voltage)
{
    CS = 0;                   // finish the command
    unsigned short o0= 0b0011000000000000;
    //unsigned short o2= 0b0011111111110000;
    channel = (channel && 1) << 15;
    /*    unsigned short o0; //= 0b0011000000000000;
    o0 = (channel && 1) << 15;
    o0 += 0 << 14;
    o0 += 1 << 13;
    o0 += 1 << 12;*/
    o0 += voltage << 4;
    spi_io_short(channel + o0);
    CS = 1;                   // finish the command
}

int main(int argc, char** argv) {
    __builtin_disable_interrupts();
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
     // do your TRIS and LAT commands here
    
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    
    __builtin_enable_interrupts();
    //NU32_Startup();
    init_spi();
    unsigned char i=0;
    while(1) {
        i++;
        float r = (sin( (float)i /255 * 6.28 ) * 128) + 127;
        if (PORTBbits.RB4)
        {
            set_voltage(1,i);
            set_voltage(0,(unsigned char) r);
        }
        else
        {
            set_voltage(0,i);
            set_voltage(1,(unsigned char) r);
        }
        
    }
    CS = 0;
    return (EXIT_SUCCESS);
}