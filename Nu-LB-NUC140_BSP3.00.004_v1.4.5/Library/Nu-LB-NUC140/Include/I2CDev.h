#ifndef _I2CAPI_H
#define _I2CAPI_H

#include "NVT_I2C.h"

int8_t I2Cdev_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
int8_t I2Cdev_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data);
int8_t I2Cdev_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
int8_t I2Cdev_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data);
int8_t I2Cdev_readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);
bool I2Cdev_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data);
bool I2Cdev_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
bool I2Cdev_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
bool I2Cdev_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
bool I2Cdev_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
#endif
