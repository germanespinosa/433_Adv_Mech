#include "imu.h" 

#define IMU_SLAVE_ADDR 0b1101011 // device addr  
#define IMU_SLAVE_ADDR_WRITE 0b11010110 // device addr + 0 Write 
#define IMU_SLAVE_ADDR_READ 0b11010111 // device addr + 1 Read 

#define IMU_INIT_ACC 0b10000000
#define IMU_INIT_GYR 0b10000010
#define IMU_INIT_CTR 0b00000100

// Reads the 3 accelerometer channels and stores them in vector a
void readTemp(short *t)
{
  unsigned char data[2];
  I2C_read_multiple(IMU_SLAVE_ADDR, IMU_REG_OUT_TEMP_L, data, 2);
  
  unsigned char xla = data[0];
  unsigned char xha = data[1];

  // combine high and low bytes
  *t = (short)(xha << 8 | xla);
}

// Reads the 3 accelerometer channels and stores them in vector a
void readAcc(short *x, short *y, short *z)
{
  static short error_x= 0, error_y= 0, error_z= 0;
  static int error_sample =0;

  unsigned char data[6];
  
  I2C_read_multiple(IMU_SLAVE_ADDR, IMU_REG_OUTX_L_XL, data, 6);
  
  unsigned char xla = data[0];
  unsigned char xha = data[1];
  unsigned char yla = data[2];
  unsigned char yha = data[3];
  unsigned char zla = data[4];
  unsigned char zha = data[5];

  // combine high and low bytes
  *x = (short)(xha << 8 | xla) - error_x;
  *y = (short)(yha << 8 | yla) - error_y;
  *z = (short)(zha << 8 | zla) - error_z;
  if (error_sample < 50)
  {
      error_x = (error_x * error_sample + *x) / (error_sample + 1);
      error_y = (error_y * error_sample + *y) / (error_sample + 1);
      error_z = (error_z * error_sample + *z) / (error_sample + 1);
      error_sample++;
  }
}

void readGyro(short *x, short *y, short *z)
{
  static short error_x= 0, error_y= 0, error_z= 0;
  static int error_sample = 0;
    
  unsigned char data[6];
  I2C_read_multiple(IMU_SLAVE_ADDR, IMU_REG_OUTX_L_G, data, 6);

  unsigned char xlg = data[0];
  unsigned char xhg = data[1];
  unsigned char ylg = data[2];
  unsigned char yhg = data[3];
  unsigned char zlg = data[4];
  unsigned char zhg = data[5];

  // combine high and low bytes
  *x = (short)(xhg << 8 | xlg) - error_x;
  *y = (short)(yhg << 8 | ylg) - error_y;
  *z = (short)(zhg << 8 | zlg) - error_z;
  if (error_sample < 50)
  {
      error_x = (error_x * error_sample + *x) / (error_sample + 1);
      error_y = (error_y * error_sample + *y) / (error_sample + 1);
      error_z = (error_z * error_sample + *z) / (error_sample + 1);
      error_sample++;
  }  
}
void start_imu()
{
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send(IMU_SLAVE_ADDR_WRITE);
    i2c_master_send(IMU_REG_CTRL1_XL); // OLAT
    i2c_master_send(IMU_INIT_ACC); 
    i2c_master_stop();
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send(IMU_SLAVE_ADDR_WRITE);
    i2c_master_send(IMU_REG_CTRL2_G); // OLAT
    i2c_master_send(IMU_INIT_GYR); 
    i2c_master_stop();
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send(IMU_SLAVE_ADDR_WRITE);
    i2c_master_send(IMU_REG_CTRL3_C); // OLAT
    i2c_master_send(IMU_INIT_CTR); 
    i2c_master_stop();    
}

