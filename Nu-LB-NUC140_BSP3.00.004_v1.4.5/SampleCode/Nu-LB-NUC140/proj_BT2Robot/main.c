//
// proj_BT2Robot :  Android App remote control vaccuum robot
//
// AppInventor2: 
//
// Board       : Nu-LB-NUC140
// MCU         : NUC140VE3CN (LQFP100)
// Module      : BT18 (Bluetooth 4.2) 
//             : MPU6050
//             : TB612FNG
//
// Bluetooth4.2 : BT18 (baudrate=9600, databit=8, stopbit=1, paritybit=0, flowcontrol=None)
// pin1 KEY  : N.C
// pin2 VCC  : to VCC +5V
// pin3 GND  : to GND
// pin4 TXD  : to UART0-RX/PB0 (NUC140VE3CN pin 32)
// pin5 RXD  : to UART0-TX/PB1 (NUC140VE3CN pin 33)
// pin6 STATE: N.C.
//
// IMU : MPU6050
// SCL : to I2C0-SCL/PA9 (NUC140VE3CN pin 11)
// SDA : to I2C0-SDA/PA8 (NUC140VE3CN pin 12)
//
// DC motor driver : TB6612FNG
// AO1 & AO2 connected to Left wheel motor
// BO1 & BO2 connected to Right wheel motor
// PWMA : to PA12 (NUC140VE3CN pin 65)
// AIN1 : to PC10 (NUC140VE3CN pin 59) 
// AIN2 : to PC11 (NUC140VE3CN pin 58)
// STDBY: to PC9  (NUC140VE3CN pin 60)
// BIN1 : to PC12 (NUC140VE3CN pin 57)
// BIN2 : to PC13 (NUC140VE3CN pin 56)
// PWMB : to PA13 (NUC140VE3CN pin 64)
// MPU6050 Connections
// SCL : to I2C0-SCL/PA9 (NUC140VE3CN pin 11)
// SDA : to I2C0-SDA/PA8 (NUC140VE3CN pin 12)

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "PWM.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "NVT_I2C.h"

#define PI 3.14159265359

//#define OUTPUT_CSV_READABLE_RAWDATA

char Text[128];

int Heading, TargetHeading;

#define BTBUF_LENGTH 16
char     BTcommand=' ';
volatile uint8_t BTcomRbuf[BTBUF_LENGTH];
volatile uint8_t BTcomRbytes = 0;
volatile uint8_t BTflag =0;

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
int yaw, pitch, roll;

volatile uint32_t millis =0;

// PID tuning method : Ziegler-Nichols method
const int Ku = 1000;  
const int Tu = 10000; // 10x Ku
const int Kp = 0.6 * Ku;
const int Ki = 1.2 * Ku / Tu;
const int Kd = 3 * Ku * Tu /40;

#define PWM_FULLPOWER  0xFFFF
int USR_FullPower = 8000;
int PID_FullPower = 8000;
int USR_MotorPower;
int PID_MotorPower;

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

// UART to receive data from Bluetooth HC-05
void UART02_IRQHandler(void)
{
	//uint8_t i;
	uint8_t c;
	uint32_t u32IntSts = UART0->ISR;
	
	if(u32IntSts & UART_IS_RX_READY(UART0)) // check ISR on & RX is ready
  {
		while (!(UART0->FSR & UART_FSR_RX_EMPTY_Msk)){ // check RX is not empty
			c = UART_READ(UART0); // read UART RX data
			if (c!='\n') {        // check line-end 
				BTcomRbuf[BTcomRbytes] = c;
				BTcomRbytes++;
			} else {
				BTcommand=BTcomRbuf[0];       // COMMAND = first charactor received
				//for (i=0; i<BTcomRbytes; i++)	 BT_Message[i]=BTcomRbuf[i]; // store received data to Message
				//for (i=BTcomRbytes; i<BTBUF_LENGTH; i++) BT_Message[i]=' ';
				BTcomRbytes=0;                 // clear Read buffer pointer
				BTflag=1;	                   // set flag when BT command input
				break;
			}
		}		
	}
}

