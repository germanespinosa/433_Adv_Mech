/* 
 * File:   main.c
 * Author: German
 *
 * Created on April 5, 2016, 2:37 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

/*
 * 
 */
int main(int argc, char** argv) {
    __builtin_disable_interrupts();
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
    __builtin_enable_interrupts();
    TRISBbits.TRISB4=1;
    while(1) {
        TRISBbits.TRISB4=1;
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		// remember the core timer runs at half the CPU speed
    }

    return (EXIT_SUCCESS);
}

