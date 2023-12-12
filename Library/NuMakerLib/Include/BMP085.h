//
// BMP085 : Digital Pressure Sensor
// 
#define BMP085_devAddr    0x77

#define BMP085_AC1        0xAA
#define BMP085_AC2        0xAC
#define BMP085_AC3        0xAE
#define BMP085_AC4        0xB0
#define BMP085_AC5        0xB2
#define BMP085_AC6        0xB4
#define BMP085_B1         0xB6
#define BMP085_B2         0xB8
#define BMP085_MB         0xBA
#define BMP085_MC         0xBC
#define BMP085_MD         0xBE

#define BMP085_CR         0xF4
#define BMP085_MSB        0xF6
#define BMP085_LSB        0xF7
#define BMP085_XSB        0xF8

extern void BMP085_Calibration(void);
	
extern uint16_t BMP085_ReadUT(void);

extern uint32_t BMP085_ReadUP(void);
	
extern short BMP085_GetTemperature(uint16_t ut);

extern long BMP085_GetPressure(uint32_t up);
