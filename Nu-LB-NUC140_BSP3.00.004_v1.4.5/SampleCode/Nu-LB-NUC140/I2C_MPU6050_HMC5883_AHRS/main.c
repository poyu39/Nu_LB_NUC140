//
// I2C_MPU6050_HMC5883_AHRS (Attitude and Heading Reference System)
// 
// IMU module : GY88 = MPU6050 + HMC5883L + BMP085
// 
// Connections:
// I2C0-SCL (PA9 = LQFP100 pin11 )
// I2C0-SDA (PA8 = LQFP100 pin12)
// UART0-RX (PB0 = LQFP100 pin32)
// UART0-TX (PB1 = LQFP100 pin33)
//	
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "HMC5883L.h"

#define PI 3.14159265359
#define RAD_TO_DEG  180/PI // degree = radians * 180 /  PI
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

enum Mrate { // set magnetometer ODR
  MRT_0075 = 0, // 0.75 Hz ODR
  MRT_015,      // 1.5 Hz
  MRT_030,      // 3.0 Hz
  MRT_075,      // 7.5 Hz
  MRT_15,       // 15 Hz
  MRT_30,       // 30 Hz
  MRT_75,       // 75 Hz ODR    
};

char Text[128];
VecInt16 mag;

// Specify sensor full scale
int Gscale = GFS_250DPS;
int Ascale = AFS_2G;
uint8_t Mrate = MRT_15;        //  15 Hz ODR 
float aRes, gRes, mRes; // scale resolutions per LSB for the sensors

int16_t accelCount[3];  // Stores the 16-bit signed accelerometer sensor output
int16_t gyroCount[3];   // Stores the 16-bit signed gyro sensor output
float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0}; // Bias corrections for gyro and accelerometer
int16_t tempCount;   // Stores the real internal chip temperature in degrees Celsius
float temperature;
float SelfTest[6];
int16_t magCount[3];    // 16-bit signed magnetometer sensor output
float   magbias[3];     // User-specified magnetometer corrections values

// global constants for 9 DoF fusion and AHRS (Attitude and Heading Reference System)
#define GyroMeasError PI * (40.0f / 180.0f)      // gyroscope measurement error in rads/s (shown as 40 deg/s)
#define GyroMeasDrift PI * (2.0f / 180.0f)       // gyroscope measurement drift in rad/s/s (shown as 2.0 deg/s/s)
#define beta sqrt(3.0f / 4.0f) * GyroMeasError   // compute beta
#define zeta sqrt(3.0f / 4.0f) * GyroMeasDrift   // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
#define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define Ki 0.0f

uint32_t delt_t = 0; // used to control display output rate
uint32_t count = 0;  // used to control display output rate
bool toggle = false;

float pitch, yaw, roll;
float deltaT = 0.0f;        // integration interval for both filter schemes

volatile uint32_t system_time=0; //ms
volatile uint32_t current_time=0; //ms

float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 
float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};    // vector to hold quaternion
float eInt[3] = {0.0f, 0.0f, 0.0f};       // vector to hold integral error for Mahony method

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

//#define OUTPUT_READABLE_RAWDATA
//#define OUTPUT_READABLE_YAWPITCHROLL
#define OUTPUT_TEAPOT_PACKET

// Timer0 for System Time (counting in 1ms)
void TMR0_IRQHandler(void)
{
	system_time++;
  TIMER_ClearIntFlag(TIMER0);
}

void Init_Timer0(void)
{
  TIMER_Open(TIMER0, TMR0_OPERATING_MODE, TMR0_OPERATING_FREQ); // 1000Hz = 1ms
  TIMER_EnableInt(TIMER0);
  NVIC_EnableIRQ(TMR0_IRQn);
  TIMER_Start(TIMER0);
}

void SerialPrint(char *text){
	UART_Write(UART0, text, strlen(text));
}
//===================================================================================================================
//====== Set of useful function to access acceleratio, gyroscope, and temperature data
//===================================================================================================================

