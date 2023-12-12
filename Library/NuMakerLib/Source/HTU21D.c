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

bool HTU21DF_begin(void)
{
	uint8_t tmp;
  I2Cdev_readByte(HTU21DF_I2CADDR, HTU21DF_READREG, &tmp);
	return (tmp==0x02); // after reset should be 0x2
}

void HTU21DF_reset(void)
{
  I2Cdev_writeByte(HTU21DF_I2CADDR, HTU21DF_RESET, 0x00);
	CLK_SysTickDelay(15000); // Delay 15ms
}

uint8_t HTU21DF_ReadHumid(void)
{
  uint8_t data[1];
  I2Cdev_readByte(HTU21DF_I2CADDR, HTU21DF_READHUM, data); // trigger Temperature Hold master
  return data[0];
}

uint16_t HTU21DF_ReadTemp(void)
{
  uint8_t data[2];
  I2Cdev_readBytes(HTU21DF_I2CADDR, HTU21DF_READTEMP, 2, data); // trigger Temperature Hold master
  return((data[0]<<4) + (data[1]>>4));
}

float HTU21DF_Humidity(void)
{
	uint8_t sRH   = HTU21DF_ReadHumid(); //8-bit			
	float   Humid =  -6    + ((125 * sRH)/256);
	return (Humid);
}

float HTU21DF_Temperature(void)
{
	uint16_t sTemp = HTU21DF_ReadTemp();  //12-bit
	float    Temp  = -46.85 + ((175.72 * sTemp)/4096);
	return(Temp);
}
