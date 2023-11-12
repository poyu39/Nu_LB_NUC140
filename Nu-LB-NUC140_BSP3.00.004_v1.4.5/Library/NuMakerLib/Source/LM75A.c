//
// LM75A Driver: Temperature sensor
//
// pin1: SDA : I2C Serial Data
// pin2: SCL : I2C Serail Clock
// pin3: OS  : OverTemp Shutdown output - opendrain
// pin4: GND 
// pin5: A2  : user-defined address bit 2
// pin6: A1  : user-defined address bit 1
// pin7: A0  : user-defined address bit 0
// pin8: VCC : 2.8V to 5.5V

#include <stdio.h>
#include <stdint.h>
#include <NUC100Series.h>
#include "I2Cdev.h"
#include "LM75A.h"

uint8_t LM75A_ReadConfig(void)
{
  uint8_t data[1];
  I2C_readBytes(LM75A_devAddr, LM75A_CONF, 1, data);
  return (data[0]);
}

uint16_t LM75A_ReadTemp(void)
{
  uint8_t data[2];
  I2C_readBytes(LM75A_devAddr, LM75A_TEMP, 2, data);
  return (((data[0]<<8) + data[1])>>3);
}