void getGres() 
{
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

void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
	float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
	float norm;
	float hx, hy, _2bx, _2bz;
	float s1, s2, s3, s4;
	float qDot1, qDot2, qDot3, qDot4;

	// Auxiliary variables to avoid repeated arithmetic
	float _2q1mx;
	float _2q1my;
	float _2q1mz;
	float _2q2mx;
	float _4bx;
	float _4bz;
	float _2q1 = 2.0f * q1;
	float _2q2 = 2.0f * q2;
	float _2q3 = 2.0f * q3;
	float _2q4 = 2.0f * q4;
	float _2q1q3 = 2.0f * q1 * q3;
	float _2q3q4 = 2.0f * q3 * q4;
	float q1q1 = q1 * q1;
	float q1q2 = q1 * q2;
	float q1q3 = q1 * q3;
	float q1q4 = q1 * q4;
	float q2q2 = q2 * q2;
	float q2q3 = q2 * q3;
	float q2q4 = q2 * q4;
	float q3q3 = q3 * q3;
	float q3q4 = q3 * q4;
	float q4q4 = q4 * q4;

	// Normalise accelerometer measurement
	norm = sqrt(ax * ax + ay * ay + az * az);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f/norm;
	ax *= norm;
	ay *= norm;
	az *= norm;

	// Normalise magnetometer measurement
	norm = sqrt(mx * mx + my * my + mz * mz);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f/norm;
	mx *= norm;
	my *= norm;
	mz *= norm;

	// Reference direction of Earth's magnetic field
	_2q1mx = 2.0f * q1 * mx;
	_2q1my = 2.0f * q1 * my;
	_2q1mz = 2.0f * q1 * mz;
	_2q2mx = 2.0f * q2 * mx;
	hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
	hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
	_2bx = sqrt(hx * hx + hy * hy);
	_2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
	_4bx = 2.0f * _2bx;
	_4bz = 2.0f * _2bz;

	// Gradient decent algorithm corrective step
	s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	norm = sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
	norm = 1.0f/norm;
	s1 *= norm;
	s2 *= norm;
	s3 *= norm;
	s4 *= norm;

	// Compute rate of change of quaternion
	qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
	qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
	qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
	qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

	// Integrate to yield quaternion
	q1 += qDot1 * deltaT;
	q2 += qDot2 * deltaT;
	q3 += qDot3 * deltaT;
	q4 += qDot4 * deltaT;
	norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
	norm = 1.0f/norm;
	q[0] = q1 * norm;
	q[1] = q2 * norm;
	q[2] = q3 * norm;
	q[3] = q4 * norm;
}
  

 // Similar to Madgwick scheme but uses proportional and integral filtering on the error between estimated reference vectors and
 // measured ones. 
void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
	float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
	float norm;
	float hx, hy, bx, bz;
	float vx, vy, vz, wx, wy, wz;
	float ex, ey, ez;
	float pa, pb, pc;

	// Auxiliary variables to avoid repeated arithmetic
	float q1q1 = q1 * q1;
	float q1q2 = q1 * q2;
	float q1q3 = q1 * q3;
	float q1q4 = q1 * q4;
	float q2q2 = q2 * q2;
	float q2q3 = q2 * q3;
	float q2q4 = q2 * q4;
	float q3q3 = q3 * q3;
	float q3q4 = q3 * q4;
	float q4q4 = q4 * q4;   

	// Normalise accelerometer measurement
	norm = sqrt(ax * ax + ay * ay + az * az);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f / norm;        // use reciprocal for division
	ax *= norm;
	ay *= norm;
	az *= norm;

	// Normalise magnetometer measurement
	norm = sqrt(mx * mx + my * my + mz * mz);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f / norm;        // use reciprocal for division
	mx *= norm;
	my *= norm;
	mz *= norm;

	// Reference direction of Earth's magnetic field
	hx = 2.0f * mx * (0.5f - q3q3 - q4q4) + 2.0f * my * (q2q3 - q1q4) + 2.0f * mz * (q2q4 + q1q3);
	hy = 2.0f * mx * (q2q3 + q1q4) + 2.0f * my * (0.5f - q2q2 - q4q4) + 2.0f * mz * (q3q4 - q1q2);
	bx = sqrt((hx * hx) + (hy * hy));
	bz = 2.0f * mx * (q2q4 - q1q3) + 2.0f * my * (q3q4 + q1q2) + 2.0f * mz * (0.5f - q2q2 - q3q3);

	// Estimated direction of gravity and magnetic field
	vx = 2.0f * (q2q4 - q1q3);
	vy = 2.0f * (q1q2 + q3q4);
	vz = q1q1 - q2q2 - q3q3 + q4q4;
	wx = 2.0f * bx * (0.5f - q3q3 - q4q4) + 2.0f * bz * (q2q4 - q1q3);
	wy = 2.0f * bx * (q2q3 - q1q4) + 2.0f * bz * (q1q2 + q3q4);
	wz = 2.0f * bx * (q1q3 + q2q4) + 2.0f * bz * (0.5f - q2q2 - q3q3);  

	// Error is cross product between estimated direction and measured direction of gravity
	ex = (ay * vz - az * vy) + (my * wz - mz * wy);
	ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
	ez = (ax * vy - ay * vx) + (mx * wy - my * wx);
	if (Ki > 0.0f)
            {
                eInt[0] += ex;      // accumulate integral error
                eInt[1] += ey;
                eInt[2] += ez;
            }
	else
            {
                eInt[0] = 0.0f;     // prevent integral wind up
                eInt[1] = 0.0f;
                eInt[2] = 0.0f;
            }

	// Apply feedback terms
	gx = gx + Kp * ex + Ki * eInt[0];
	gy = gy + Kp * ey + Ki * eInt[1];
	gz = gz + Kp * ez + Ki * eInt[2];

	// Integrate rate of change of quaternion
	pa = q2;
	pb = q3;
	pc = q4;
	q1 = q1 + (-q2 * gx - q3 * gy - q4 * gz) * (0.5f * deltaT);
	q2 = pa + (q1 * gx + pb * gz - pc * gy) * (0.5f * deltaT);
	q3 = pb + (q1 * gy - pa * gz + pc * gx) * (0.5f * deltaT);
	q4 = pc + (q1 * gz + pa * gy - pb * gx) * (0.5f * deltaT);

	// Normalise quaternion
	norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);
	norm = 1.0f / norm;
	q[0] = q1 * norm;
	q[1] = q2 * norm;
	q[2] = q3 * norm;
	q[3] = q4 * norm;
}

