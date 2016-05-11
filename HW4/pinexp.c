#define PINEX_REG_IODIR 0x00
#define PINEX_REG_IPOL 0x01
#define PINEX_REG_GPINTEN 0x02
#define PINEX_REG_DEFVAL 0x03
#define PINEX_REG_INTCON 0x04
#define PINEX_REG_IOCON 0x05
#define PINEX_REG_GPPU 0x06
#define PINEX_REG_INTF 0x07
#define PINEX_REG_INTCAP 0x08
#define PINEX_REG_GPIO 0x09
#define PINEX_REG_OLAT 0x0A

#define PINEX_BASE_ADDRESS (0b0100 << 3)
 
void pinexp_set_reg(unsigned char reg, unsigned char value, unsigned char addr)
{
    i2c_write(PINEX_BASE_ADDRESS+addr,reg, &value,1);
}

void pinexp_start(unsigned char direction, unsigned char addr)
{
    i2c_setup();
    unsigned char conf[6] = {direction,0,direction,direction,direction,0x22};
    i2c_write(PINEX_BASE_ADDRESS+addr,PINEX_REG_IODIR, conf, 1);
    i2c_write(PINEX_BASE_ADDRESS+addr,PINEX_REG_GPPU, &direction, 1);
/*   i2c_write(PINEX_BASE_ADDRESS+addr, PINEX_REG_IODIR, &direction); // All Pins are Inputs 
   i2c_write(PINEX_BASE_ADDRESS+addr, PINEX_REG_IPOL, 0x00); // All Outputs are similar to inputs 
   i2c_write(PINEX_BASE_ADDRESS+addr, PINEX_REG_GPINTEN, &direction); //All Interrupt on Change 
   i2c_write(PINEX_BASE_ADDRESS+addr, PINEX_REG_DEFVAL, 0x00); //To Compare GPINTEN & INTCON against DEFVAL 
   i2c_write(PINEX_BASE_ADDRESS+addr, PINEX_REG_INTCON &direction); //Compare with Defcal Register 
   i2c_write(PINEX_BASE_ADDRESS+addr, PINEX_REG_IOCON, 0x0A); 
   i2c_write(PINEX_BASE_ADDRESS+addr, PINEX_REG_GPPU, 0x00); // Disable Pull-Up Resistors 
   i2c_write(PINEX_BASE_ADDRESS+addr, PINEX_REG_IOCON, 0x00); 
   i2c_write(PINEX_BASE_ADDRESS+addr, PINEX_REG_GPIO, 0x00); 
   i2c_write(PINEX_BASE_ADDRESS+addr, PINEX_REG_OLAT, 0x00);*/
}

void pinexp_write(unsigned char value,unsigned char addr)
{
     i2c_write(PINEX_BASE_ADDRESS+addr,PINEX_REG_GPIO, &value,1);
}

unsigned char pinexp_read(unsigned char addr)
{
    unsigned char d=0;
    i2c_read(PINEX_BASE_ADDRESS+addr, PINEX_REG_GPIO, &d, 1);
    return d;
}