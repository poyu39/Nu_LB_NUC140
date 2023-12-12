//
// BH1750 Driver: Ambient Light Sensor
//
// Interface: I2C (slave address = 0xB8 when ADD=1, 0x46 when ADD=0)
// pin1: GND  (Ground)
// pin2: ADD  (Slave Address Selection) 
// pin3: SDA  (I2C Data)
// pin4: SCL  (I2C Clock Input)
// pin5: VCC  (+3.0~3.6V)

#include <stdio.h>
#include "NUC100Series.h"
#include "i2c.h"
#include "BH1750.h"

void BH1750_Write(uint8_t CODE) 
{ 
  I2C_START(BH1750_I2C_PORT);                    //Start
  I2C_WAIT_READY(BH1750_I2C_PORT);
	
  I2C_SET_DATA(BH1750_I2C_PORT, BH1750_I2C_SLA); //send slave address
  I2C_SET_CONTROL_REG(BH1750_I2C_PORT, I2C_SI);
  I2C_WAIT_READY(BH1750_I2C_PORT);

  I2C_SET_DATA(BH1750_I2C_PORT, CODE);           //send instruction 
  I2C_SET_CONTROL_REG(BH1750_I2C_PORT, I2C_SI);
  I2C_WAIT_READY(BH1750_I2C_PORT);

  I2C_STOP(BH1750_I2C_PORT);
}

uint16_t BH1750_Read(void)
{
  uint8_t  HiByte, LoByte;

  I2C_START(BH1750_I2C_PORT);                        //Start
  I2C_WAIT_READY(BH1750_I2C_PORT);
	
  I2C_SET_DATA(BH1750_I2C_PORT, (BH1750_I2C_SLA+1)); //send slave address+W
  I2C_SET_CONTROL_REG(BH1750_I2C_PORT, I2C_SI);
  I2C_WAIT_READY(BH1750_I2C_PORT);

  I2C_SET_CONTROL_REG(BH1750_I2C_PORT, I2C_SI);
  I2C_WAIT_READY(BH1750_I2C_PORT);							
  HiByte = I2C_GET_DATA(BH1750_I2C_PORT);            //read data

  I2C_SET_CONTROL_REG(BH1750_I2C_PORT, I2C_SI);
  I2C_WAIT_READY(BH1750_I2C_PORT);							
  LoByte = I2C_GET_DATA(BH1750_I2C_PORT);            //read data

  I2C_STOP(BH1750_I2C_PORT);
  return ((HiByte<<8) | LoByte);
}

void BH1750_Init(void)
{
  BH1750_Write(Power_On);
  BH1750_Write(Continuously_H_ResolutionMode);		
}
