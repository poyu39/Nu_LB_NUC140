//
// HTU21D Driver: Digital Relative Humidity Sensor with Temperature Output
//
// Interface: I2C (slave address = 0x80)
// pin1: Gnd to GND
// pin2: Vdd to 3.3V
// pin3: SDA  (I2C Data) 
// pin4: SCL  (I2C Clock)

#include <stdio.h>
#include <stdint.h>
#include "NUC100Series.h"
#include "I2Cdev.h"
#include "HTU21D.h"

// Bit7 Bit0 RH     Temp
// 0    0    12bits 14bits
// 0    1    8 bits 12bits
// 1    0    10bits 13bits
// 1    1    11bits 11bits
uint8_t HTU21D_Init(void)
{
  uint8_t tmp[1], temp[1];
  I2C_readBytes(HTU21D_I2C_PORT, HTU21D_I2C_SLA, HTU21D_ReadUserregister, 1, tmp);
  temp[0]=0x03; // bit7=0, bit0=1 (RH = 8-bit, Temp=12-bit)
  I2C_writeBytes(HTU21D_I2C_PORT, HTU21D_I2C_SLA, HTU21D_WriteUserregister, 1, temp);
	return(tmp[0]);
}

uint8_t HTU21D_ReadHumid(void)
{
	uint8_t tmp[1];
  I2C_readBytes(HTU21D_I2C_PORT, HTU21D_I2C_SLA, HTU21D_TriggerHumidHold, 1,tmp); // trigger Temperature Hold master
	return(tmp[0]);
}

uint16_t HTU21D_ReadTemp(void)
{
	uint8_t tmp[2];
  I2C_readBytes(HTU21D_I2C_PORT, HTU21D_I2C_SLA, HTU21D_TriggerTempHold, 2,tmp); // trigger Temperature Hold master
	return((tmp[0]<<4) + (tmp[1]>>4));
}

float HTU21D_Humidity(void)
{
	uint8_t sRH   = HTU21D_ReadHumid(); //8-bit			
	float   Humid =  -6    + ((125 * sRH)/256);
	return (Humid);
}

float HTU21D_Temperature(void)
{
	uint16_t sTemp = HTU21D_ReadTemp();  //12-bit
	float    Temp  = -46.85 + ((175.72 * sTemp)/4096);
	return(Temp);
}
