//
// I2C_IMU_Kalman : to calculate AHRS using Kalman filter
//                  using Timer to calculate AHRS every 10ms (100Hz)
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
#define PI 3.14159265359
#define RAD_TO_DEG  180/PI

// Complementary filter parameters
#define gSens 131.072            // gRes = 250 degree/s, gSens = 32768/250
#define aSens 16384.0            // aRes = 2G,           aSens = 32768/2

#define ALPHA 0.98

// Global Variables for IMU
char Text[16];
int16_t Gyro_out[3], Acc_out[3];

int16_t Gyro_rate[2];
float Gyro_pitch, Acc_pitch;
float Gyro_roll,  Acc_roll;
float Comp_pitch, Comp_roll;
float GyroX_cal, GyroY_cal, GyroZ_cal;
float AccX_cal,  AccY_cal,  AccZ_cal;

// Global Variables for Kalman filter prediction
float Kalman_pitch, Kalman_roll;
float Q=0.1; // Prediction Estimate Initial Guess
float R=5;   // Prediction Estimate Initial Guess
float P00=0.1; // Preidction Estiamte Iintial Guess
float P11=0.1; // Preidction Estiamte Iintial Guess
float P01=0.1; // Preidction Estiamte Iintial Guess
float Kk0, Kk1;

volatile uint32_t millis = 0;
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

void Complementary_Filter(uint32_t dt)
{
  // Complimentary Filter
  Acc_pitch = atan(Acc_out[0]/ sqrt(pow(Acc_out[1],2)+pow(Acc_out[2],2))) * RAD_TO_DEG;	
  Acc_roll  = atan(Acc_out[1]/ sqrt(pow(Acc_out[0],2)+pow(Acc_out[2],2))) * RAD_TO_DEG;

  Gyro_rate[0]= Gyro_out[0] /gSens;
  Gyro_pitch  = Gyro_pitch + Gyro_rate[0] *dt;
	
  Gyro_rate[1]= Gyro_out[1] /gSens;	
  Gyro_roll   = Gyro_roll +  Gyro_rate[1] *dt;

  Comp_pitch = (Comp_pitch + Gyro_rate[0] *dt) * ALPHA + Acc_pitch * (1 - ALPHA);
  Comp_roll  = (Comp_roll  + Gyro_rate[1] *dt) * ALPHA + Acc_roll  * (1 - ALPHA);
}

void Kalman_Filter(float dt)
{
  //--- Kalman Filter ---
  // Time Update step 1
  Kalman_pitch = Kalman_pitch + ((Gyro_out[0] - GyroX_cal) / gSens) * dt; 
  Kalman_roll  = Kalman_roll  - ((Gyro_out[1] - GyroY_cal) / gSens) * dt;	
  // Measurement Update step 1
  //   Projected error covariance terms from derivation result
  P00 += dt * (2 * P01 + dt * P11); 
  P01 += dt * P11; 
  P00 += dt * Q;
  P11 += dt * Q;
	
  Kk0 = P00 / (P00 + R);
  Kk1 = P01 / (P01 + R); 
	
  // Measurement Update step 2
  Kalman_pitch += (Acc_pitch - Kalman_pitch) * Kk0; 
  Kalman_roll  += (Acc_roll  - Kalman_roll)  * Kk0;	
	
  // Measurement Update step 3
  P00 *= (1 - Kk0);
  P01 *= (1 - Kk1);
  P11 -= Kk1 * P01;	
}

void Print_LCD(void)
{
  sprintf(Text,"pitch=%4d %4d", (int)Kalman_pitch, (int)Comp_pitch);
  print_Line(1, Text);
  sprintf(Text,"roll =%4d %4d", (int)Kalman_roll,  (int)Comp_roll);
  print_Line(2, Text);
  sprintf(Text,"deltaT= %4d ms", deltaT);
  print_Line(3, Text);
}

int32_t main()
{
  SYS_Init();
  init_LCD();
  clear_LCD();
  print_Line(0,"Kalman filter");

  NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);	
  MPU6050_address(MPU6050_DEFAULT_ADDRESS);
  MPU6050_initialize();
	
  Init_Timer();
  prevT = millis;
  while(1) {
    Gyro_out[0]=MPU6050_getRotationX();
    Gyro_out[1]=MPU6050_getRotationY();
    Gyro_out[2]=MPU6050_getRotationZ();
    Acc_out[0]=MPU6050_getAccelerationX();
    Acc_out[1]=MPU6050_getAccelerationY();
    Acc_out[2]=MPU6050_getAccelerationZ();

    deltaT = millis - prevT;
    prevT = millis;		
    Complementary_Filter(deltaT/1000); // calculate pitch & roll using Complementary filter
    Kalman_Filter(deltaT/1000);        // calculate pitch & roll using Kalman filter					
    Print_LCD();		
	}
}
