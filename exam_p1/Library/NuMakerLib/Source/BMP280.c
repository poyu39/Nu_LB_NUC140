//
// BMP280 device driver
//
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "NUC100Series.h"
#include "I2Cdev.h"
#include "BMP280.h"                

volatile uint16_t idata AC1,AC2,AC3,B1,B2,MC,MD; 

volatile uint32_t idata AC4,AC5,AC6; 

//unsigned char idata AltitudeTempFlag;                  //??????????????

//int idata AltitudeTemp[10]={ 0,0,0,0,0,0,0,0,0,0};       //?????10????,????


void DelayMs (unsigned int nbrOfUs) 
{
  uint16_t i=0;
  for( i=0; i<nbrOfUs; i++);
}

uint16_t BMP280_Read_2Byte(uint8_t index)
{   
    uint8_t read_data[2];
    int dat;

    I2C_readBytes(BMP280_I2C_PORT, BMP280_I2C_SLA, index, 2, *read_data);

    DelayMs(50);
    dat = read_data[0] << 8;
    dat |= read_data[1];    

    return dat;
}

long BMP280_Get_UT(void)
{
	uint32_t data[2];
	  data[0]=0x22;
    I2C_writeBytes(BMP280_I2C_PORT, BMP280_I2C_SLA, CTRL_REG_ADD, 1, data[]);
    DelayMs(CONVERSION_TIME);
    I2C_readBytes (BMP280_I2C_PORT, BMP280_I2C_SLA, 0xFA, 2, data[]);
	  return (data);
}

long BMP280_Get_UP(void)
{
    long pressure = 0;

    I2C_writeBytes(BMP280_I2C_PORT, BMP280_I2C_SLA, CTRL_REG_ADD, 1, 0x06);
    DelayMs(CONVERSION_TIME);  
    pressure = BMP280_Read_2Byte(0xF7);
    pressure &= 0x00FFFF;   
    return pressure;    
}

void Init_BMP280()
{
    I2C_readBytes(BMP280_I2C_PORT,BMP280_SLA, 0xAA, 1, AC1);               
    I2C_readBytes(BMP280_I2C_PORT,BMP280_SLA, 0xAC);
    I2C_readBytes(BMP280_I2C_PORT,BMP280_SLA, 0xAE);
    AC4 = I2C_readBytes(BMP280_I2C_PORT,BMP280_SLA, 0xB0);
    AC5 = I2C_readBytes(BMP280_I2C_PORT,BMP280_SLA, 0xB2);
    AC6 = I2C_readBytes(BMP280_I2C_PORT,BMP280_SLA, 0xB4);
    I2C_readBytes(BMP280_I2C_PORT,BMP280_SLA, 0xB6);
    I2C_readBytes(BMP280_I2C_PORT,BMP280_SLA, 0xB8);
//  MB =  BMP280_Read_2Byte(0xBA);
    I2C_readBytes(BMP280_I2C_PORT,BMP280_SLA, 0xBC);
    I2C_readBytes(BMP280_I2C_PORT,BMP280_SLA, 0xBE);
}

long BMP280_Get_Param(bit choice)
{

    long UT;

    long UP;

    long X1, X2, B5, B6, X3, B3, p;

    unsigned long B4, B7;

    long  temperature;

    long  pressure;

    UT = BMP280_Get_UT();                               // ????????

    UP = BMP280_Get_UP();                               // ????????

     

    X1 = ((long)UT - AC6) * AC5 >> 15;                    //????????????????????

    X2 = ((long) MC << 11) / (X1 + MD);

    B5 = X1 + X2;

    temperature = (B5 + 8) >> 4;

 

    if(choice==0)

        return (long)temperature;                       //?????,0.1?

     

    B6 = B5 - 4000;                                     //????????????????????

    X1 = (B2 * (B6 * B6 >> 12)) >> 11;

    X2 = AC2 * B6 >> 11;

    X3 = X1 + X2;

    B3 = (((long)AC1 * 4 + X3) + 2)/4;

    X1 = AC3 * B6 >> 13;

    X2 = (B1 * (B6 * B6 >> 12)) >> 16;

    X3 = ((X1 + X2) + 2) >> 2;

    B4 = (AC4 * (unsigned long) (X3 + 32768)) >> 15;

    B7 = ((unsigned long) UP - B3) * (50000);

    if( B7 < 0x80000000)

        p = (B7 * 2) / B4 ;

    else 

        p = (B7 / B4) * 2;

    X1 = (p >> 8) * (p >> 8);

    X1 = (X1 * 3038) >> 16;

    X2 = (-7357 * p) >> 16;

    pressure = p + ((X1 + X2 + 3791) >> 4);

    return (long)pressure;                              //?????,Pa

}