void initMPU6050()
{ 
  uint8_t c[1];	
  // wake up device
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors 
  CLK_SysTickDelay(100000); // Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt  

  // get stable time source
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0x01);  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001

  // Configure Gyro and Accelerometer
  // Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively; 
  // DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
  // Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_CONFIG, 0x03);  
 
  // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; the same rate set in CONFIG above
 
  // Set gyroscope full scale range
  // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
  I2Cdev_readByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_CONFIG, c);
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_CONFIG, c[0] & ~0xE0); // Clear self-test bits [7:5] 
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_CONFIG, c[0] & ~0x18); // Clear AFS bits [4:3]
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_CONFIG, c[0] | Gscale << 3); // Set full scale range for the gyro
   
  // Set accelerometer configuration
  I2Cdev_readByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_CONFIG, c);
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_CONFIG, c[0] & ~0xE0); // Clear self-test bits [7:5] 
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_CONFIG, c[0] & ~0x18); // Clear AFS bits [4:3]
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_CONFIG, c[0] | Ascale << 3); // Set full scale range for the accelerometer 

  // Configure Interrupts and Bypass Enable
  // Set interrupt pin active high, push-pull, and clear on read of INT_STATUS, enable I2C_BYPASS_EN so additional chips 
  // can join the I2C bus and all can be controlled by the Arduino as master
   I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_PIN_CFG, 0x22);    
   I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt
}

