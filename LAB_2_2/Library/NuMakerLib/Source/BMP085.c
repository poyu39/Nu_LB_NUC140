//
// BMP085 Driver: Digital Pressure Sensor
//
// Interface: I2C (slave address = 0xEE)
// pin1: Gnd  (Ground)
// pin2: EOC  (End of Conversion)
// pin3: Vdda (Analog Power Supply)
// pin4: Vddd (Digital Power Supply)
// pin5: NC   (No Connection)
// pin6: SCL  (I2C Clock Input)
// pin7: SDA  (I2C Data)
// pin8: XCLR (Master Clear Input, low-active)

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "NUC100Series.h"
#include "I2Cdev.h"
#include "BMP085.h"

const float p0 = 101325;     // Pressure at sea level (Pa)

short   ac1, ac2, ac3;
unsigned short ac4, ac5, ac6;
short b1, b2, mb, mc, md;
long x1, x2, x3, b3, b5, b6;
unsigned long b4, b7;
int16_t OSS =0; // over-sampling setting	
long p;

void BMP085Calibration(void)
{
  uint8_t tmp[2];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, 0xAA, 2, tmp);
    ac1 = (tmp[1]<<8) | tmp[0];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, 0xAC, 2, tmp);	
    ac2 = (tmp[1]<<8) | tmp[0];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, 0xAE, 2, tmp);		
    ac3 = (tmp[1]<<8) | tmp[0];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, 0xB0, 2, tmp);			
    ac4 = (tmp[1]<<8) | tmp[0];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, 0xB2, 2, tmp);			 	
    ac5 = (tmp[1]<<8) | tmp[0];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, 0xB4, 2, tmp);			 	
    ac6 = (tmp[1]<<8) | tmp[0];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, 0xB6, 2, tmp);			 		
    b1 =  (tmp[1]<<8) | tmp[0];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, 0xB8, 2, tmp);			 		
    b2 =  (tmp[1]<<8) | tmp[0];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, 0xBA, 2, tmp);			 			
    mb =  (tmp[1]<<8) | tmp[0];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, 0xBC, 2, tmp);			 				
    mc =  (tmp[1]<<8) | tmp[0];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, 0xBE, 2, tmp);			 				
    md =  (tmp[1]<<8) | tmp[0];
}

uint16_t BMP085_ReadUT(void)
{
  uint8_t msb, lsb;
  uint8_t tmp[1], data[1];
    tmp[0] =0x2E;
    I2C_writeBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, BMP085_CR, 1, tmp);
    CLK_SysTickDelay(4500);
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, BMP085_MSB, 1, data);
    msb = data[0];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, BMP085_LSB, 1, data);
    lsb = data[0];	
    return ((msb<<8) | lsb);
}

uint32_t BMP085_ReadUP(void)
{
  uint8_t msb, lsb, xsb;
  uint8_t data[1];
    data[0]=0x34+(OSS<<6);
    I2C_writeBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, BMP085_CR, 1, data);
    CLK_SysTickDelay(4500);		
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, BMP085_MSB, 1, data);
    msb = data[0];
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, BMP085_LSB, 1, data);
    lsb = data[0]; 
    I2C_readBytes(BMP085_I2C_PORT, BMP085_I2C_SLA, BMP085_XSB, 1, data);	
    xsb = data[0];
    return( ( ((unsigned long)msb<<16) + ((unsigned long)lsb<<8) + (unsigned long)xsb )>> (8-OSS));
}

short BMP085GetTemperature(uint16_t ut)
{
    x1 = (((long)ut - (long)ac6)*(long)ac5)>>15;
    x2 = ((long)mc <<11)/(x1 + md);
    b5 = x1 + x2;

    return ((b5 + 8)>>4);	
}

long BMP085GetPressure(uint32_t up)
{
    b6 = b5 - 4000;
    // Calculate B3
    x1 = ((b2 * (b6 * b6))>>12)>>11;
    x2 = (ac2 * b6)>>11;
    x3 = x1 + x2;
    b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;
  
    // Calculate B4
    x1 = (ac3 * b6)>>13;
    x2 = (b1 * ((b6 * b6)>>12))>>16;
    x3 = ((x1 + x2) + 2)>>2;
    b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;  
    b7 = ((unsigned long)(up - b3) * (50000>>OSS));
    if (b7 < 0x80000000)
      p = (b7<<1)/b4;
    else
      p = (b7/b4)<<1;
    
    x1 = (p>>8) * (p>>8);
    x1 = (x1 * 3038)>>16;
    x2 = (-7357 * p)>>16;
    p += (x1 + x2 + 3791)>>4;
  
    return p;
}
