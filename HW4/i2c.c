#include<xc.h>           // processor SFR definitions
// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C2 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k

#define SYS_FREQ    (24000000)
#define PBCLK       (SYS_FREQ/2)
#define Fsck        26000
#define BRG_VAL     (PBCLK/2/Fsck)

#define I2C_TIMEOUT 1000000

void i2c_setup(void) 
{
    static unsigned char started = 0;
    if (!started)
    {
        ANSELB = 0;
        I2C2BRG = BRG_VAL;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
                                        // look up PGD for your PIC32 
        I2C2CONbits.ON = 1;               // turn on the I2C2 module
        started = 1;
    }
}

// Start a transmission on the I2C bus
//returns != 0 when error
unsigned i2c_start(void) 
{
    I2C2CONbits.SEN = 1;            // send the start bit
    _CP0_SET_COUNT(0);
    while(I2C2CONbits.SEN && _CP0_GET_COUNT()<I2C_TIMEOUT) { ; }    // wait for the start bit to be sent
    return I2C2CONbits.SEN;
}

unsigned i2c_restart(void) 
{
    I2C2CONbits.RSEN = 1;           // send a restart 
    _CP0_SET_COUNT(0);
    while(I2C2CONbits.RSEN && _CP0_GET_COUNT()<I2C_TIMEOUT) { ; }   // wait for the restart to clear
    return I2C2CONbits.RSEN;
}

//returns != 0 when error
unsigned i2c_send(unsigned char byte) 
{ // send a byte to slave
    I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
    _CP0_SET_COUNT(0);
    while(I2C2STATbits.TRSTAT && _CP0_GET_COUNT()<I2C_TIMEOUT) { ; }  // wait for the transmission to finish
    return I2C2STATbits.ACKSTAT; // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
}

unsigned i2c_recv(unsigned char *buffer) 
{ // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    _CP0_SET_COUNT(0);
    while(!I2C2STATbits.RBF && _CP0_GET_COUNT()<I2C_TIMEOUT) { ; }    // wait to receive the data
    *buffer= I2C2RCV;                   // read and return the data
    return I2C2STATbits.RBF;
}

unsigned i2c_ack(int val) 
{        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    _CP0_SET_COUNT(0);
    while(I2C2CONbits.ACKEN && _CP0_GET_COUNT()<I2C_TIMEOUT) { ; }    // wait for ACK/NACK to be sent
    return I2C2CONbits.ACKEN;
}

unsigned i2c_stop(void) 
{          // send a STOP:
    I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
    _CP0_SET_COUNT(0);
    while(I2C2CONbits.PEN && _CP0_GET_COUNT()<I2C_TIMEOUT) { ; }        // wait for STOP to complete
    return I2C2CONbits.PEN;
}

unsigned i2c_write(unsigned char address, unsigned char reg, unsigned char *data, char length)
{
    int i=0;
    for (i=0; i<length; i++)
    {
        if (!i2c_start())
        {
            i2c_send(address<<1);
            i2c_send(reg);
            i2c_send(*(data++));
            i2c_stop();
        }
        else
        {
            return i;
        }
    }
}

unsigned i2c_read(char address, char reg, unsigned char *data, char length)
{
    unsigned char i=0;
    unsigned char retry = 0;
    i2c_start();                     // Begin the start sequence
    i2c_send(address << 1 | 0); //write
    i2c_send(reg); 
    i2c_restart();
    i2c_send(address << 1 | 1); //read

    
    while (i<length)
    {
        if (i2c_recv(data))
        {
            data++;
            i++;
            if (i<length)
                i2c_ack(0);
        }
        else
        break;
    }
    i2c_ack(1);
    i2c_stop();
    return i;
}