// Function which accumulates gyro and accelerometer data after device initialization. It calculates the average
// of the at-rest readings and then loads the resulting offsets into accelerometer and gyro bias registers.
void calibrateMPU6050(float * dest1, float * dest2)
{  
  uint8_t fifodata[12]; // data array to hold accelerometer and gyro x, y, z, data
	uint8_t data[2];
  uint16_t ii,packet_count, fifo_count;
  int32_t gyro_bias[3] = {0, 0, 0}, accel_bias[3] = {0, 0, 0};
  int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
	
  uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
  uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g
  
  uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
  uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis
  int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};  
	
  // reset device, reset all registers, clear gyro and accelerometer bias registers
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
  CLK_SysTickDelay(100000);  
   
  // get stable time source
  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0x01);  
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_PWR_MGMT_2, 0x00); 
  CLK_SysTickDelay(200000);
  
  // Configure device for bias calculation
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_ENABLE, 0x00);   // Disable all interrupts
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_FIFO_EN, 0x00);      // Disable FIFO
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0x00);   // Turn on internal clock source
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_I2C_MST_CTRL, 0x00); // Disable I2C master
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_USER_CTRL, 0x0C);    // Reset FIFO and DMP
  CLK_SysTickDelay(15000);
  
  // Configure MPU6050 gyro and accelerometer for bias calculation
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_CONFIG, 0x01);      // Set low-pass filter to 188 Hz
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity

  // Configure FIFO to capture accelerometer and gyro data for bias calculation
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_USER_CTRL, 0x40);   // Enable FIFO  
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO  (max size 1024 bytes in MPU-6050)
  CLK_SysTickDelay(80000); // accumulate 80 samples in 80 milliseconds = 960 bytes

  // At end of sample accumulation, turn off FIFO sensor read
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
  I2Cdev_readBytes(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_FIFO_COUNTH, 2, data); // read FIFO sample count
  fifo_count = (uint16_t) ((data[0] << 8) | data[1]);
	sprintf(Text,"fifo_count=%d\n", fifo_count);
	SerialPrint(Text);
  packet_count = fifo_count/12; // How many sets of full gyro and accelerometer data for averaging

  I2Cdev_readBytes(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_FIFO_R_W, 12, fifodata); // read data for averaging
		
  accel_temp[0] = (int16_t) ((fifodata[0] << 8) | fifodata[1] );  // Form signed 16-bit integer for each sample in FIFO
  accel_temp[1] = (int16_t) ((fifodata[2] << 8) | fifodata[3] );
  accel_temp[2] = (int16_t) ((fifodata[4] << 8) | fifodata[5] );    
  gyro_temp[0]  = (int16_t) ((fifodata[6] << 8) | fifodata[7] );
  gyro_temp[1]  = (int16_t) ((fifodata[8] << 8) | fifodata[9] );
  gyro_temp[2]  = (int16_t) ((fifodata[10]<< 8) | fifodata[11]);
	//printf("accel_temp: %d %d %d, gyro_temp: %d %d %d\n", accel_temp[0], accel_temp[1], accel_temp[2], gyro_temp[0], gyro_temp[1], gyro_temp[2]);
    
  accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
  accel_bias[1] += (int32_t) accel_temp[1];
  accel_bias[2] += (int32_t) accel_temp[2];
  gyro_bias[0]  += (int32_t) gyro_temp[0];
  gyro_bias[1]  += (int32_t) gyro_temp[1];
  gyro_bias[2]  += (int32_t) gyro_temp[2];            

  accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
  accel_bias[1] /= (int32_t) packet_count;
  accel_bias[2] /= (int32_t) packet_count;
  gyro_bias[0]  /= (int32_t) packet_count;
  gyro_bias[1]  /= (int32_t) packet_count;
  gyro_bias[2]  /= (int32_t) packet_count;
    
  if(accel_bias[2] > 0L) {accel_bias[2] -= (int32_t) accelsensitivity;}  // Remove gravity from the z-axis accelerometer bias calculation
  else {accel_bias[2] += (int32_t) accelsensitivity;}
 
// Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
  fifodata[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
  fifodata[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
  fifodata[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
  fifodata[3] = (-gyro_bias[1]/4)       & 0xFF;
  fifodata[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
  fifodata[5] = (-gyro_bias[2]/4)       & 0xFF;

// Push gyro biases to hardware registers
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_XG_OFFS_USRH, fifodata[0]); 
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_XG_OFFS_USRL, fifodata[1]);
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_YG_OFFS_USRH, fifodata[2]);
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_YG_OFFS_USRL, fifodata[3]);
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ZG_OFFS_USRH, fifodata[4]);
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ZG_OFFS_USRL, fifodata[5]);

  dest1[0] = (float) gyro_bias[0]/(float) gyrosensitivity; // construct gyro bias in deg/s for later manual subtraction
  dest1[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
  dest1[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

// Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
// factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
// non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
// compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
// the accelerometer biases calculated above must be divided by 8.

  I2Cdev_readBytes(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_XA_OFFSET_H, 2, data); // Read factory accelerometer trim values
  accel_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
  I2Cdev_readBytes(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_YA_OFFSET_H, 2, data);
  accel_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
  I2Cdev_readBytes(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ZA_OFFSET_H, 2, data);
  accel_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];
  
  for(ii = 0; ii < 3; ii++) {
    if(accel_bias_reg[ii] & mask) mask_bit[ii] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
  }

  // Construct total accelerometer bias, including calculated average accelerometer bias from above
  accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
  accel_bias_reg[1] -= (accel_bias[1]/8);
  accel_bias_reg[2] -= (accel_bias[2]/8);
 
  fifodata[0] = (accel_bias_reg[0] >> 8) & 0xFF;
  fifodata[1] = (accel_bias_reg[0])      & 0xFF;
  fifodata[1] = fifodata[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
  fifodata[2] = (accel_bias_reg[1] >> 8) & 0xFF;
  fifodata[3] = (accel_bias_reg[1])      & 0xFF;
  fifodata[3] = fifodata[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
  fifodata[4] = (accel_bias_reg[2] >> 8) & 0xFF;
  fifodata[5] = (accel_bias_reg[2])      & 0xFF;
  fifodata[5] = fifodata[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

  // Push accelerometer biases to hardware registers
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_XA_OFFSET_H,    fifodata[0]);  
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_XA_OFFSET_L_TC, fifodata[1]);
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_YA_OFFSET_H,    fifodata[2]);
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_YA_OFFSET_L_TC, fifodata[3]);  
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ZA_OFFSET_H,    fifodata[4]);
  I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ZA_OFFSET_L_TC, fifodata[5]);

