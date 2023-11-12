//
// I2C_IMUx2_Tilt : I2C0 interface with two IMUs & calculate their pitch/roll
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN (LQFP100)
// IMU : MPU6050 (3-axis accelerometer & 3-axis gyroscope)
// 
// MPU6050 Connections
// VCC : to VCC5 / VCC33
// GND : to GND
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

#define PI 3.14159265359

char Text[16];

int32_t main()
{
  int16_t a0x[1], a0y[1], a0z[1];	
  int16_t a1x[1], a1y[1], a1z[1];
  float pitch[2], roll[2];
	
  SYS_Init();
  init_LCD();
  clear_LCD();
  print_Line(0,"IMUx2 Tilt");
	
  NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);

  MPU6050_address(MPU6050_ADDRESS_AD0_LOW);
  MPU6050_initialize();
  MPU6050_address(MPU6050_ADDRESS_AD0_HIGH);
  MPU6050_initialize();	

  while(1) {
    MPU6050_address(MPU6050_ADDRESS_AD0_LOW);
    MPU6050_getAcceleration(a0x, a0y, a0z);
    MPU6050_address(MPU6050_ADDRESS_AD0_HIGH);
    MPU6050_getAcceleration(a1x, a1y, a1z);

    pitch[0] = atan(a0x[0]/ sqrt(pow(a0y[0],2)+pow(a0z[0],2))) * 180/PI;
    roll[0]  = atan(a0y[0]/ sqrt(pow(a0x[0],2)+pow(a0z[0],2))) * 180/PI;
    pitch[1] = atan(a1x[0]/ sqrt(pow(a1y[0],2)+pow(a1z[0],2))) * 180/PI;
    roll[1]  = atan(a1y[0]/ sqrt(pow(a1x[0],2)+pow(a1z[0],2))) * 180/PI;
		
    sprintf(Text, "pitch:%4d %4d", (int)pitch[0], (int)pitch[1]);
    print_Line(1,Text);
    sprintf(Text, "roll :%4d %4d", (int)roll[0], (int)roll[1]);
    print_Line(2,Text);
		
    }

}
