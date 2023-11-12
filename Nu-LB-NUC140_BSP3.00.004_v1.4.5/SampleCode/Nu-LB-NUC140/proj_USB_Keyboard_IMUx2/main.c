//
// proj_USB_Keyboard_IMUx2: emulate USB Keyboard using IMUx2
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN#include <stdio.h>
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
#include "hid_kb.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "MPU6050.h"
#include "NVT_I2C.h"

#define PI 3.14159265359
#define KEY_A 0x04 // 0x50 (LEFT_ARROW)
#define KEY_D 0x07 // 0x4F (RIGHT_ARROW)
#define KEY_W 0x1A // 0x52 (UP_ARROW)
#define KEY_S 0x16 // 0x51 (DOWN_ARROW)
#define KEY_SPACE 0x2C
#define KEY_RETURN 0x28

char Text[16];

int16_t a0x[1], a0y[1], a0z[1];	
int16_t a1x[1], a1y[1], a1z[1];
float pitch[2], roll[2];

uint8_t volatile g_u8EP2Ready = 0;

uint32_t key = 0x2C;

void HID_UpdateKbData(void)
{
    int32_t i;
    uint8_t *buf;
    static uint32_t preKey;

    if(g_u8EP2Ready)
    {
        buf = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));

        if      ((int) pitch[1]<-30) key=KEY_SPACE;
        else if ((int) pitch[0]<-30) key=KEY_W;		
        else if ((int) pitch[0]> 30) key=KEY_S;
        else if ((int) roll[0] <-30) key=KEY_A;
        else if ((int) roll[0] > 30) key=KEY_D;
        else                         key=0;
			
        if(key == 0)
        {
            for(i = 0; i < 8; i++)
            {
                buf[i] = 0;
            }

            if(key != preKey)
            {
                /* Trigger to note key release */
                USBD_SET_PAYLOAD_LEN(EP2, 8);
            }
        }
        else
        {
            preKey = key;
            buf[2] = key; 
            USBD_SET_PAYLOAD_LEN(EP2, 8);
        }
    }
}

void Init_IMU(void)
{
  NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);

  MPU6050_address(MPU6050_ADDRESS_AD0_LOW);
  MPU6050_initialize();
  MPU6050_address(MPU6050_ADDRESS_AD0_HIGH);
  MPU6050_initialize();
}

void get_IMUx2_Tilt(void)
{
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

int32_t main(void)
{
    SYS_Init();
    USBD_Open(&gsInfo, HID_ClassRequest, NULL);
	  init_LCD();
	  clear_LCD();
	  print_Line(0, "proj_USB_IMUx2");
	
    Init_IMU();

    /* Endpoint configuration */
    HID_Init();
    USBD_Start();
    NVIC_EnableIRQ(USBD_IRQn);

    /* start to IN data */
    g_u8EP2Ready = 1;

    while(1)
    {
	    get_IMUx2_Tilt();		
      HID_UpdateKbData();
    }
}

