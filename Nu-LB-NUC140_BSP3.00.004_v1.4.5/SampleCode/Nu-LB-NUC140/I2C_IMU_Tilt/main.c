//
// I2C_IMU_Tilt
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
  int16_t ax[1], ay[1], az[1];	
  float pitch, roll;
	
  SYS_Init();
  init_LCD();
  clear_LCD();
  print_Line(0,"IMU Tilt");
	
  NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);

  MPU6050_address(MPU6050_DEFAULT_ADDRESS);
  MPU6050_initialize();

  while(1) {		
    MPU6050_getAcceleration(ax, ay, az);

    pitch = atan(ax[0]/ sqrt(pow(ay[0],2)+pow(az[0],2))) * 180/PI;	
    roll  = atan(ay[0]/ sqrt(pow(ax[0],2)+pow(az[0],2))) * 180/PI;

    sprintf(Text, "pitch %4d", (int)pitch);
    print_Line(1,Text);
    sprintf(Text, "roll  %4d", (int)roll);
    print_Line(2,Text);
		
    }

}
