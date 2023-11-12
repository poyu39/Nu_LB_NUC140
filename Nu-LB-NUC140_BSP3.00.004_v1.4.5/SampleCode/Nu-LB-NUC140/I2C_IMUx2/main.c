//
// I2C_IMUx2 : I2C0 interface with two IMUs 
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN (LQFP100)
// IMU : MPU6050 (3-axis accelerometer & 3-axis gyroscope)
// 
// MPU6050 Connections
// SCL : to I2C0-SCL/PA9
// SDA : to I2C0-SDA/PA8
// AD0 : (1K pulldown on module)
//     : 1st IMU connected to Low
//     : 2nd IMU connected to High

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
  int16_t a0x[1], a0y[1], a0z[1];	
  int16_t a1x[1], a1y[1], a1z[1];	
	
  SYS_Init();
  init_LCD();
  clear_LCD();
  print_Line(0,"IMU: MPU6050");
	
  NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);

  MPU6050_address(MPU6050_ADDRESS_AD0_LOW); 
  MPU6050_initialize(); // initialize 1st IMU
  MPU6050_address(MPU6050_ADDRESS_AD0_HIGH);
  MPU6050_initialize();	// initialize 2nd IMU

  while(1) {
    MPU6050_address(MPU6050_ADDRESS_AD0_LOW);
    MPU6050_getAcceleration(a0x, a0y, a0z);
    MPU6050_address(MPU6050_ADDRESS_AD0_HIGH);
    MPU6050_getAcceleration(a1x, a1y, a1z);
		
    sprintf(Text, "x%6d %6d", a0x[0], a1x[0]);
    print_Line(1,Text);
    sprintf(Text, "y%6d %6d", a0y[0], a1y[0]);
    print_Line(2,Text);
    sprintf(Text, "z%6d %6d", a0z[0], a1z[0]);
    print_Line(3,Text);		
    }

}
