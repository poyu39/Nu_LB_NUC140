#include "NUC100Series.h"
#include "I2C.h"
#include "MLX90614.h"

#define I2C_PORT I2C0
#define I2C_ADDR MLX90614_I2CADDR

float MLX90614_readObjectTempF(void) {
  return (readTemp(MLX90614_TOBJ1) * 9 / 5) + 32;
}

float MLX90614_readAmbientTempF(void) {
  return (readTemp(MLX90614_TA) * 9 / 5) + 32;
}

float MLX90614_readObjectTempC(void) {
  return readTemp(MLX90614_TOBJ1);
}
 
float MLX90614_readAmbientTempC(void) {
  return readTemp(MLX90614_TA);
}

uint16_t MLX90614_readID(uint8_t id) {
	uint16_t devID;
	if (id==1) devID = read16(MLX90614_ID1);
	if (id==2) devID = read16(MLX90614_ID2);
	if (id==3) devID = read16(MLX90614_ID3);
	if (id==4) devID = read16(MLX90614_ID4);	
  return devID;
}

float readTemp(uint8_t reg) {
  float temp;
  temp = read16(reg);
  temp *= .02;
  temp  -= 273.15;
  return temp;
}

uint16_t read16(uint8_t regAddr) {
  uint16_t ret;
  uint8_t data[3];
	uint8_t length = 3;
	uint8_t pec;
	uint8_t i;

	I2C_SET_CONTROL_REG(I2C_PORT, I2C_I2CON_STA);   //start
	I2C_WAIT_READY(I2C_PORT);
	
	I2C_SET_DATA(I2C_PORT, I2C_ADDR<<1);               //send slave address+W
	I2C_SET_CONTROL_REG(I2C_PORT, I2C_I2CON_SI);
	I2C_WAIT_READY(I2C_PORT);

	I2C_SET_DATA(I2C_PORT, regAddr);                //send index
	I2C_SET_CONTROL_REG(I2C_PORT, I2C_I2CON_SI);
	I2C_WAIT_READY(I2C_PORT);

	I2C_SET_CONTROL_REG(I2C_PORT, I2C_I2CON_STA_SI);//Repeated Start
	I2C_WAIT_READY(I2C_PORT);
	
	I2C_SET_DATA(I2C_PORT, I2C_ADDR<<1 | 0x01);     //send slave address+R
	I2C_SET_CONTROL_REG(I2C_PORT, I2C_I2CON_SI);
	I2C_WAIT_READY(I2C_PORT);						
		
	for (i=0; i<length; i++) {
	  I2C_SET_CONTROL_REG(I2C_PORT, I2C_I2CON_SI_AA);// send Acknowledge
	  I2C_WAIT_READY(I2C_PORT);							
		data[i] = I2C_GET_DATA(I2C_PORT);              //read data   
	}	
  I2C_STOP(I2C_PORT);
	
  ret = data[0] | data[1]<<8;
  pec = data[2]; // PEC (Packet Error Code)
	return ret;
}
