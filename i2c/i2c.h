#include<xc.h>           // processor SFR definitions
#include"../timer/timer.h"

//clock settings
#define I2C_SYS_FREQ    48000000 // PIC clock speed
#define I2C_FSCK        100000   // Desired i2c channel clock speed 
#define I2C_BRG_VAL     I2C_SYS_FREQ/2/I2C_FSCK

//op timeout
#define I2C_TIMEOUT     480000 // one millisecond

//error constant
#define I2C_ERROR 1

void i2c_setup(void);
unsigned i2c_start(void);
unsigned i2c_restart(void);
unsigned i2c_send(unsigned char byte);
unsigned i2c_recv(unsigned char *buffer);
unsigned i2c_ack(int ack/* 0 ACK - 1 NACK */); 
unsigned i2c_stop(void);
unsigned i2c_write(unsigned char address, unsigned char reg, unsigned char *data, char length);
unsigned i2c_read(char address, char reg, unsigned char *data, char length);
