
#include <stdio.h>
#include <stdbool.h>
#include "NVT_I2C.h"
#include "NUC100Series.h"

int8_t I2Cdev_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) 
{
	uint8_t ErrorFlag;
  
	NVT_SetDeviceAddress(devAddr);
	ErrorFlag = NVT_ReadByteContinue_addr8(regAddr,data, length);
  #ifdef I2CDEV_SERIAL_DEBUG
	Serial.print(". Done (");
	Serial.print(count, DEC);
	Serial.println(" read).");
	#endif
	if(ErrorFlag)
		return 0;
	else
		return 1;
}

int8_t I2Cdev_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data)
{
	NVT_SetDeviceAddress(devAddr);
	NVT_ReadByteContinue_addr8(regAddr, data, 1);
	return 1;
}

int8_t I2Cdev_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) 
{
	uint8_t count, b;
	
	if ((count = I2Cdev_readByte(devAddr, regAddr, &b)) != 0) {
		uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
		b &= mask;
		b >>= (bitStart - length + 1);
		*data = b;
	}
	return count;
}

int8_t I2Cdev_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data) 
{
	uint8_t b;
	uint8_t count = I2Cdev_readByte(devAddr, regAddr, &b);
	*data = b & (1 << bitNum);
	return count;
}

int8_t I2Cdev_readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data) 
{
	return 0;
}

int8_t I2Cdev_readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data) 
{
	return I2Cdev_readWords(devAddr, regAddr, 1, data);
}

int8_t I2Cdev_readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data) 
{
	uint16_t b = 0;
	uint8_t count = I2Cdev_readWord(devAddr, regAddr, &b);
	*data = b & (1 << bitNum);
	return count;
}

int8_t I2Cdev_readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data) 
{
	uint8_t count;
	uint16_t w;
	
	if ((count = I2Cdev_readWord(devAddr, regAddr, &w)) != 0) {
		uint16_t mask = ((1 << length) - 1) << (bitStart - length + 1);
		w &= mask;
		w >>= (bitStart - length + 1);
		*data = w;
	}
	return count;
}

bool I2Cdev_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) 
{
	NVT_SetDeviceAddress(devAddr);
	NVT_WriteByteContinue_addr8(regAddr,data, length);
	return 1;
}

bool I2Cdev_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) 
{
	return I2Cdev_writeBytes(devAddr, regAddr, 1, &data);
}

bool I2Cdev_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) 
{
	uint8_t b;
	I2Cdev_readByte(devAddr, regAddr, &b);
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	return I2Cdev_writeByte(devAddr, regAddr, b);
}

bool I2Cdev_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) 
{
	uint8_t b;
	uint8_t mask;

	if (I2Cdev_readByte(devAddr, regAddr, &b) != 0) {
		mask = ((1 << length) - 1) << (bitStart - length + 1);
		data <<= (bitStart - length + 1); 
		data &= mask; 
		b &= ~(mask);
		b |= data;
		return I2Cdev_writeByte(devAddr, regAddr, b);

	} else {
		return false;
	}
}

bool I2Cdev_writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t* data) 
{
	return false;
}

bool I2Cdev_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data)
{
	return I2Cdev_writeWords(devAddr, regAddr, 1, &data);
}

bool I2Cdev_writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data) 
{
	uint16_t w;
	I2Cdev_readWord(devAddr, regAddr, &w);
	w = (data != 0) ? (w | (1 << bitNum)) : (w & ~(1 << bitNum));
	
	return I2Cdev_writeWord(devAddr, regAddr, w);
}

bool I2Cdev_writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data) 
{
	return false;
}
