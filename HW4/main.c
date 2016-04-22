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


#define SLAVE_ADDR 0b0100000
#define SLAVE_REGISTER 0x09


// send a byte via spi and return the response

void set_voltage (unsigned short channel, unsigned char voltage)
{
    unsigned short o= 0b0011000000000000;
    //unsigned short o2= 0b0011111111110000;
    o += (channel && 1) << 15;
    o += voltage << 4;
    SPI_io_short(o);
}

int main(int argc, char** argv) {
    __builtin_disable_interrupts();
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
     // do your TRIS and LAT commands here

    unsigned char master_write0 = 0xCD;       // first byte that master writes
    unsigned char master_write1 = 0x91;       // second byte that master writes
    unsigned char master_read0  = 0x00;       // first received byte
    unsigned char master_read1  = 0x00;       // second received byte
  
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    
    __builtin_enable_interrupts();
    SPI_init();
    I2C_init();
    unsigned char i=0;
    while(1) {

        I2C_start();                     // Begin the start sequence
        I2C_send(SLAVE_ADDR << 1);       // send the slave address, left shifted by 1, 
                                         // which clears bit 0, indicating a write

        I2C_send(SLAVE_REGISTER);       // send the slave address, left shifted by 1, 
                                        // which clears bit 0, indicating a write
        i++;
        float r = (sin( (float)i /255 * 6.28 ) * 128) + 127;
        if (PORTBbits.RB4)
        {
            set_voltage(1,i);
            set_voltage(0,(unsigned char) r);

            I2C_send(0b11111111);           // send the slave address, left shifted by 1, 
                                                // which clears bit 0, indicating a write
        }
        else
        {
            set_voltage(0,i);
            set_voltage(1,(unsigned char) r);

            I2C_send(0b00000000);           // send the slave address, left shifted by 1, 
                                                // which clears bit 0, indicating a write
        }
        
        I2C_stop();                   // send a RESTART so we can begin reading 
    }
    return (EXIT_SUCCESS);
}