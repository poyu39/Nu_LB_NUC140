//
// ADXL345 : Analog Device 3-axis accelerometer
//
// Interface: I2C/SPI (CS Hi/Lo)
// pin1: Gnd to Vss  (NUC123 pin15)
// pin2: Vcc to Vdd
// pin3: CS  to Vdd          
// pin4: INT1-- N.C.
// pin5: INT2-- N.C.
// pin6: SDO to Gnd  (ALT_Address =0)
// pin7: SDA to SDA
// pin8: SCL to SCL

#include <stdio.h>
#include <NUC100Series.h>
#include "I2Cdev.h"
#include "ADXL345.h"

void ADXL345_Init(void)
{
    I2Cdev_writeByte(ADXL345_devAddr, ADXL345_DATA_FORMAT,0x0B); 
    I2Cdev_writeByte(ADXL345_devAddr, ADXL345_BW_RATE,0x0F);
    I2Cdev_writeByte(ADXL345_devAddr, ADXL345_POWER_CTL,0x08);
    I2Cdev_writeByte(ADXL345_devAddr, ADXL345_INT_ENABLE,0x80);
    I2Cdev_writeByte(ADXL345_devAddr, ADXL345_OFSX,0x00);
    I2Cdev_writeByte(ADXL345_devAddr, ADXL345_OFSY,0x00);
    I2Cdev_writeByte(ADXL345_devAddr, ADXL345_OFSZ,0x05);
}

uint16_t ADXL345_ReadAccelX(void)
{
    uint8_t  LoByte, HiByte;
    I2Cdev_readByte(ADXL345_devAddr, ADXL345_DATAX0, LoByte);
    I2Cdev_readByte(ADXL345_devAddr, ADXL345_DATAX1, HiByte);	
    return((HiByte<<8)|LoByte);
}

uint16_t ADXL345_ReadAccelY(void)
{
    uint8_t  LoByte, HiByte;
    I2Cdev_readByte(ADXL345_devAddr, ADXL345_DATAY0, LoByte);
    I2Cdev_readByte(ADXL345_devAddr, ADXL345_DATAY1, HiByte);
    return((HiByte<<8)|LoByte);
}

uint16_t ADXL345_ReadAccelZ(void)
{
    uint8_t  LoByte, HiByte;
    I2Cdev_readByte(ADXL345_devAddr, ADXL345_DATAZ0, LoByte);
    I2Cdev_readByte(ADXL345_devAddr, ADXL345_DATAZ1, HiByte);
    return((HiByte<<8)|LoByte);
}
