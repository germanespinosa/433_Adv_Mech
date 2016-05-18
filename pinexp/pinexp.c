#include "pinexp.h"

//returns 1 success - 0 error
unsigned pinexp_set_reg(unsigned char reg, unsigned char value, unsigned char addr)
{
    return i2c_write(PINEX_BASE_ADDRESS+addr,reg, &value,1);
}
// returns 1 success - 0 error
unsigned pinexp_start(unsigned char direction/* 1 input - 0 output */, unsigned char addr)
{
    i2c_setup();
    return i2c_write(PINEX_BASE_ADDRESS+addr,PINEX_REG_IODIR, &direction, 1) && i2c_write(PINEX_BASE_ADDRESS+addr,PINEX_REG_GPPU, &direction, 1);    // pull up resistor
}
// returns 1 success - 0 error
unsigned pinexp_write(unsigned char value,unsigned char addr)
{
     return i2c_write(PINEX_BASE_ADDRESS+addr,PINEX_REG_GPIO, &value,1);
}
// returns 1 success - 0 error
unsigned char pinexp_read(unsigned char addr)
{
    unsigned char d=0;
    i2c_read(PINEX_BASE_ADDRESS+addr, PINEX_REG_GPIO, &d, 1);
    return d;
}