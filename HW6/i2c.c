#include<xc.h>           // processor SFR definitions
#include "i2c.h" 


void i2c_master_setup(void) {
  ANSELB = 0;
  I2C2BRG = BRG_VAL;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
  
                                    // look up PGD for your PIC32 
  I2C2CONbits.ON = 1;               // turn on the I2C2 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    long t = 800000; 
    unsigned char i=0;
    unsigned char b = 0 ;
    while(!I2C2STATbits.RBF) { 
        if (_CP0_GET_COUNT()>t)
        {
            if (i)
                i=0;
            else
                i=1;
            LATAbits.LATA4 = i;
            _CP0_SET_COUNT(0);
        }
    }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}

void I2C_read_multiple(char address, char reg, unsigned char *data, char length)
{
    unsigned char i=0;
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send(address << 1 | 0); //write
    i2c_master_send(reg); 
    i2c_master_restart();
    i2c_master_send(address << 1 | 1); //read
    
    while (i<length)
    {
        *data= i2c_master_recv();
        data++;
        i++;
        if (i<length)
            i2c_master_ack(0);
    }
    i2c_master_ack(1);
    i2c_master_stop();
}