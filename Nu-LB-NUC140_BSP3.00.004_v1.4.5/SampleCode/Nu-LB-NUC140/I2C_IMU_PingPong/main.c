//
// proj_IMU_Pingpong: project sample code for controlling bar to rebounce ball on LCD 
//                    by tilt angle measured by reading IMU
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// IMU : MPU6050
//
// MPU6050 - 3-axis accelerometer & 3-axis gyroscope
// SCL : to I2C0_SCL/PA9
// SDA : to I2C0_SDA/PA8
//
#include <stdio.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Draw2D.h"
#include "MPU6050.h"
#include "NVT_I2C.h"
#define PI 3.1415926535

#define PIXEL_ON  1
#define PIXEL_OFF 0
#define X0 15       // Circle initial X 
#define Y0 10       // Circle initial Y
#define BAR_X 64    // Bar initial X
#define BAR_Y 63    // Bar initial Y

int32_t main (void)
{
	int16_t accX, accY, accZ;
  float theta;
	
	int dirX_BAR, movX_BAR;
	int dirX, dirY;
	int movX, movY;
	uint16_t r;
	uint16_t x, y;
	uint16_t BAR_x, BAR_y, BAR_l;	
	uint16_t fgColor, bgColor;
	
	SYS_Init();
	NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);	//set I2C1 running at 400KHz
  MPU6050_address(MPU6050_DEFAULT_ADDRESS);
  MPU6050_initialize();
	
	init_LCD();  
	clear_LCD();
	
	
	x = X0;    // circle center x
	y = Y0;    // circle center y
	r = 5;    // circle radius
	
	BAR_x = BAR_X;
	BAR_y = BAR_Y;
	BAR_l = 7;
	
	movX = 1;  // x movement
	movY = 1;  // y movement
	dirX = 1;  // x direction
	dirY = 1;  // y direction
	
	dirX_BAR =0;
	movX_BAR =3;
	
	bgColor = BG_COLOR;
	while(1) {
	   fgColor = FG_COLOR;
	   draw_Circle(x, y, r, fgColor, bgColor); // draw a circle
		 draw_Line(BAR_x-BAR_l,BAR_y, BAR_x+BAR_l,BAR_y, fgColor, bgColor); // draw a line 

	   CLK_SysTickDelay(50000); // adjustable delay for vision
			
     fgColor = BG_COLOR;
	   draw_Circle(x, y, r, fgColor, bgColor); // erase a circle
     draw_Line(BAR_x-BAR_l,BAR_y, BAR_x+BAR_l,BAR_y, fgColor, bgColor); // draw a line	

		 // read Accelerometer to calculate tilt angle
     accX = MPU6050_getAccelerationX();
     accY = MPU6050_getAccelerationY();
     accZ = MPU6050_getAccelerationZ();
     theta = atan(accX / sqrt(pow(accY,2)+pow(accZ,2))) *180 /PI;
		
		 if      (theta<-20) dirX_BAR= 1;
		 else if (theta> 20) dirX_BAR=-1;
		 else                dirX_BAR= 0;

		 // boundary check for changing direction
     if      ((x-r) <=0)        dirX=1;  
	   else if ((x+r) > LCD_Xmax) dirX=-1;
		 
		 if ((y-r) <=0)             dirY=1;
	   else if ((y+r) >= BAR_y)
			    if (((x+r) < BAR_x-BAR_l) || ((x-r) > BAR_x+BAR_l)) y=0;
		      else                  dirY=-1;
					
	   x = x + dirX * movX; // change x of circle center
     y = y + dirY * movY; // change y of circle center
		 
     BAR_x = BAR_x + dirX_BAR * movX_BAR;
     if ((BAR_x-BAR_l)<0       ) BAR_x=LCD_Xmax-BAR_l;
		 if ((BAR_x+BAR_l)>LCD_Xmax) BAR_x=BAR_l;
   }
}
