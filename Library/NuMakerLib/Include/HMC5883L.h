/*
HMC5883L.h - Header file for the HMC5883L Triple Axis Digital Compass Arduino Library.

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
#ifndef HMC5883L_h
#define HMC5883L_h

#include "I2Cdev.h"
#include <stdbool.h>

#define HMC5883L_DEFAULT_ADDRESS      0x1E
#define HMC5883L_REG_CONFIG_A         0x00
#define HMC5883L_REG_CONFIG_B         0x01
#define HMC5883L_REG_MODE             0x02
#define HMC5883L_REG_OUT_X_H          0x03
#define HMC5883L_REG_OUT_X_L          0x04
#define HMC5883L_REG_OUT_Z_H          0x05
#define HMC5883L_REG_OUT_Z_L          0x06
#define HMC5883L_REG_OUT_Y_H          0x07
#define HMC5883L_REG_OUT_Y_L          0x08
#define HMC5883L_REG_STATUS           0x09
#define HMC5883L_REG_IDENT_A          0x0A
#define HMC5883L_REG_IDENT_B          0x0B
#define HMC5883L_REG_IDENT_C          0x0C

typedef enum
{
    HMC5883L_SAMPLES_8     = 3,//0b11,
    HMC5883L_SAMPLES_4     = 2,//0b10,
    HMC5883L_SAMPLES_2     = 1,//0b01,
    HMC5883L_SAMPLES_1     = 0 //0b00
} hmc5883l_samples_t;

typedef enum
{
    HMC5883L_DATARATE_75HZ       = 6,//0b110,
    HMC5883L_DATARATE_30HZ       = 5,//0b101,
    HMC5883L_DATARATE_15HZ       = 4,//0b100,
    HMC5883L_DATARATE_7_5HZ      = 3,//0b011,
    HMC5883L_DATARATE_3HZ        = 2,//0b010,
    HMC5883L_DATARATE_1_5HZ      = 1,//0b001,
    HMC5883L_DATARATE_0_75_HZ    = 0 //0b000
} hmc5883l_dataRate_t;

typedef enum
{
    HMC5883L_RANGE_8_1GA     = 7,//0b111,
    HMC5883L_RANGE_5_6GA     = 6,//0b110,
    HMC5883L_RANGE_4_7GA     = 5,//0b101,
    HMC5883L_RANGE_4GA       = 4,//0b100,
    HMC5883L_RANGE_2_5GA     = 3,//0b011,
    HMC5883L_RANGE_1_9GA     = 2,//0b010,
    HMC5883L_RANGE_1_3GA     = 1,//0b001,
    HMC5883L_RANGE_0_88GA    = 0 //0b000
} hmc5883l_range_t;

typedef enum
{
    HMC5883L_IDLE          = 2,//0b10,
    HMC5883L_SINGLE        = 1,//0b01,
    HMC5883L_CONTINOUS     = 0 //0b00
} hmc5883l_mode_t;

typedef struct
{
    int16_t X;
    int16_t Y;
    int16_t Z;
}VecInt16;

	bool HMC5883L_begin(void);

	VecInt16 HMC5883L_readRaw(void);
	VecInt16 HMC5883L_readNormalize(void);

	void  HMC5883L_setOffset(int xo, int yo);

	void  HMC5883L_setRange(hmc5883l_range_t range);
	hmc5883l_range_t HMC5883L_getRange(void);

	void  HMC5883L_setMeasurementMode(hmc5883l_mode_t mode);
	hmc5883l_mode_t HMC5883L_getMeasurementMode(void);

	void  HMC5883L_setDataRate(hmc5883l_dataRate_t dataRate);
	hmc5883l_dataRate_t HMC5883L_getDataRate(void);

	void  HMC5883L_setSamples(hmc5883l_samples_t samples);
	hmc5883l_samples_t HMC5883L_getSamples(void);

#endif
