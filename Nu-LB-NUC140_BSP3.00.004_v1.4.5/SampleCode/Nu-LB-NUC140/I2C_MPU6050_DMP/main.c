//
// I2C_MPU6050_DMP
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN (LQFP100)
// IMU : MPU6050 (3-axis accelerometer & 3-axis gyroscope)
// 
// MPU6050 Connections
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
#include "MPU6050_6Axis_MotionApps20.h"
#include "NVT_I2C.h"

#define PI 3.14159265359
#define RAD_TO_DEG  180/PI // degree = radians * 180 /  PI

//#define OUTPUT_CSV_READABLE_RAWDATA
//#define OUTPUT_READABLE_RAWDATA
//#define OUTPUT_READABLE_REALACCEL
//#define OUTPUT_READABLE_QUATERNION
//#define OUTPUT_READABLE_EULER
#define OUTPUT_READABLE_YAWPITCHROLL
//#define OUTPUT_TEAPOT_PACKET

char Text[128];

// Set initial input parameters
enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum Gscale {
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};
/*
enum Mrate { // set magnetometer ODR
  MRT_0075 = 0, // 0.75 Hz ODR
  MRT_015,      // 1.5 Hz
  MRT_030,      // 3.0 Hz
  MRT_075,      // 7.5 Hz
  MRT_15,       // 15 Hz
  MRT_30,       // 30 Hz
  MRT_75,       // 75 Hz ODR    
};
*/
// Specify sensor full scale
int Gscale = GFS_250DPS;
int Ascale = AFS_2G;
//uint8_t Mrate = MRT_15;        //  15 Hz ODR (for Magnetometer)
float aRes, gRes, mRes; // scale resolutions per LSB for the sensors
float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0}; // Bias corrections for gyro and accelerometer
float magBias[3] = {+56., -118., +35.}; // // User environmental X,Y,Z correction in milliGauss

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 acc;         // [x, y, z]            accel sensor measurements
VectorInt16 accReal;     // [x, y, z]            accel sensor measurements
VectorInt16 accWorld;    // [x, y, z]            accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
VectorInt16 gyro;       // [x, y, z]            gyro sensor measurements
VectorInt16 mag;        // [x, y, z]            magneto sensor measurements
float ax, ay, az;
float gx, gy, gz;
float mx, my, mz;
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

volatile uint32_t millis =0;

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

void getAres() {
  switch (Ascale)
  {
 	// Possible accelerometer scales (and their register bit settings) are:
	// 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). 
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case AFS_2G:
          aRes = 2.0/32768.0;
          break;
    case AFS_4G:
          aRes = 4.0/32768.0;
          break;
    case AFS_8G:
          aRes = 8.0/32768.0;
          break;
    case AFS_16G:
          aRes = 16.0/32768.0;
          break;
  }
}

void getGres() {
  switch (Gscale)
  {
 	// Possible gyro scales (and their register bit settings) are:
	// 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11). 
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case GFS_250DPS:
          gRes = 250.0/32768.0;
          break;
    case GFS_500DPS:
          gRes = 500.0/32768.0;
          break;
    case GFS_1000DPS:
          gRes = 1000.0/32768.0;
          break;
    case GFS_2000DPS:
          gRes = 2000.0/32768.0;
          break;
  }
}

void SerialPrint(char *text){
	UART_Write(UART0, text, strlen(text));
}

