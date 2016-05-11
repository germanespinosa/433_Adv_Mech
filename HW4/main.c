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

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module


#define PINEX_SLAVE_ADDR1 0b001 // device addr 0100+ A0A1A2 001
#define PINEX_SLAVE_ADDR2 0b100 // device addr 0100+ A0A1A2 100

void set_voltage (unsigned short channel, unsigned char voltage)
{
    unsigned short o= 0b0011000000000000;
    //unsigned short o2= 0b0011111111110000;
    o += (channel && 1) << 15;
    o += voltage << 4;
    SPI_io_short(o);
}

unsigned char i2cData[4] = {0x98,0x01,0xAA};

int main(int argc, char** argv) {
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
     // do your TRIS and LAT commands here

    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    
    __builtin_disable_interrupts();
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
    __builtin_enable_interrupts();
    unsigned char i=0;
    LATAbits.LATA4 = 0;

    pinexp_start(0b00000000/*all out*/,PINEX_SLAVE_ADDR1);
    pinexp_start(0b00001111/*all in*/,PINEX_SLAVE_ADDR2);
    
    unsigned char o=0;

    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT()<24000000){;}
    o=pinexp_read(PINEX_SLAVE_ADDR2);
    pinexp_write(0b11111111,PINEX_SLAVE_ADDR1);

    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT()<24000000){;}
    pinexp_write(0b00000000,PINEX_SLAVE_ADDR1);

    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT()<24000000){;}
    pinexp_write(o,PINEX_SLAVE_ADDR1);
    
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT()<24000000){;}

    unsigned short b = 0 ;
    long t = 1000000; 
    
    
    while (PORTBbits.RB4)
    {
        if (_CP0_GET_COUNT()>t)
        {
            if (b)
                b = b << 1;
            else
                b=1;
            
            unsigned char o;
            o=pinexp_read(PINEX_SLAVE_ADDR2);

            //pinexp_write(b,PINEX_SLAVE_ADDR2);

            pinexp_write(o<<4,PINEX_SLAVE_ADDR2);

            pinexp_write(o,PINEX_SLAVE_ADDR2);

            if ((o & 1) || !b)
            {
                LATAbits.LATA4 = 1;
            }
            else
            {
                LATAbits.LATA4 = 0;
            }
            _CP0_SET_COUNT(0);
        }
    }
/*
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
        LATAbits.LATA4 = 1;
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()<8000000); 
        LATAbits.LATA4 = 0;
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()<8000000); 
    }*/
    return (EXIT_SUCCESS);
}
