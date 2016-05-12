#include"i2c.h"

void i2c_setup(void) 
{
    static unsigned char started = 0;
    if (!started)
    {
        ANSELB = 0;
        I2C2BRG = I2C_BRG_VAL;
        I2C2CONbits.ON = 1;
        started = 1;
    }
}
//returns 1 fail - 0 success
unsigned i2c_start(void) 
{
    I2C2CONbits.SEN = 1;
    register unsigned int t = timer_start();
    while(I2C2CONbits.SEN && !timer_timeout(t,I2C_TIMEOUT)) { ; } 
    return I2C2CONbits.SEN;
}

//returns 1 fail - 0 success
unsigned i2c_restart(void) 
{
    I2C2CONbits.RSEN = 1;
    register unsigned int t = timer_start();
    while(I2C2CONbits.RSEN && !timer_timeout(t,I2C_TIMEOUT)) { ; }
    return I2C2CONbits.RSEN;
}

//returns 1 fail - 0 success
unsigned i2c_send(unsigned char byte) 
{
    I2C2TRN = byte;
    register unsigned int t = timer_start();
    while(I2C2STATbits.TRSTAT && !timer_timeout(t,I2C_TIMEOUT)) { ; } 
    return I2C2STATbits.ACKSTAT;
}

//returns 1 fail - 0 success
unsigned i2c_recv(unsigned char *buffer) 
{ 
    I2C2CONbits.RCEN = 1;
    register unsigned int t = timer_start();
    while(!I2C2STATbits.RBF && !timer_timeout(t,I2C_TIMEOUT)) { ; }
    *buffer= I2C2RCV; // copy the data to the buffer
    return !I2C2STATbits.RBF;
}

//returns 1 fail - 0 success
unsigned i2c_ack(int ack/* 0 ACK - 1 NACK */) 
{
    I2C2CONbits.ACKDT = ack;
    I2C2CONbits.ACKEN = 1;
    register unsigned int t = timer_start();
    while(I2C2CONbits.ACKEN && !timer_timeout(t,I2C_TIMEOUT)) { ; }
    return I2C2CONbits.ACKEN;
}

//returns 1 fail - 0 success
unsigned i2c_stop(void) 
{
    I2C2CONbits.PEN = 1;
    register unsigned int t = timer_start();
    while(I2C2CONbits.PEN && !timer_timeout(t,I2C_TIMEOUT)) { ; }
    return I2C2CONbits.PEN;
}

// returns the number of bytes successfully written 
unsigned i2c_write(unsigned char address, unsigned char reg, unsigned char *data, char length)
{
    int i=0;
    for (i=0; i<length; i++)
    {
        if (i2c_start() || i2c_send(address<<1) || i2c_send(reg) || i2c_send(*(data++)) || i2c_stop())
        {
            break;
        }
    }
    return i;
}

// returns the number of bytes successfully read
unsigned i2c_read(char address, char reg, unsigned char *data, char length)
{
    unsigned char i=0;
    if (i2c_start() ||  i2c_send(address << 1 | 0) || i2c_send(reg) || i2c_restart() || i2c_send(address << 1 | 1))
    {
        return I2C_ERROR;
    }
    while (i++<length)
    {
        if (i2c_recv(data) || i2c_ack(i==length))
        {
            break;
        }
        data++;
    }
    if (i2c_stop())
        return I2C_ERROR;
    else
        return i-1;
}