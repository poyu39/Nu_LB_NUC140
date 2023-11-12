//
// I2C_MPU9250_DMP
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN (LQFP100)
// IMU : MPU9250 (MPU6500: 3-axis accelerometer & 3-axis gyroscope + AK8963 3-axis magnetometer)
//        
// 
// MPU9250 Connections
// SCL : to I2C0-SCL/PA9 (NUC140VE3CN pin 11)
// SDA : to I2C0-SDA/PA8 (NUC140VE3CN pin 12)

// Serial port :
// Comport-TX : to UART0-RX/PB0 (NUC140VE3CN pin 32)
// Comport-RX : to UART1-TX/PB1 (NUC140VE3CN pin 33)

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "MPU9250.h"
#include "NVT_I2C.h"

#define PI 3.14159265359

#define OUTPUT_CSV_READABLE_RAWDATA
//#define OUTPUT_READABLE_RAWDATA
//#define OUTPUT_READABLE_QUATERNION
//#define OUTPUT_READABLE_YAWPITCHROLL
//#define OUTPUT_TEAPOT_PACKET

char Text[128];

// orientation/motion vars
float q[4];             // [w, x, y, z]         quaternion container
float acc[3];           // [x, y, z]            accel sensor measurements
float gyro[3];          // [x, y, z]            gyro sensor measurements
float mag[3];           // [x, y, z]            magneto sensor measurements
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[22] = { '$', 0x02, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0x00, 0x00, '\r', '\n' };

volatile unsigned long millis;

void TMR0_IRQHandler(void)
{
  millis++;
  TIMER_ClearIntFlag(TIMER0);
}

void Init_Timer(void)
{
  millis=0;
  TIMER_Open(TIMER0, TMR0_OPERATING_MODE, TMR0_OPERATING_FREQ);
  TIMER_EnableInt(TIMER0);
  NVIC_EnableIRQ(TMR0_IRQn);
  TIMER_Start(TIMER0);
}

void SerialPrint(char *text){
  UART_Write(UART0, text, strlen(text));
}

