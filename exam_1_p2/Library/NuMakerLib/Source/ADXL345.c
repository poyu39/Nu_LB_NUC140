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

void ADXL345_WriteByte(uint8_t ADXL345_reg, uint8_t ADXL345_data)
{
  uint8_t data[1];
    data[0]=ADXL345_data;
    I2C_writeBytes(ADXL345_I2C_PORT, ADXL345_I2C_SLA, ADXL345_reg, 1, data);
}

uint8_t ADXL345_ReadByte(uint8_t ADXL345_reg)
{
  uint8_t data[1];
    I2C_readBytes(ADXL345_I2C_PORT, ADXL345_I2C_SLA, ADXL345_reg, 1, data);	
    return (data[0]);
}

void Init_ADXL345(void)
{
    ADXL345_WriteByte(ADXL345_DATA_FORMAT,0x0B); 
    ADXL345_WriteByte(ADXL345_BW_RATE,0x0F);
    ADXL345_WriteByte(ADXL345_POWER_CTL,0x08);
    ADXL345_WriteByte(ADXL345_INT_ENABLE,0x80);
    ADXL345_WriteByte(ADXL345_OFSX,0x00);
    ADXL345_WriteByte(ADXL345_OFSY,0x00);
    ADXL345_WriteByte(ADXL345_OFSZ,0x05);
}

uint16_t Read_ADXL345_DataX(void)
{
    uint8_t  LoByte, HiByte;
    LoByte=ADXL345_ReadByte(ADXL345_DATAX0);
    HiByte=ADXL345_ReadByte(ADXL345_DATAX1);
    return((HiByte<<8)|LoByte);
}

uint16_t Read_ADXL345_DataY(void)
{
    uint8_t  LoByte, HiByte;
    LoByte=ADXL345_ReadByte(ADXL345_DATAY0);
    HiByte=ADXL345_ReadByte(ADXL345_DATAY1);
    return((HiByte<<8)|LoByte);
}

uint16_t Read_ADXL345_DataZ(void)
{
    uint8_t  LoByte, HiByte;
    LoByte=ADXL345_ReadByte(ADXL345_DATAY0);
    HiByte=ADXL345_ReadByte(ADXL345_DATAY1);
    return((HiByte<<8)|LoByte);
}
