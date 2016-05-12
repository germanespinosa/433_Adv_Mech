#include "../i2c/i2c.h"

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

unsigned pinexp_set_reg(unsigned char reg, unsigned char value, unsigned char addr);
unsigned pinexp_start(unsigned char direction, unsigned char addr);
unsigned pinexp_write(unsigned char value,unsigned char addr);
unsigned char pinexp_read(unsigned char addr);