// Output scaled accelerometer biases for manual subtraction in the main program
   dest2[0] = (float)accel_bias[0]/(float)accelsensitivity; 
   dest2[1] = (float)accel_bias[1]/(float)accelsensitivity;
   dest2[2] = (float)accel_bias[2]/(float)accelsensitivity;
}

// Accelerometer and gyroscope self test; check calibration wrt factory settings
void MPU6050SelfTest(float * destination) // Should return percent deviation from factory trim values, +/- 14 or less deviation is a pass
{
	 int i;
   uint8_t rawData[4];
   uint8_t selfTest[6];
   float factoryTrim[6];
   
   // Configure the accelerometer for self-test
   I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_CONFIG, 0xF0); // Enable self test on all three axes and set accelerometer range to +/- 8 g
   I2Cdev_writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_CONFIG,  0xE0); // Enable self test on all three axes and set gyro range to +/- 250 degrees/s
   CLK_SysTickDelay(250000);  // Delay a while to let the device execute the self-test
   I2Cdev_readByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_SELF_TEST_X, &rawData[0]); // X-axis self-test results
   I2Cdev_readByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_SELF_TEST_Y, &rawData[1]); // Y-axis self-test results
   I2Cdev_readByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_SELF_TEST_Z, &rawData[2]); // Z-axis self-test results
   I2Cdev_readByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_SELF_TEST_A, &rawData[3]); // Mixed-axis self-test results
   // Extract the acceleration test results first
   selfTest[0] = (rawData[0] >> 3) | (rawData[3] & 0x30) >> 4 ; // XA_TEST result is a five-bit unsigned integer
   selfTest[1] = (rawData[1] >> 3) | (rawData[3] & 0x0C) >> 4 ; // YA_TEST result is a five-bit unsigned integer
   selfTest[2] = (rawData[2] >> 3) | (rawData[3] & 0x03) >> 4 ; // ZA_TEST result is a five-bit unsigned integer
   // Extract the gyration test results first
   selfTest[3] = rawData[0]  & 0x1F ; // XG_TEST result is a five-bit unsigned integer
   selfTest[4] = rawData[1]  & 0x1F ; // YG_TEST result is a five-bit unsigned integer
   selfTest[5] = rawData[2]  & 0x1F ; // ZG_TEST result is a five-bit unsigned integer   
   // Process results to allow final comparison with factory set values
   factoryTrim[0] = (4096.0*0.34)*(pow( (0.92/0.34) , (((float)selfTest[0] - 1.0)/30.0))); // FT[Xa] factory trim calculation
   factoryTrim[1] = (4096.0*0.34)*(pow( (0.92/0.34) , (((float)selfTest[1] - 1.0)/30.0))); // FT[Ya] factory trim calculation
   factoryTrim[2] = (4096.0*0.34)*(pow( (0.92/0.34) , (((float)selfTest[2] - 1.0)/30.0))); // FT[Za] factory trim calculation
   factoryTrim[3] = ( 25.0*131.0)*(pow( 1.046 , ((float)selfTest[3] - 1.0) ));             // FT[Xg] factory trim calculation
   factoryTrim[4] = (-25.0*131.0)*(pow( 1.046 , ((float)selfTest[4] - 1.0) ));             // FT[Yg] factory trim calculation
   factoryTrim[5] = ( 25.0*131.0)*(pow( 1.046 , ((float)selfTest[5] - 1.0) ));             // FT[Zg] factory trim calculation
   
   //  Output self-test results and factory trim calculation if desired
	 sprintf(Text,"Self-Test results:\n");
	 SerialPrint(Text);
   sprintf(Text, "%x %x %x\n", selfTest[0], selfTest[1], selfTest[2]);
	 SerialPrint(Text);	 
   sprintf(Text, "%x %x %x\n", selfTest[3], selfTest[4], selfTest[5]);	 
	 SerialPrint(Text);	 
	 sprintf(Text, "Factory-Trim Calculation\n");
	 SerialPrint(Text);	 
   sprintf(Text, "%f %f %f\n", factoryTrim[0], factoryTrim[1], factoryTrim[2]);
	 SerialPrint(Text);	 
   sprintf(Text, "%f %f %f\n", factoryTrim[3], factoryTrim[4], factoryTrim[5]);
	 SerialPrint(Text);
	 
 // Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of the Self-Test Response
 // To get to percent, must multiply by 100 and subtract result from 100
   for (i = 0; i < 6; i++) {
     destination[i] = 100.0 + 100.0*((float)selfTest[i] - factoryTrim[i])/factoryTrim[i]; // Report percent differences
   }  
}

