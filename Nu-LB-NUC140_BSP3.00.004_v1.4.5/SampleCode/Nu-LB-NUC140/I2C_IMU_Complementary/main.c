//
// I2C_IMU_Complementary : to calculate AHRS using Complementary filter
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

#define PI 3.14159265359         // Degree = Radians * 180 /  PI
#define gSens 131.072            // gRes = 250 degree/s, gSens = 32768/250
#define aSens 16384.0            // aRes = 2G,           aSens = 32768/2

#define ALPHA 0.98               // Complementary filter parameters

// Global Variables for IMU
char Text[16];
float dt = 0.01; // sample time = 100Hz (10ms)
int16_t Acc_out[3], Gyro_out[3];
int16_t Gyro_rate[2];
float Gyro_pitch, Acc_pitch;
float Gyro_roll,  Acc_roll;
float Comp_pitch, Comp_roll;

volatile uint32_t millis =0;
uint32_t prevT, deltaT;

void TMR0_IRQHandler(void)
{
	millis++;
  TIMER_ClearIntFlag(TIMER0);
}

void Init_Timer(void)
{
  TIMER_Open(TIMER0, TMR0_OPERATING_MODE, TMR0_OPERATING_FREQ);
  TIMER_EnableInt(TIMER0);
  NVIC_EnableIRQ(TMR0_IRQn);
  TIMER_Start(TIMER0);	
}

int32_t main()
{
  SYS_Init();
  init_LCD();
  clear_LCD();
  print_Line(0,"Complementary");
	
  NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);
	
  MPU6050_address(MPU6050_DEFAULT_ADDRESS);
  MPU6050_initialize();
	
  Comp_pitch=0;
  Comp_roll=0;
	
	Init_Timer();
	prevT = millis;
  while(1) {
    Acc_out[0]=MPU6050_getAccelerationX();
    Acc_out[1]=MPU6050_getAccelerationY();
    Acc_out[2]=MPU6050_getAccelerationZ();		
    Gyro_out[0]=MPU6050_getRotationX();
    Gyro_out[1]=MPU6050_getRotationY();
    Gyro_out[2]=MPU6050_getRotationZ();	
		
		deltaT = millis - prevT;
		prevT = millis;
		// IMU's X = pitch
		Acc_pitch = atan(Acc_out[0]/ sqrt(pow(Acc_out[1],2)+pow(Acc_out[2],2))) *180/PI;
		Gyro_rate[0]= Gyro_out[0] / gSens;
		Gyro_pitch  = Gyro_pitch + Gyro_rate[0] * (deltaT/1000);	
		// IMU's Y = roll
		Acc_roll  = atan(Acc_out[1]/ sqrt(pow(Acc_out[0],2)+pow(Acc_out[2],2))) *180/PI;
		Gyro_rate[1]= Gyro_out[1] / gSens;
		Gyro_roll   = Gyro_roll +  Gyro_rate[1] * (deltaT/1000);
		// Complimentary Filter
		Comp_pitch = (Comp_pitch + Gyro_rate[0] * (deltaT/1000)) * ALPHA + Acc_pitch * (1 - ALPHA);
		Comp_roll  = (Comp_roll  + Gyro_rate[1] * (deltaT/1000)) * ALPHA + Acc_roll  * (1 - ALPHA);
		
    sprintf(Text,"pitch=%4d %4d", (int)Comp_pitch, (int)Acc_pitch);
		print_Line(1, Text);
    sprintf(Text,"roll =%4d %4d", (int)Comp_roll,  (int)Acc_roll);
		print_Line(2, Text);
    sprintf(Text,"deltaT=%d ms", deltaT);
		print_Line(3, Text);		
    }
}
