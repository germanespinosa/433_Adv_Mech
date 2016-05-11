#include "i2c.h"
#include<xc.h>           // processor SFR definitions



void i2c_master_setup(void) {
    ANSELB = 0;
    I2C2BRG = BRG_VAL;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
                                      // look up PGD for your PIC32 
    I2C2CONbits.ON = 1;               // turn on the I2C2 module
}

// Start a transmission on the I2C bus
unsigned i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    _CP0_SET_COUNT(0);
    while(I2C2CONbits.SEN && _CP0_GET_COUNT()<TIMEOUT)  {  }    // wait for the start bit to be sent
    return I2C2CONbits.SEN;
}

unsigned i2c_master_restart(void) 
{     
    I2C2CONbits.RSEN = 1;           // send a restart 
    _CP0_SET_COUNT(0);
    while(I2C2CONbits.RSEN && _CP0_GET_COUNT()<TIMEOUT) {  }
    return I2C2CONbits.RSEN;
}

unsigned i2c_master_send(unsigned char byte) { // send a byte to slave
    I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
    _CP0_SET_COUNT(0);
    while(I2C2STATbits.TRSTAT && _CP0_GET_COUNT()<TIMEOUT) { ; }  // wait for the transmission to finish
    return I2C2STATbits.ACKSTAT;  // if this is high, slave has not acknowledged
                                  // ("I2C2 Master: failed to receive ACK\r\n");
}

unsigned i2c_master_recv(unsigned char *data) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    _CP0_SET_COUNT(0);
    while(!I2C2STATbits.RBF && _CP0_GET_COUNT()<TIMEOUT) { }    // wait to receive the data
    I2C2CONbits.RCEN = 0;             // reset data rcv
    *data = (unsigned char) I2C2RCV;  // read and return the data
    return I2C2STATbits.RBF;
}

unsigned i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    _CP0_SET_COUNT(0);
    while(I2C2CONbits.ACKEN && _CP0_GET_COUNT()<TIMEOUT) { ; }    // wait for ACK/NACK to be sent
    return I2C2CONbits.ACKEN;
}

unsigned i2c_master_stop(void) {          // send a STOP:
    I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
    _CP0_SET_COUNT(0);
    while(I2C2CONbits.PEN && _CP0_GET_COUNT()<TIMEOUT) { ; }        // wait for STOP to complete
    return I2C2CONbits.PEN ;
}

unsigned I2C_read_multiple(char address, char reg, unsigned char *data, unsigned length)
{
    unsigned char i=0;
    unsigned char retry = 0;
    i2c_master_start(); // Begin the start sequence
    i2c_master_send(address << 1 | 0); //write
    i2c_master_send(reg); 
    i2c_master_restart();
    i2c_master_send(address << 1 | 1); //read
    while (i<length)
    {
        if (i2c_master_recv(data))
        {
            data++;
            i++;
        } else
        {
            retry++;
            if (retry==3)
            {
                break;
            }
        }
        if (i<length)
            i2c_master_ack(0);
    }
    i2c_master_ack(1);
    i2c_master_stop();
    return i;
}

