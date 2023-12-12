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

int16_t BMP085_readword(uint8_t regAddr)
{
	uint8_t data[2];
	I2Cdev_readBytes(BMP085_devAddr, regAddr, 2, data);
	return data[0]<<8 | data[1];
}

void BMP085_Calibration(void)
{
    ac1=BMP085_readword(0xAA);
    ac2=BMP085_readword(0xAC);
    ac3=BMP085_readword(0xAE);
    ac4=BMP085_readword(0xB0);
    ac5=BMP085_readword(0xB2);
    ac6=BMP085_readword(0xB4);
    b1 =BMP085_readword(0xB6);
    b2 =BMP085_readword(0xB8);
    mb =BMP085_readword(0xBA);
    mc =BMP085_readword(0xBC);
    md =BMP085_readword(0xBE);		 				
}

uint16_t BMP085_ReadUT(void)
{
  uint8_t msb[1], lsb[1];
    I2Cdev_writeByte(BMP085_devAddr, BMP085_CR, 0x2E);
    CLK_SysTickDelay(4500);
    I2Cdev_readByte(BMP085_devAddr, BMP085_MSB, msb);
    I2Cdev_readByte(BMP085_devAddr, BMP085_LSB, lsb);
    return ((msb[0]<<8) | lsb[0]);
}

uint32_t BMP085_ReadUP(void)
{
  uint8_t msb[1], lsb[1], xsb[1];
  uint8_t data;
    data=0x34+(OSS<<6);
    I2Cdev_writeByte(BMP085_devAddr, BMP085_CR, data);
    CLK_SysTickDelay(4500);		
    I2Cdev_readByte(BMP085_devAddr, BMP085_MSB, msb);
    I2Cdev_readByte(BMP085_devAddr, BMP085_LSB, lsb); 
    I2Cdev_readByte(BMP085_devAddr, BMP085_XSB, xsb);	
    return( ( ((unsigned long)msb[0]<<16) + ((unsigned long)lsb[0]<<8) + (unsigned long)xsb[0] )>> (8-OSS));
}

short BMP085_GetTemperature(uint16_t ut)
{
    x1 = (((long)ut - (long)ac6)*(long)ac5)>>15;
    x2 = ((long)mc <<11)/(x1 + md);
    b5 = x1 + x2;

    return ((b5 + 8)>>4);	
}

long BMP085_GetPressure(uint32_t up)
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
