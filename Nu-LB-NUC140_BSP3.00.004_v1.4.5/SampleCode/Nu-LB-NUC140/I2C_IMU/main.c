//
// I2C_IMU
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN (LQFP100)
// IMU : MPU6050 (3-axis accelerometer & 3-axis gyroscope)
// 
// MPU6050 Connections
// SCL : to I2C0-SCL/PA9
// SDA : to I2C0-SDA/PA8

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "MPU6050.h"
#include "NVT_I2C.h"

char Text[16];

int32_t main()
{
  int16_t ax[1], ay[1], az[1];	
  int16_t gx[1], gy[1], gz[1];
	
  SYS_Init();
  init_LCD();
  clear_LCD();
  print_Line(0,"IMU: MPU6050");
	
  NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);

  MPU6050_address(MPU6050_DEFAULT_ADDRESS);
  MPU6050_initialize();

  while(1) {		
    MPU6050_getAcceleration(ax, ay, az);
    MPU6050_getRotation(gx, gy, gz);
		
    sprintf(Text, "x%6d X%6d", ax[0], gx[0]);
    print_Line(1,Text);
    sprintf(Text, "y%6d Y%6d", ay[0], gy[0]);
    print_Line(2,Text);
    sprintf(Text, "z%6d Z%6d", az[0], gz[0]);
    print_Line(3,Text);		
    }

}