int32_t main()
{	
// Setup()
  SYS_Init();	
  UART_Open(UART0, 9600);
  NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);

  CLK_SysTickLongDelay(5000000);	
  MPU6050_address(MPU6050_DEFAULT_ADDRESS);
  MPU6050_initialize();
	
  devStatus = MPU6050_dmpInitialize();
	
  // supply your own gyro offsets here, scaled for min sensitivity
  MPU6050_setXGyroOffset(220);
  MPU6050_setYGyroOffset(76);
  MPU6050_setZGyroOffset(-85);
  MPU6050_setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    printf("Enabling DMP...\n");
		
    MPU6050_setDMPEnabled(true);

    // enable Arduino interrupt detection
    printf("Enabling interrupt detection (Arduino external interrupt 0)...\n");
    //attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = MPU6050_getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    printf("DMP ready! Waiting for first interrupt...\n");
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = MPU6050_dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    printf("DMP Initialization failed (code %c)\n", devStatus);
    MPU6050_reset();
  }
		
  Init_Timer();
	
  // loop()		
  while(1) {
    // if DMP initialization failed, then do nothing.
    if (!dmpReady) return 0;
		
    // get INT_STATUS byte
    mpuIntStatus = MPU6050_getIntStatus();
		
    // get current FIFO count
    fifoCount = MPU6050_getFIFOCount();
		
    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
      // reset so we can continue cleanly
      MPU6050_resetFIFO();
      sprintf(Text,"FIFO overflow!\n"); SerialPrint(Text);
      // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
      // wait for correct available data length, should be a VERY short wait
      while (fifoCount < packetSize) fifoCount = MPU6050_getFIFOCount();

      // read a packet from FIFO
      MPU6050_getFIFOBytes(fifoBuffer, packetSize);
        
      // track FIFO count here in case there is > 1 packet available
      // (this lets us immediately read more without waiting for an interrupt)
      fifoCount -= packetSize;

			#ifdef OUTPUT_CSV_READABLE_RAWDATA
         MPU6050_dmpGetAccel(&acc, fifoBuffer);
				 getAres();
				 ax = acc.x * aRes;
				 ay = acc.y * aRes;
				 az = acc.z * aRes;
			   MPU6050_dmpGetGyro(&gyro, fifoBuffer);
				 getGres();
				 gx = gyro.x * gRes;
				 gy = gyro.y * gRes;
				 gz = gyro.z * gRes;
			   // MPU6050 has no magneto
			   mx =0; my=0; mz=0;
         MPU6050_dmpGetQuaternion(&q, fifoBuffer);			
			   sprintf(Text, "%d,%f,%f,%f, %f,%f,%f, %f,%f,%f, %f,%f,%f,%f\r\n", millis, ax,ay,az,gx,gy,gz,mx,my,mz,q.w,q.x,q.y,q.z);
				 SerialPrint(Text);		
      #endif
			
      #ifdef OUTPUT_READABLE_RAWDATA
         // display acc & gyrco values in easy matrix form: w x y z
         MPU6050_dmpGetAccel(&acc, fifoBuffer);
				 getAres();
				 ax = acc.x * aRes;
				 ay = acc.y * aRes;
				 az = acc.z * aRes;
			   MPU6050_dmpGetGyro(&gyro, fifoBuffer);
				 getGres();
				 gx = gyro.x * gRes;
				 gy = gyro.y * gRes;
				 gz = gyro.z * gRes;
			   sprintf(Text, "ax=%f, ay=%f, az=%f; gx=%f, gy=%f, gz=%f\r\n", ax, ay, az, gx, gy, gz);
				 SerialPrint(Text);			
      #endif
			
      #ifdef OUTPUT_READABLE_REALACCEL
         // display real acceleration, adjusted to remove gravity
         MPU6050_dmpGetQuaternion(&q, fifoBuffer);
         MPU6050_dmpGetAccel(&acc, fifoBuffer);
         MPU6050_dmpGetGravity(&gravity, &q);
         MPU6050_dmpGetLinearAccel(&accReal, &acc, &gravity);
         sprintf(Text, "accReal: %d %d %d\r\n", accReal.x, accReal.y, accReal.z);
				 SerialPrint(Text);
      #endif
				
      #ifdef OUTPUT_READABLE_QUATERNION
         // display quaternion values in easy matrix form: w x y z
         MPU6050_dmpGetQuaternion(&q, fifoBuffer);
			   sprintf(Text, "Q.w= %f, Q.x= %f, Q.y= %f, Q.z= %f)\r\n", q.w, q.x, q.y, q.z);
				 SerialPrint(Text);
      #endif
				
      #ifdef OUTPUT_READABLE_EULER
         // display Euler angles in degrees
         MPU6050_dmpGetQuaternion(&q, fifoBuffer);
         MPU6050_dmpGetEuler(euler, &q);
			   sprintf(Text, "Euler=( %f, %f, %f)\r\n", euler[0] * 180/PI, euler[1] * 180/PI, euler[2] * 180/PI);
				 SerialPrint(Text);
      #endif	
			
      #ifdef OUTPUT_READABLE_YAWPITCHROLL
         // display Euler angles in degrees
         MPU6050_dmpGetQuaternion(&q, fifoBuffer);
         MPU6050_dmpGetGravity(&gravity, &q);
         MPU6050_dmpGetYawPitchRoll(ypr, &q, &gravity);
			   sprintf(Text, "Yaw= %f,Pitch= %f,Roll= %f\r\n", ypr[0] * 180/PI, ypr[1] * 180/PI, ypr[2] * 180/PI);
				 SerialPrint(Text);
      #endif			
					
      #ifdef OUTPUT_TEAPOT_PACKET
         // display quaternion values in InvenSense Teapot demo format:
         teapotPacket[2] = fifoBuffer[0];
         teapotPacket[3] = fifoBuffer[1];
         teapotPacket[4] = fifoBuffer[4];
         teapotPacket[5] = fifoBuffer[5];
         teapotPacket[6] = fifoBuffer[8];
         teapotPacket[7] = fifoBuffer[9];
         teapotPacket[8] = fifoBuffer[12];
         teapotPacket[9] = fifoBuffer[13];		 
         UART_Write(UART0, teapotPacket, 14);
         teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
      #endif
		}
	}

}
