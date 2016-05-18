#include "HW4.h"

#define PINEX_SLAVE_ADDR1 0b001 // device addr 0100+ A0A1A2 001
#define PINEX_SLAVE_ADDR2 0b100 // device addr 0100+ A0A1A2 100

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

    // starts the DAC
    dac_init();
    pinexp_start(0b00000000,PINEX_SLAVE_ADDR1);
    pinexp_start(0b11111111,PINEX_SLAVE_ADDR2);
    _CP0_SET_COUNT(0);

    unsigned int ti2c = timer_start();
  
    unsigned char i=0;
    unsigned char i2=0;
    unsigned char l=0;
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
        if (timer_timeout(ti2c,10000000))
        {
            unsigned char b = pinexp_read(PINEX_SLAVE_ADDR2);
            LATAbits.LATA4 = b & l ;

            i2=i2<<1;
            if (!i2) i2=1;
            if (l)
                l=0;
            else
                l=1;
            if (b & 1) 
                pinexp_write(255,PINEX_SLAVE_ADDR1);
            else
                pinexp_write(0,PINEX_SLAVE_ADDR1);
            _CP0_SET_COUNT(0);
            ti2c = timer_start();
        }
        unsigned int t = timer_start();
        while (!timer_timeout(t,24000)); 
    }
    return (EXIT_SUCCESS);
}
