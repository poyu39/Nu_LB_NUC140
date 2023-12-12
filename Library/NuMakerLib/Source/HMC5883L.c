/*
HMC5883L.cpp - Class file for the HMC5883L Triple Axis Digital Compass Arduino Library.

Version: 1.1.0
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "HMC5883L.h"

static uint8_t devAddr;
float mgPerDigit;
VecInt16 v;
int xOffset, yOffset;
	
bool HMC5883L_begin(void)
{
	  devAddr= HMC5883L_DEFAULT_ADDRESS;
    HMC5883L_setRange(HMC5883L_RANGE_1_3GA);
    HMC5883L_setMeasurementMode(HMC5883L_CONTINOUS);
    HMC5883L_setDataRate(HMC5883L_DATARATE_15HZ);
    HMC5883L_setSamples(HMC5883L_SAMPLES_1);

    mgPerDigit = 0.92f;

    return true;
}

VecInt16 HMC5883L_readRaw(void)
{
	uint8_t data[2];
	  I2Cdev_readBytes(devAddr, HMC5883L_REG_OUT_X_H, 2, data);
    v.X = (data[0]<<8 | data[1]) - xOffset;
	  I2Cdev_readBytes(devAddr, HMC5883L_REG_OUT_Y_H, 2, data);
    v.Y = (data[0]<<8 | data[1])- yOffset;
	  I2Cdev_readBytes(devAddr, HMC5883L_REG_OUT_Z_H, 2, data);
    v.Z = (data[0]<<8 | data[1]);

    return v;
}

VecInt16 HMC5883L_readNormalize(void)
{
	uint8_t data[2];
	  I2Cdev_readBytes(devAddr, HMC5883L_REG_OUT_X_H, 2, data);
    v.X = ((float)(data[0]<<8 | data[1]) - xOffset) * mgPerDigit;
	  I2Cdev_readBytes(devAddr, HMC5883L_REG_OUT_Y_H, 2, data);
    v.Y = ((float)(data[0]<<8 | data[1]) - yOffset) * mgPerDigit;
	  I2Cdev_readBytes(devAddr, HMC5883L_REG_OUT_Z_H, 2, data);
    v.Z = (float)(data[0]<<8 | data[1]) * mgPerDigit;
    return v;
}

void HMC5883L_setOffset(int xo, int yo)
{
    xOffset = xo;
    yOffset = yo;
}

void HMC5883L_setRange(hmc5883l_range_t range)
{
    switch(range)
    {
	case HMC5883L_RANGE_0_88GA:
	    mgPerDigit = 0.073f;
	    break;

	case HMC5883L_RANGE_1_3GA:
	    mgPerDigit = 0.92f;
	    break;

	case HMC5883L_RANGE_1_9GA:
	    mgPerDigit = 1.22f;
	    break;

	case HMC5883L_RANGE_2_5GA:
	    mgPerDigit = 1.52f;
	    break;

	case HMC5883L_RANGE_4GA:
	    mgPerDigit = 2.27f;
	    break;

	case HMC5883L_RANGE_4_7GA:
	    mgPerDigit = 2.56f;
	    break;

	case HMC5883L_RANGE_5_6GA:
	    mgPerDigit = 3.03f;
	    break;

	case HMC5883L_RANGE_8_1GA:
	    mgPerDigit = 4.35f;
	    break;

	default:
	    break;
    }

    I2Cdev_writeByte(devAddr, HMC5883L_REG_CONFIG_B, range << 5);
}

hmc5883l_range_t HMC5883L_getRange(void)
{
	uint8_t value;
	  I2Cdev_readByte(devAddr,HMC5883L_REG_CONFIG_B, &value);
    return (hmc5883l_range_t)(value >> 5);
}

void HMC5883L_setMeasurementMode(hmc5883l_mode_t mode)
{
    uint8_t value;
    I2Cdev_readByte(devAddr, HMC5883L_REG_MODE, &value);
    value &= 0xFC;//0b11111100;
    value |= mode;

    I2Cdev_writeByte(devAddr, HMC5883L_REG_MODE, value);
}

hmc5883l_mode_t HMC5883L_getMeasurementMode(void)
{
    uint8_t value;

    I2Cdev_readByte(devAddr, HMC5883L_REG_MODE, &value);
    value &= 0x03; //0b00000011;

    return (hmc5883l_mode_t)value;
}

void HMC5883L_setDataRate(hmc5883l_dataRate_t dataRate)
{
    uint8_t value;

    I2Cdev_readByte(devAddr, HMC5883L_REG_CONFIG_A, &value);
    value &= 0xE3;//0b11100011;
    value |= (dataRate << 2);

    I2Cdev_writeByte(devAddr, HMC5883L_REG_CONFIG_A, value);
}

hmc5883l_dataRate_t HMC5883L_getDataRate(void)
{
    uint8_t value;

    I2Cdev_readByte(devAddr, HMC5883L_REG_CONFIG_A, &value);
    value &= 0x1C;//0b00011100;
    value >>= 2;

    return (hmc5883l_dataRate_t)value;
}

void HMC5883L_setSamples(hmc5883l_samples_t samples)
{
    uint8_t value;

    I2Cdev_readByte(devAddr, HMC5883L_REG_CONFIG_A, &value);
    value &= 0x9F;//0b10011111;
    value |= (samples << 5);

    I2Cdev_writeByte(devAddr, HMC5883L_REG_CONFIG_A, value);
}

hmc5883l_samples_t HMC5883L_getSamples(void)
{
    uint8_t value;

    I2Cdev_readByte(devAddr, HMC5883L_REG_CONFIG_A, &value);
    value &= 0xc0; //0b01100000;
    value >>= 5;

    return (hmc5883l_samples_t)value;
}
