#define IMU_SLAVE_ADDR 0b1101011 // device addr  
#define IMU_SLAVE_ADDR_WRITE 0b11010110 // device addr + 0 Write 
#define IMU_SLAVE_ADDR_READ 0b11010111 // device addr + 1 Read 

#define IMU_INIT_ACC 0b10000000
#define IMU_INIT_GYR 0b10000010
#define IMU_INIT_CTR 0b00000100

#define IMU_REG_OUT_TEMP_L 0x20
#define IMU_REG_OUTX_L_G 0x22
#define IMU_REG_OUTX_L_XL 0x28
#define IMU_REG_WHOAMI 0x0F
#define IMU_REG_CTRL1_XL 0x10
#define IMU_REG_CTRL2_G 0x11
#define IMU_REG_CTRL3_C 0x11

void readTemp(short *t);
void readAcc(short *x, short *y, short *z);
void readGyro(short *x, short *y, short *z);
unsigned char start_imu();