void Init_UART(void)
{ 
  UART_Open(UART0, 9600);                       // set UART baud rate
  UART_ENABLE_INT(UART0, UART_IER_RDA_IEN_Msk); // enable UART interrupt (triggerred by Read-Data-Available)
  NVIC_EnableIRQ(UART02_IRQn);		                // enable Cortex-M0 NVIC interrupt for UART
}


uint32_t PWM_ConfigOutputCMR(PWM_T *pwm,
                                 uint32_t u32ChannelNum,
                                 uint32_t u32Frequency,
                                 uint16_t u16CMR)
{
    uint32_t u32Src;
    uint32_t u32PWMClockSrc;
    uint32_t u32PWMClkTbl[8] = {__HXT, __LXT, NULL, __HIRC, NULL, NULL, NULL, __LIRC};
    uint32_t i;
    uint8_t  u8Divider = 1, u8Prescale = 0xFF;
    // this table is mapping divider value to register configuration
    uint32_t u32PWMDividerToRegTbl[17] = {NULL, 4, 0, NULL, 1, NULL, NULL, NULL, 2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 3};
    uint16_t u16CNR = 0xFFFF;

    if(pwm == PWMA)
    {
        if(u32ChannelNum < 2)// channel 0 and channel 1
            u32Src = ((CLK->CLKSEL2 & (CLK_CLKSEL2_PWM01_S_EXT_Msk)) >> (CLK_CLKSEL2_PWM01_S_EXT_Pos - 2)) | (CLK->CLKSEL1 & (CLK_CLKSEL1_PWM01_S_Msk)) >> (CLK_CLKSEL1_PWM01_S_Pos);
        else // channel 2 and channel 3
            u32Src = ((CLK->CLKSEL2 & (CLK_CLKSEL2_PWM23_S_EXT_Msk)) >> (CLK_CLKSEL2_PWM23_S_EXT_Pos - 2)) | (CLK->CLKSEL1 & (CLK_CLKSEL1_PWM23_S_Msk)) >> (CLK_CLKSEL1_PWM23_S_Pos);
    }
    else //pwm == PWMB
    {
        if(u32ChannelNum < 2)// channel 0 and channel 1
            u32Src = ((CLK->CLKSEL2 & (CLK_CLKSEL2_PWM45_S_EXT_Msk)) >> (CLK_CLKSEL2_PWM45_S_EXT_Pos - 2)) | (CLK->CLKSEL2 & (CLK_CLKSEL2_PWM45_S_Msk)) >> (CLK_CLKSEL2_PWM45_S_Pos);
        else // channel 2 and channel 3
            u32Src = ((CLK->CLKSEL2 & (CLK_CLKSEL2_PWM67_S_EXT_Msk)) >> (CLK_CLKSEL2_PWM67_S_EXT_Pos - 2)) | (CLK->CLKSEL2 & (CLK_CLKSEL2_PWM67_S_Msk)) >> (CLK_CLKSEL2_PWM67_S_Pos);
    }

    if(u32Src == 2)
    {
        SystemCoreClockUpdate();
        u32PWMClockSrc = SystemCoreClock;
    }
    else
    {
        u32PWMClockSrc = u32PWMClkTbl[u32Src];
    }

    for(; u8Divider < 17; u8Divider <<= 1)    // clk divider could only be 1, 2, 4, 8, 16
    {
        i = (u32PWMClockSrc / u32Frequency) / u8Divider;
        // If target value is larger than CNR * prescale, need to use a larger divider
        if(i > (0x10000 * 0x100))
            continue;

        // CNR = 0xFFFF + 1, get a prescaler that CNR value is below 0xFFFF
        u8Prescale = (i + 0xFFFF) / 0x10000;

        // u8Prescale must at least be 2, otherwise the output stop
        if(u8Prescale < 3)
            u8Prescale = 2;

        i /= u8Prescale;

        if(i <= 0x10000)
        {
            if(i == 1)
                u16CNR = 1;     // Too fast, and PWM cannot generate expected frequency...
            else
                u16CNR = i;
            break;
        }
    }
    // Store return value here 'cos we're gonna change u8Divider & u8Prescale & u16CNR to the real value to fill into register
    i = u32PWMClockSrc / (u8Prescale * u8Divider * u16CNR);

    u8Prescale -= 1;
    u16CNR -= 1;
    // convert to real register value
    u8Divider = u32PWMDividerToRegTbl[u8Divider];

    // every two channels share a prescaler
    (pwm)->PPR = ((pwm)->PPR & ~(PWM_PPR_CP01_Msk << ((u32ChannelNum >> 1) * 8))) | (u8Prescale << ((u32ChannelNum >> 1) * 8));
    (pwm)->CSR = ((pwm)->CSR & ~(PWM_CSR_CSR0_Msk << (4 * u32ChannelNum))) | (u8Divider << (4 * u32ChannelNum));
    // set PWM to edge aligned type
    (pwm)->PCR &= ~(PWM_PCR_PWM01TYPE_Msk << (u32ChannelNum >> 1));
    (pwm)->PCR |= PWM_PCR_CH0MOD_Msk << (8 * u32ChannelNum);

    if(u16CMR)
    {
        *((__IO uint32_t *)((((uint32_t) & ((pwm)->CMR0)) + u32ChannelNum * 12))) = u16CMR; //u8Color * (u16CNR + 1) /255	- 1;
    }
    else
    {
        *((__IO uint32_t *)((((uint32_t) & ((pwm)->CMR0)) + u32ChannelNum * 12))) = 0;
    }
    *((__IO uint32_t *)((((uint32_t) & ((pwm)->CNR0)) + (u32ChannelNum) * 12))) = u16CNR;

    return(i);
}

