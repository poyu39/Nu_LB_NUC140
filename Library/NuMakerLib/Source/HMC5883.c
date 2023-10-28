//
// HMC5883 Driver: E-compass
//
// Interface: I2C
// pin1: Vcc to Vcc (+5V)
// pin2: Gnd to Gnd
// pin3: SCL to I2C0_SCL/GPA9
// pin4: SDA to I2C0_SDA/GPA8
// pin5: DRDY N.C.
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "NUC100Series.h"
#include "I2Cdev.h"
#include "HMC5883.h"
#include "sensor.h"

hmc5883MagRegisters_t hmc5883_reg;

static float _hmc5883_Gauss_LSB_XY = 1100.0F;  // Varies with gain
static float _hmc5883_Gauss_LSB_Z  = 980.0F;   // Varies with gain

void HMC5883_write(uint8_t reg, uint8_t value)
{
	I2C_WriteBytes(HMC5883_I2C_PORT, HMC5883_I2C_SLA, reg, value, 1);
}

uint8_t HMC5883_read(uint8_t reg)
{
	uint8_t data[1];
	I2C_readBytes(HMC5883_I2C_PORT, HMC5883_I2C_SLA, reg, 1, data);
	return data;
}

void init_HMC5883(void)
{
	HMC5883_write(HMC5883_MAG_MR_REG_M, 0x00);
  CLK_SysTickDelay(20000);               // wait for 20ms
	setMagGain(HMC5883_MAGGAIN_1_3);
}

void setMagGain(uint8_t gain)
{
  HMC5883_write(HMC5883_MAG_CRB_REG_M, gain);
  
  switch(gain)
  {
    case HMC5883_MAGGAIN_1_3:
      _hmc5883_Gauss_LSB_XY = 1100;
      _hmc5883_Gauss_LSB_Z  = 980;
      break;
    case HMC5883_MAGGAIN_1_9:
      _hmc5883_Gauss_LSB_XY = 855;
      _hmc5883_Gauss_LSB_Z  = 760;
      break;
    case HMC5883_MAGGAIN_2_5:
      _hmc5883_Gauss_LSB_XY = 670;
      _hmc5883_Gauss_LSB_Z  = 600;
      break;
    case HMC5883_MAGGAIN_4_0:
      _hmc5883_Gauss_LSB_XY = 450;
      _hmc5883_Gauss_LSB_Z  = 400;
      break;
    case HMC5883_MAGGAIN_4_7:
      _hmc5883_Gauss_LSB_XY = 400;
      _hmc5883_Gauss_LSB_Z  = 255;
      break;
    case HMC5883_MAGGAIN_5_6:
      _hmc5883_Gauss_LSB_XY = 330;
      _hmc5883_Gauss_LSB_Z  = 295;
      break;
    case HMC5883_MAGGAIN_8_1:
      _hmc5883_Gauss_LSB_XY = 230;
      _hmc5883_Gauss_LSB_Z  = 205;
      break;
  }   
}

bool HMC5883_getEvent(sensors_event_t *event)
{
  memset(event, 0, sizeof(sensors_event_t));

  /* Read new data */
  read();
  
  event->version   = sizeof(sensors_event_t);
  event->sensor_id = _sensorID;
  event->type      = SENSOR_TYPE_MAGNETIC_FIELD;
  event->timestamp = 0;
  event->magnetic.x = _magData.x / _hmc5883_Gauss_LSB_XY * SENSORS_GAUSS_TO_MICROTESLA;
  event->magnetic.y = _magData.y / _hmc5883_Gauss_LSB_XY * SENSORS_GAUSS_TO_MICROTESLA;
  event->magnetic.z = _magData.z / _hmc5883_Gauss_LSB_Z * SENSORS_GAUSS_TO_MICROTESLA;
  
  return true;
}

void HMC5883_getSensor(sensor_t *sensor) {
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy (sensor->name, "HMC5883", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name)- 1] = 0;
  sensor->version     = 1;
  sensor->sensor_id   = _sensorID;
  sensor->type        = SENSOR_TYPE_MAGNETIC_FIELD;
  sensor->min_delay   = 0;
  sensor->max_value   = 800; // 8 gauss == 800 microTesla
  sensor->min_value   = -800; // -8 gauss == -800 microTesla
  sensor->resolution  = 0.2; // 2 milligauss == 0.2 microTesla
}
