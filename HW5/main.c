#include "HW5.h"


void blink ()
{
    while(1)
    {
        LATAbits.LATA4 = 0;
        unsigned int t = timer_start();
        while (!timer_timeout(t, 10000000));
        LATAbits.LATA4 = 1;
        t = timer_start();
        while (!timer_timeout(t, 10000000));
    }   
}


int main(int argc, char** argv) {
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
     // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; // A4 output
    TRISBbits.TRISB4 = 1; // B4 inuput
    ANSELA = 0; // A Analogic off
    ANSELB = 0; // B Analogic off
    
    __builtin_disable_interrupts();

    // starts the PINEX
    
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
    __builtin_enable_interrupts();
    lcd_start();
    while(1)
    {
        LATAbits.LATA4 = 0;
        lcd_clearScreen(LCD_COLOR_BLACK);
        lcd_printf("hello\nworld!",LCD_COLOR_WHITE);
        //unsigned int t = timer_start();
        //while (!timer_timeout(t, 500 * TIMER_MILLISECOND));
        blink();
        LATAbits.LATA4 = 1;
        lcd_clearScreen(LCD_COLOR_WHITE);
        lcd_printf("hello!",LCD_COLOR_BLACK);
        //t = timer_start();
        //while (!timer_timeout(t, 500 * TIMER_MILLISECOND));
    }   
    return (EXIT_SUCCESS);
}
