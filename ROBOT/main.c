#include "ROBOT.h"

void PWM1_Duty(int Pwm)    
{
    
    OC1RS = Pwm; // Write new duty cycle
    OC2RS = Pwm; // Write new duty cycle
}

void blink ()
{
    unsigned int t;
    PWM1_Duty(24000);
    while(1)
    {
        LATAbits.LATA4 = 0;
        LATAbits.LATA0 = 1;
        LATBbits.LATB2 = 1;
        t = timer_start();
        while (!timer_timeout(t, 10000000));
        LATAbits.LATA4 = 1;
        LATAbits.LATA0 = 0;
        LATBbits.LATB2 = 0;
        t = timer_start();
        while (!timer_timeout(t, 10000000));
    }   
}

void start_PWM()
{
    RPB7Rbits.RPB7R = 0b0101;
    RPB8Rbits.RPB8R = 0b0101;
    OC1CON = 0x0000; // Turn off OC1 while doing setup.
    OC1R = 0x0000; // Initialize primary Compare Register
    OC1RS = 0x0000; // Initialize secondary Compare Register
    OC1CON = 0x0006; // Configure for PWM mode


    OC2CON = 0x0000; // Turn off OC3 while doing setup.
    OC2R = 0x0000; // Initialize primary Compare Register
    OC2RS = 0x0000; // Initialize secondary Compare Register
    OC2CON = 0x0006; // Configure for PWM mode

    PR2 = 2000; // Set period 24000

    T2CONSET = 0x8000; // Enable Timer2
    OC1CONSET = 0x8000; // Enable OC1
    OC2CONSET = 0x8000; // Enable OC3
}

int main(int argc, char** argv) {
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
     // do your TRIS and LAT commands here
    ANSELA = 0; // A Analogic off
    ANSELB = 0; // B Analogic off

    TRISBbits.TRISB2 = 0; // A4 output
    TRISAbits.TRISA0 = 0; // A4 output

    TRISAbits.TRISA4 = 0; // A4 output
    TRISBbits.TRISB4 = 1; // B4 inuput
    
    __builtin_disable_interrupts();
    
    // starts the PINEX
    
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
    __builtin_enable_interrupts();
    uart_init();
    return 0;
    
    start_PWM();
    
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
