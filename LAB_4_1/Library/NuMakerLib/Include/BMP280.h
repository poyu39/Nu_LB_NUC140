//
// BMP280.h : Barometer Device Driver
// 
#define BMP280_I2C_SLA  0xEE 	// SDO tied to Vdd
#define BMP280_I2C_PORT I2C1

#define BMP280_REG_TEMP_XLSB            0xFC
#define BMP280_REG_TEMP_LSB             0xFB
#define BMP280_REG_TEMP_MSB             0xFA
#define BMP280_REG_PRESS_XLSB           0xF9
#define BMP280_REG_PRESS_LSB            0xF8
#define BMP280_REG_PRESS_MSB            0xF7
#define BMP280_REG_CONFIG               0xF5
#define BMP280_REG_CTRL_MEAS            0xF4
#define BMP280_REG_STATUS               0xF3
#define BMP280_REG_RESET                0xE0
#define BMP280_REG_ID                   0xD0

#define BMP280_REG_COMP_TEMP_START      0x88
#define BMP280_COMP_TEMP_REG_COUNT      6

#define BMP280_REG_COMP_PRESS_START     0x8E
#define BMP280_COMP_PRESS_REG_COUNT     18

#define BMP280_CHIP_ID                  0x58
#define BMP280_SOFT_RESET_VAL           0xB6