void Init_PWM(void)
{
    // PWM0 frequency is 7.14KHz
    PWM_ConfigOutputCMR(PWM0, PWM_CH0, 7140, 0);
    PWM_ConfigOutputCMR(PWM0, PWM_CH1, 7140, 0);

    // Enable output of all PWM0 channels
    PWM_EnableOutput(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK);

	// Start
    PWM_Start(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK);
}

void Init_GPIO(void)
{
  GPIO_SetMode(PC, BIT9 | BIT10 | BIT11 | BIT12 | BIT13, GPIO_MODE_OUTPUT);
  PC9=0; PC10=0; PC11=0; PC12=0; PC13=0;
}

void Motor_Forward(void)
{ PC10=1; PC11=0; PC12=0; PC13=1;}

void Motor_Backward(void)
{ PC10=0; PC11=1; PC12=1; PC13=0;}

void Motor_Left(void) 
{ PC10=0; PC11=1; PC12=0; PC13=1;}

void Motor_Right(void) 
{ PC10=1; PC11=0; PC12=1; PC13=0;}

void Motor_Stop(void) 
{ PC10=0; PC11=0; PC12=0; PC13=0;}

void Motor_Power(void){
	int Right_MotorPower, Left_MotorPower;
  switch (BTcommand) {
		if (Heading==TargetHeading) PID_MotorPower = 0;
    case 'F':
		  Left_MotorPower =USR_MotorPower - PID_MotorPower;				
      Right_MotorPower=USR_MotorPower + PID_MotorPower;	
		  break;
    case 'B':	
		  Left_MotorPower =USR_MotorPower + PID_MotorPower;				
      Right_MotorPower=USR_MotorPower - PID_MotorPower;
		  break;
    case 'R':
		  Left_MotorPower =USR_MotorPower - PID_MotorPower;			
      Right_MotorPower=USR_MotorPower + PID_MotorPower;
		  break;
    case 'L':
		  Left_MotorPower =USR_MotorPower + PID_MotorPower;			
      Right_MotorPower=USR_MotorPower - PID_MotorPower;
		  break;
    case 'S':
			Left_MotorPower =0;
      Right_MotorPower=0;
		  break;
    default:
		  Left_MotorPower =0;			
      Right_MotorPower=0;
		  break;
	}
	if (Left_MotorPower<0)  Left_MotorPower=0;
	if (Right_MotorPower<0) Right_MotorPower=0;	
  PWM_ConfigOutputCMR(PWM0, PWM_CH0, 7140, Left_MotorPower);		
  PWM_ConfigOutputCMR(PWM0, PWM_CH1, 7140, Right_MotorPower); 
  sprintf(Text, "%c,%d,%d,%d,%d\n", BTcommand, Heading, TargetHeading, Left_MotorPower, Right_MotorPower);
	SerialPrint(Text);
}

