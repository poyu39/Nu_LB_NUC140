//
// I2C slave device read/write access
//
#include <stdio.h>
#include "NUC100Series.h"
#include "I2Cdev.h"

void I2C_readBytes(I2C_T *i2c, uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) 
{
	uint8_t i, tmp;
	      I2C_START(i2c);                         //Start
	      I2C_WAIT_READY(i2c);
	
	      I2C_SET_DATA(i2c, devAddr);         //send slave address+W
	      I2C_SET_CONTROL_REG(i2c, I2C_SI);
	      I2C_WAIT_READY(i2c);

	      I2C_SET_DATA(i2c, regAddr);        //send index
	      I2C_SET_CONTROL_REG(i2c, I2C_SI);
	      I2C_WAIT_READY(i2c);
	
	      I2C_SET_CONTROL_REG(i2c, I2C_STA | I2C_SI);	//Start
	      I2C_WAIT_READY(i2c);

		    I2C_SET_DATA(i2c, (devAddr+1));    //send slave address+R
	      I2C_SET_CONTROL_REG(i2c, I2C_SI);
	      I2C_WAIT_READY(i2c);							
	
	      for (i=0; i<length; i++) {
	      I2C_SET_CONTROL_REG(i2c, I2C_SI);
	      I2C_WAIT_READY(i2c);							
				tmp = I2C_GET_DATA(i2c);           //read data   
				data[i]=tmp;
				}
				I2C_STOP(i2c);										 //Stop
}

void I2C_writeBytes(I2C_T *i2c, uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) 
{
	uint8_t i;
	uint32_t tmp;
        I2C_START(i2c);                    //Start
	      I2C_WAIT_READY(i2c);
	
	      I2C_SET_DATA(i2c, devAddr);        //send slave address
	      I2C_SET_CONTROL_REG(i2c, I2C_SI);
	      I2C_WAIT_READY(i2c);

	      I2C_SET_DATA(i2c, regAddr);        //send index
	      I2C_SET_CONTROL_REG(i2c, I2C_SI);
	      I2C_WAIT_READY(i2c);	

	      for (i=0; i<length; i++) {
				tmp = data[i];
	      I2C_SET_DATA(i2c, tmp);            //send Data
	      I2C_SET_CONTROL_REG(i2c, I2C_SI);
	      I2C_WAIT_READY(i2c);
				}
				
				I2C_STOP(i2c);										 //Stop
}