int32_t main()
{	
// Setup()
  SYS_Init();
	
  UART_Open(UART0, 115200);           // UART0 = 11520
  NVT_I2C_Init(I2C0_CLOCK_FREQUENCY); // I2C0  = 400KHz

  // Call imu.begin() to verify communication and initialize
  while (MPU9250_begin() != INV_SUCCESS)
  {
    sprintf(Text, "Unable to communicate with MPU-9250\r\n"); SerialPrint(Text);
    sprintf(Text, "Check connections, and try again.\r\n"); SerialPrint(Text);
    CLK_SysTickDelay(1000);
  }

  // Enable 6-axis quat
  // Use gyro calibration
  // Set DMP FIFO rate to 10 Hz (Max sample rate = 200Hz)
  MPU9250_dmpBegin(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_GYRO_CAL, 10);	

  #ifndef OUTPUT_CSV_READABLE_RAWDATA
  // check MPU9250 setting
  sprintf(Text,"\r\n"); SerialPrint(Text);
  sprintf(Text,"Accel FSR = %d\r\n", MPU9250_getAccelFSR()); SerialPrint(Text); // 2G
  sprintf(Text,"Gyro  FSR = %d\r\n", MPU9250_getGyroFSR());  SerialPrint(Text); // 2000 deg/s
  sprintf(Text,"Mag   FSR = %d\r\n", MPU9250_getMagFSR());   SerialPrint(Text); // 4915 
  sprintf(Text,"SampleRate= %d\r\n", MPU9250_getSampleRate()); SerialPrint(Text); // 0 if DMP on, 50 if DMP off
  sprintf(Text,"Compass SampleRate= %d\r\n", MPU9250_getCompassSampleRate() ); SerialPrint(Text); // 10	
  sprintf(Text,"\r\n"); SerialPrint(Text); 
  #else
  // print header of CSV
  sprintf(Text,"time(ms), aX , aY, aZ, , gX, gY, GZ, mX, mY, mZ, q0, q1, q2, q3\r\n"); SerialPrint(Text); 
  #endif
			
  Init_Timer();
	
// loop()		
  while(1) { 	
    if ( MPU9250_fifoAvailable() )// Check for new data in the FIFO
    { // Use dmpUpdateFifo to update the ax, gx, mx, etc. values
			if ( MPU9250_dmpUpdateFifo() == INV_SUCCESS)
      {		
        #ifdef OUTPUT_CSV_READABLE_RAWDATA
        q[0] = MPU9250_calcQuat(imu.qw);
        q[1] = MPU9250_calcQuat(imu.qx);
        q[2] = MPU9250_calcQuat(imu.qy);
        q[3] = MPU9250_calcQuat(imu.qz);
        MPU9250_updateAccel();
        acc[0] = MPU9250_calcAccel(imu.ax);
        acc[1] = MPU9250_calcAccel(imu.ay);
        acc[2] = MPU9250_calcAccel(imu.az);
        MPU9250_updateGyro();
        gyro[0]= MPU9250_calcGyro(imu.gx);
        gyro[1]= MPU9250_calcGyro(imu.gy);
        gyro[2]= MPU9250_calcGyro(imu.gz);
        MPU9250_updateCompass();
        mag[0] = MPU9250_calcMag(imu.mx);
        mag[1] = MPU9250_calcMag(imu.my);
        mag[2] = MPU9250_calcMag(imu.mz);							
        sprintf(Text,"%d, %f,%f,%f, %f,%f,%f, %f,%f,%f, %f,%f,%f,%f\r\n", 
				             millis,acc[0],acc[1],acc[2],gyro[0],gyro[1],gyro[2],mag[0],mag[1],mag[2],q[0],q[1],q[2],q[3]);
        SerialPrint(Text);
        #endif
				
        #ifdef OUTPUT_READABLE_RAWDATA
        MPU9250_updateAccel();
        acc[0] = MPU9250_calcAccel(imu.ax);
        acc[1] = MPU9250_calcAccel(imu.ay);
        acc[2] = MPU9250_calcAccel(imu.az);
        MPU9250_updateGyro();
        gyro[0]= MPU9250_calcGyro(imu.gx);
        gyro[1]= MPU9250_calcGyro(imu.gy);
        gyro[2]= MPU9250_calcGyro(imu.gz);
        MPU9250_updateCompass();
        mag[0] = MPU9250_calcMag(imu.mx);
        mag[1] = MPU9250_calcMag(imu.my);
        mag[2] = MPU9250_calcMag(imu.mz);
            
        sprintf(Text,"acc: %f %f %f, gyro: %f %f %f, mag: %f %f %f\r\n", 
				             acc[0],acc[1],acc[2], gyro[0],gyro[1],gyro[2], mag[0],mag[1],mag[2]);
        SerialPrint(Text);
        #endif
			
        #ifdef OUTPUT_READABLE_QUATERNION
        // display quaternion values in easy matrix form: w x y z
        q[0] = MPU9250_calcQuat(imu.qw);
        q[1] = MPU9250_calcQuat(imu.qx);
        q[2] = MPU9250_calcQuat(imu.qy);
        q[3] = MPU9250_calcQuat(imu.qz);
        sprintf(Text, "Quat: %f %f %f %f\r\n", q[0], q[1], q[2], q[3]);
        SerialPrint(Text);
        #endif

        #ifdef OUTPUT_READABLE_YAWPITCHROLL
        MPU9250_computeEulerAngles(true); //compute Euler angles from Quaternions
        ypr[0] = imu.yaw;  // psi
        ypr[1] = imu.pitch; // theta
        ypr[2] = imu.roll;  // phi
        sprintf(Text, "Yaw-Pitch-Roll: %f %f %f\r\n", ypr[0], ypr[1], ypr[2]);
        SerialPrint(Text);
        #endif
              
        #ifdef OUTPUT_TEAPOT_PACKET					
        teapotPacket[2] = (imu.qw >>24) & 0x000000ff;
        teapotPacket[3] = (imu.qw >>16) & 0x000000ff;
        teapotPacket[4] = (imu.qw >>8 ) & 0x000000ff;
        teapotPacket[5] =  imu.qw       & 0x000000ff;
        teapotPacket[6] = (imu.qx >>24) & 0x000000ff;
        teapotPacket[7] = (imu.qx >>16) & 0x000000ff;
        teapotPacket[8] = (imu.qx >>8 ) & 0x000000ff;
        teapotPacket[9] =  imu.qx       & 0x000000ff;
        teapotPacket[10]= (imu.qy >>24) & 0x000000ff;
        teapotPacket[11]= (imu.qy >>16) & 0x000000ff;
        teapotPacket[12]= (imu.qy >>8 ) & 0x000000ff;
        teapotPacket[13]=  imu.qy       & 0x000000ff;
        teapotPacket[14]= (imu.qz >>24) & 0x000000ff;
        teapotPacket[15]= (imu.qz >>16) & 0x000000ff;
        teapotPacket[16]= (imu.qz >>8 ) & 0x000000ff;
        teapotPacket[17]=  imu.qz       & 0x000000ff;	
        UART_Write(UART0, teapotPacket, 22);
        teapotPacket[19]++; // packetCount, loops at 0xFF on purpose
        #endif
		  }
	  }
  }
}
