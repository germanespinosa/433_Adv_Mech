#include<xc.h>           // processor SFR definitions
#define _SUPPRESS_PLIB_WARNING 
#define _DISABLE_OPENADC10_CONFIGPORT_WARNING
// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C2 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k

#define SYS_FREQ    (24000000)
#define PBCLK       (SYS_FREQ/2)
#define Fsck        26000
#define BRG_VAL     (PBCLK/2/Fsck)

void i2c_master_setup(void);

// Start a transmission on the I2C bus
void i2c_master_start(void);

void i2c_master_restart(void);

void i2c_master_send(unsigned char byte);

unsigned char i2c_master_recv(void);

void i2c_master_ack(int val);

void i2c_master_stop(void);

void I2C_read_multiple(char address, char reg, unsigned char *data, char length);