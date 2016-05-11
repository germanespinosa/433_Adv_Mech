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
#include "imu.h"

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


#define PINEX_SLAVE_ADDR_WRITE 0b01000010 // device addr 0100+ A0A1A2 100+ 0 Write 

int main(int argc, char** argv) {
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
     // do your TRIS and LAT commands here

    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    
    //SPI_init();
    //I2C_init();
    __builtin_disable_interrupts();
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
    unsigned char o=0;
    start_imu();
    if (0)//error
    {
        while(1)
        {
            LATAbits.LATA4=0;
            _CP0_SET_COUNT(0);
            while (_CP0_GET_COUNT()<100000000){;}
            LATAbits.LATA4=1;
            _CP0_SET_COUNT(0);
            while (_CP0_GET_COUNT()<100000000){;}
        }            
    }else
    {
        o = 55;//imu_whoami();
        if (o==0)
        {
            while(1)
            {
                LATAbits.LATA4=0;
                _CP0_SET_COUNT(0);
                while (_CP0_GET_COUNT()<1000000){;}
                LATAbits.LATA4=1;
                _CP0_SET_COUNT(0);
                while (_CP0_GET_COUNT()<1000000){;}
            }            
        }
    }
    __builtin_enable_interrupts();
    unsigned char i=0;
    LATAbits.LATA4 = 0;
    
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send(PINEX_SLAVE_ADDR_WRITE);
    i2c_master_send(0x00); // OLAT
    i2c_master_send(0b00000000); 
    i2c_master_stop();
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT()<1000000){;}
    
    
    short x,y,z;
    short x_e,y_e,z_e;
    readAcc(&x_e,&y_e,&z_e);
    int x_f=0;
    unsigned char n;
    while (1)
    {
        i2c_master_start();                     // Begin the start sequence
        i2c_master_send(PINEX_SLAVE_ADDR_WRITE);
        i2c_master_send(0x0A); // OLAT
        i2c_master_send(o); 
        i2c_master_stop();
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()<1000000){;}
        n=3;
        readAcc(&x,&y,&z);
        x_f=x-x_e;
        unsigned char dif=0;
        if (abs(x_f)>400)
            dif++;
        if (abs(x_f)>600)
            dif++;
        if (abs(x_f)>800)
            dif++;
        if (x_f>0)
            n+=dif;
        else
            n-=dif;
        o=3 << n;
    }    
    return (EXIT_SUCCESS);
}



/*
int main(int argc, char** argv) {
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
     // do your TRIS and LAT commands here

    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    
    //SPI_init();
    //I2C_init();
    __builtin_disable_interrupts();
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
    i2c_master_setup();                       // init I2C2, which we use as a master
    __builtin_enable_interrupts();
    unsigned char i=0;
    _CP0_SET_COUNT(0);
    LATAbits.LATA4 = 0;

    unsigned char b = 0 ;
    unsigned char o = 55;
    

    I2C_read_multiple(IMU_SLAVE_ADDR, WHOAMI,  &o, 1);

    i2c_master_start();  // Begin the start sequence
    i2c_master_send(IMU_SLAVE_ADDR_WRITE);
    i2c_master_send(WHOAMI); //WHOAMI
    i2c_master_restart(); 
    i2c_master_send(IMU_SLAVE_ADDR_READ);
    o=i2c_master_recv(); 
    i2c_master_ack(1);
    i2c_master_stop();

    //I2C_read_multiple(IMU_SLAVE_ADDR_READ, 0x0F, &o, 1);
    i2c_master_start();  // Begin the start sequence
    i2c_master_send(PINEX_SLAVE_ADDR_WRITE);
    i2c_master_send(0x0A); // OLAT
    i2c_master_send(o); 
    i2c_master_stop();                
    while (PORTBbits.RB4)
    {
        ;
    }

    short x = 0 , y = 0 , z = 0;
    
    while (!PORTBbits.RB4)
    {
        ;
    }
    while (1)
    {
        
        if (_CP0_GET_COUNT()>4000000)
        {
            if (i)
            {
                //readAcc(&x,&y,&z);
                o=x;
                i2c_master_start();  // Begin the start sequence
                i2c_master_send(PINEX_SLAVE_ADDR_WRITE);
                i2c_master_send(0x0A); // OLAT
                i2c_master_send(0b00001111); 
                i2c_master_stop();  
                i=0;
            }
            else
            {
                i2c_master_start();  // Begin the start sequence
                i2c_master_send(PINEX_SLAVE_ADDR_WRITE);
                i2c_master_send(0x0A); // OLAT
                i2c_master_send(0b11110000); 
                i2c_master_stop();  
                i=1;
            }
            LATAbits.LATA4 = i;
            _CP0_SET_COUNT(0);


        }
    }

    return (EXIT_SUCCESS);
}
*/