void initHMC5883L()
{  
 // Set magnetomer ODR; default is 15 Hz 
  I2Cdev_writeByte(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_CONFIG_A, Mrate << 2);   
  I2Cdev_writeByte(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_CONFIG_B, 0x00);  // set gain (bits[7:5]) to maximum resolution of 0.73 mG/LSB
  I2Cdev_writeByte(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_MODE, 0x80 );     // enable continuous data mode
}

uint8_t selfTestHMC5883L()
{  
  int16_t selfTest[3] = {0, 0, 0};
  uint8_t rawData[6] = {0, 0, 0, 0, 0, 0}; // x/y/z gyro register data stored here	
  //  Perform self-test and calculate temperature compensation bias
  I2Cdev_writeByte(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_CONFIG_A, 0x71);   // set 8-average, 15 Hz default, positive self-test measurement
  I2Cdev_writeByte(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_CONFIG_B, 0xA0);   // set gain (bits[7:5]) to 5
  I2Cdev_writeByte(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_MODE, 0x80 );      // enable continuous data mode
  CLK_SysTickDelay(150000); // wait 150 ms

  I2Cdev_readBytes(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_OUT_X_H, 6, rawData);  // Read the six raw data registers sequentially into data array
  selfTest[0] = ((int16_t)rawData[0] << 8) | rawData[1];          // Turn the MSB and LSB into a signed 16-bit value
  selfTest[1] = ((int16_t)rawData[4] << 8) | rawData[5];  
  selfTest[2] = ((int16_t)rawData[2] << 8) | rawData[3]; 
  I2Cdev_writeByte(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_CONFIG_A, 0x00);   // exit self test
 
  if(selfTest[0] < 575 && selfTest[0] > 243 && selfTest[1] < 575 && selfTest[1] > 243 && selfTest[2] < 575 && selfTest[2] > 243)  
  { return true; } else { return false;}
}
void readAccelData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z accel register data stored here
  I2Cdev_readBytes(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, 6, rawData);  // Read the six raw data registers into data array
  destination[0] = (int16_t)((rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = (int16_t)((rawData[2] << 8) | rawData[3]) ;  
  destination[2] = (int16_t)((rawData[4] << 8) | rawData[5]) ; 
}

void readGyroData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  I2Cdev_readBytes(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_XOUT_H, 6, rawData);  // Read the six raw data registers sequentially into data array
  destination[0] = (int16_t)((rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = (int16_t)((rawData[2] << 8) | rawData[3]) ;  
  destination[2] = (int16_t)((rawData[4] << 8) | rawData[5]) ; 
}

void readMagData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  I2Cdev_readBytes(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_OUT_X_H, 6, rawData);  // Read the six raw data registers sequentially into data array
  destination[0] = ((int16_t)rawData[0] << 8) | rawData[1];       // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = ((int16_t)rawData[4] << 8) | rawData[5];  
  destination[2] = ((int16_t)rawData[2] << 8) | rawData[3]; 
}

int16_t readTempData()
{
  uint8_t rawData[2];  // x/y/z gyro register data stored here
  I2Cdev_readBytes(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_TEMP_OUT_H, 2, rawData);  // Read the two raw data registers sequentially into data array 
  return ((int16_t)rawData[0]) << 8 | rawData[1] ;  // Turn the MSB and LSB into a 16-bit value
}

int32_t main()
{
	int i;
	uint8_t tmp[1];
	uint8_t GY88_ID[4];
  SYS_Init();

	UART_Open(UART0, 115200);                       // set UART0 baud rate
  NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);
	
// Check MPU6050 & HMC5883L availability to do initialization
	I2Cdev_readByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_WHO_AM_I,   &GY88_ID[0]);
	I2Cdev_readByte(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_IDENT_A, &GY88_ID[1]);
	I2Cdev_readByte(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_IDENT_B, &GY88_ID[2]);
	I2Cdev_readByte(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_IDENT_C, &GY88_ID[3]);	
	if (GY88_ID[0]==0x68 && GY88_ID[1]==0x48 && GY88_ID[2]==0x34 && GY88_ID[3]==0x33)
	{
		sprintf(Text,"MPU6050 and HMC5883L are online!\n\r");
		SerialPrint(Text);
		MPU6050SelfTest(SelfTest);

		if(SelfTest[0] < 1.0f && SelfTest[1] < 1.0f && SelfTest[2] < 1.0f && SelfTest[3] < 1.0f && SelfTest[4] < 1.0f && SelfTest[5] < 1.0f) {
      calibrateMPU6050(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers  
      initMPU6050();
      sprintf(Text,"MPU6050 initialized for active data mode....\n\r"); // Initialize device for active mode read of acclerometer, gyroscope, and temperature
		  SerialPrint(Text);
		  if(selfTestHMC5883L()){
			  sprintf(Text,"HMC5883L passed self test!\n\r");
				SerialPrint(Text);
		  } else {
        sprintf(Text,"HMC5883L failed self test!\n\r");
				SerialPrint(Text);
		  }
			initHMC5883L(); // Initialize and configure magnetometer
			sprintf(Text,"HMC5883L initialized for active data mode....\n\r");
			SerialPrint(Text);
	  }
	  else
		{
		  sprintf(Text,"Could not connect to MPU6050: %x\n", GY88_ID[0]);
			SerialPrint(Text);
			while(1); // Loop forever if communication doesn't happen
		}
	}

	Init_Timer0();
  while(1) {
		I2Cdev_readByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_STATUS, tmp);
    if( tmp[0] & 0x01) {  // check if data ready interrupt	 
      readAccelData(accelCount);
      getAres();
    
      // Now we'll calculate the accleration value into actual g's
      ax = (float)accelCount[0]*aRes;  // get actual g value, this depends on scale being set
      ay = (float)accelCount[1]*aRes;   
      az = (float)accelCount[2]*aRes;  
		
      readGyroData(gyroCount);	
      getGres();
 
      // Calculate the gyro value into actual degrees per second
      gx = (float)gyroCount[0]*gRes;  // get actual gyro value, this depends on scale being set
      gy = (float)gyroCount[1]*gRes;  
      gz = (float)gyroCount[2]*gRes;   
			
      tempCount = readTempData();  // Read the x/y/z adc values
      temperature = ((float) tempCount) / 340. + 36.53; // Temperature in degrees Centigrade
    }
		
		I2Cdev_readByte(HMC5883L_DEFAULT_ADDRESS, HMC5883L_REG_STATUS, tmp);		
		if(tmp[0] & 0x01) { // If data ready bit set, then read magnetometer data
      readMagData(magCount);
      mRes = 0.73; // Conversion to milliGauss, 0.73 mG/LSB in highest resolution mode
      // So far, magnetometer bias is calculated and subtracted here manually, should construct an algorithm to do it automatically
      // like the gyro and accelerometer biases
      magbias[0] = +56.;   // User environmental x-axis correction in milliGauss
      magbias[1] = -118.;  // User environmental y-axis correction in milliGauss
      magbias[2] = +35.;   // User environmental z-axis correction in milliGauss
  
      // Calculate the magnetometer values in milliGauss
      // Include factory calibration per data sheet and user environmental corrections
      mx = (float)magCount[0]*mRes - magbias[0];  // get actual magnetometer value, this depends on scale being set
      my = (float)magCount[1]*mRes - magbias[1];  
      mz = (float)magCount[2]*mRes - magbias[2];
		}

  // The HMC5883 y-axis is aligned with the MPU-6050 x-axis; the HMC5883 x-axis is aligned with the MPU-6050 -y-axis;
  // The HMC5883 z-axis is pointing up, 180 degrees from where it should.
  // We have to make some allowance for this orientation mismatch in feeding the output to the quaternion filter.
  // For the MPU-6060/HMC5883 system, we have chosen a magnetic rotation that keeps the sensor forward along the x-axis just like
  // in the LSM9DS0 sensor. We negate the z-axis magnetic field to conform to AHRS convention of magnetic z-axis down.
  // This rotation can be modified to allow any convenient orientation convention.
  // This is ok by aircraft orientation standards!  
  // Pass gyro rate as rad/s
		
	   if (system_time>current_time)
	     deltaT = (system_time - current_time);
		 else
	     deltaT = -(system_time - current_time);
		 deltaT = deltaT/1000;	 
		 current_time = system_time;
		 
       MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f,  my,  -mx,  -mz);
     // MahonyQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f, my, -mx, -mz);
    
     #ifdef OUTPUT_READABLE_RAWDATA 
     sprintf(Text, "ax= %d, ay= %d, az= %d, ", (int)ax, (int)ay, (int)az); 
		 SerialPrint(Text);
     sprintf(Text, "gx = %d, gy = %d, gz = %d, ", (int)gx, (int)gy, (int)gz); 
		 SerialPrint(Text);		 
     sprintf("mx= %d, my= %d, mz= %d, ", (int)mx, (int)my, (int)mz); 
		 SerialPrint(Text);			 
     sprintf("q0= %f, q1= %f, q2= %f, q3= %f\n\r", q[0], q[1], q[2], q[3]); // w,x,y,z
		 SerialPrint(Text);		 
     #endif
    
  // Define output variables from updated quaternion---these are Tait-Bryan angles, commonly used in aircraft orientation.
  // In this coordinate system, the positive z-axis is down toward Earth. 
  // Yaw is the angle between Sensor x-axis and Earth magnetic North (or true North if corrected for local declination, looking down on the sensor positive yaw is counterclockwise.
  // Pitch is angle between sensor x-axis and Earth ground plane, toward the Earth is positive, up toward the sky is negative.
  // Roll is angle between sensor y-axis and Earth ground plane, y-axis up is positive roll.
  // These arise from the definition of the homogeneous rotation matrix constructed from quaternions.
  // Tait-Bryan angles as well as Euler angles are non-commutative; that is, the get the correct orientation the rotations must be
  // applied in the correct order which for this configuration is yaw, pitch, and then roll.
  // For more see http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles which has additional links.
	   #ifdef OUTPUT_READABLE_YAWPITCHROLL
     yaw   = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);   
     pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
     roll  = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
     pitch *= 180.0f / PI;
     yaw   *= 180.0f / PI; 
     yaw   -= 13.8; // Declination at Danville, California is 13 degrees 48 minutes and 47 seconds on 2014-04-04
     roll  *= 180.0f / PI;
		
     sprintf(Text,"ypr: %f %f %f\n\r", yaw, pitch, roll);
		 SerialPrint(Text);
     #endif

     #ifdef OUTPUT_TEAPOT_PACKET
     // display quaternion values in InvenSense Teapot demo format:
     teapotPacket[2] = (int)(q[0] * 16384) >>8;
     teapotPacket[3] = (int)(q[0] * 16384) & 0x00ff;
     teapotPacket[4] = (int)(q[1] * 16384) >> 8;
     teapotPacket[5] = (int)(q[1] * 16384) & 0x00ff;
     teapotPacket[6] = (int)(q[2] * 16384) >> 8;
     teapotPacket[7] = (int)(q[2] * 16384) & 0x00ff;
     teapotPacket[8] = (int)(q[3] * 16384) >> 8;
     teapotPacket[9] = (int)(q[3] * 16384) & 0x00ff;
		 for (i=0; i<14; i++) UART_WRITE(UART0, teapotPacket[i]);
     teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
     #endif
  }
}
