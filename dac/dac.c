#include "dac.h"

unsigned dac_init()
{
    DAC_CS_TRIS = 0; // sets A0 for output
    DAC_CS = 1;
    return spi_init();
}

// write len bytes to the ram, starting at the address addr
unsigned short spi_io_short(unsigned short o) 
{
    unsigned char buffer[2] = {(o & 0xFF00) >> 8, o};
    return spi_write(buffer,2);
}

unsigned set_voltage (unsigned short channel, unsigned char voltage)
{
    unsigned char buffer[2] = {
        0b00110000 + ((channel && 1) << 7 ) + (voltage >> 4) , // byte 0
        (voltage << 4)                                         // byte 1
    };
    DAC_CS = 0;                   // start the command
    //spi_io_short(o);
    spi_write(buffer,2);
    DAC_CS = 1;                   // finish the command
}