void BT_Command(void)
{ 
  switch (BTcommand) {	
    case 'F': Motor_Forward();		
		          break;
    case 'B':	Motor_Backward();				
		          break;
    case 'R': Motor_Right();
		          break;
    case 'L': Motor_Left();
		          break;
    case 'S': Motor_Stop();
		          break;
    default:  Motor_Stop();
		          break;
	}		
}

void Init_IMU(void)
{
	NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);
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
}

void getHeading(void)
{
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

		// display Euler angles in degrees
    MPU6050_dmpGetQuaternion(&q, fifoBuffer);
    MPU6050_dmpGetGravity(&gravity, &q);
    MPU6050_dmpGetYawPitchRoll(ypr, &q, &gravity);
		yaw   = (int)(ypr[0]*180/PI);
		pitch = (int)(ypr[1]*180/PI);
		roll  = (int)(ypr[2]*180/PI);
		
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
			

	}
}

void LimitInt(int *x,int Min, int Max)
{
  if(*x > Max)
    *x = Max;
  if(*x < Min)
    *x = Min;

}//END LimitInt

//
// Clamp a float between a min and max.  Note doubles are the same 
// as floats on this platform.

void LimitFloat(float *x,float Min, float Max)
{
  if(*x > Max)
    *x = Max;
  if(*x < Min)
    *x = Min;

}//END LimitInt

void PID(float Heading,float HeadingTarget,int *Power, float kP,float kI,float kD, uint8_t Moving)                                                 
{
	unsigned long now;
	float timeChange;
	float dErr;	
  static unsigned long lastTime; 
  static float Output; 
  static float errSum, lastErr,error ; 

  // IF not moving then 
  if(!Moving)
  {
        errSum = 0;
        lastErr = 0;
        return;
  }

  //error correction for angular overlap
  error = Heading-HeadingTarget;
  if(error<180)
    error += 360;
  if(error>180)
    error -= 360;
      
  //http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/

  /*How long since we last calculated*/
  now = millis;    
  timeChange = (float)(now - lastTime);       
  /*Compute all the working error variables*/
  //float error = Setpoint - Input;    
  errSum += (error * timeChange);   

  //integral windup guard
  LimitFloat(&errSum, -300, 300);

  dErr = (error - lastErr) / timeChange;       

  /*Compute PID Output*/
  *Power = kP * error + kI * errSum + kD * dErr;
  /*Remember some variables for next time*/
  lastErr = error;    
  lastTime = now; 

  //limit demand 
  LimitInt(Power, - PID_FullPower,  PID_FullPower);

}//END getPID



int32_t main()
{	
	uint8_t Moving = 0;
// Setup()
  SYS_Init();	
  Init_UART();

	while(!BTflag);
	SerialPrint("BT2Robot begin\n");
	
	Init_IMU();		
  Init_Timer();
	
  Init_PWM();       // initialize PWM pins
  Init_GPIO();      // intitalize GPIO pins

  PC9=1; // STDBY 
  PWM_ConfigOutputCMR(PWM0, PWM_CH0, 7140, 0x8000);
  PWM_ConfigOutputCMR(PWM0, PWM_CH1, 7140, 0x8000);
  BTcommand='S';
	USR_MotorPower = USR_FullPower; // assign User defined full power
	
  // loop()		
  while(1) {
    if (!dmpReady) {SerialPrint("DMP not ready\n"); return 0;} // if DMP initialization failed, then do nothing.
		
	  getHeading();
		Heading=yaw;
		
		PID(Heading, TargetHeading, &PID_MotorPower, Kp, Ki, Kd, Moving); // run PID to get motor driving pwm value

		Motor_Power();   // set Motor Power
		
		if (BTflag==1) { // check if bluetooth sent new command
			if (BTcommand=='S') { 
				TargetHeading=yaw;
				sprintf(Text,"Target= %d\n", TargetHeading);
				SerialPrint(Text);
				Moving = 0;
			}
			else 
        Moving = 1;
		  BT_Command();
			BTflag=0;
		}		
	}
}
