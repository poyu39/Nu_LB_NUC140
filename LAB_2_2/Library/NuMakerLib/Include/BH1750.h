//
// BH1750 : Ambient Light Sensor
// 
#define BH1750_I2C_SLA          0xB8
#define BH1750_I2C_PORT         I2C1

// Instruction Set 
#define Power_Down                      0x00
#define Power_On                        0x01
#define Reset                           0x07
#define Continuously_H_ResolutionMode   0x10
#define Continuously_H_ResolutionMode2  0x11
#define Continuously_L_ResolutionMode   0x13
#define OneTime_H_ResolutionMode        0x20
#define OneTime_H_ResolutionMode2       0x21
#define OneTime_L_ResolutionMode        0x23
#define ChangeMeasurementTime_H         0x40
#define ChangeMeasurementTime_L         0x60

extern void init_BH1750(void);

extern void BH1750_Write(uint8_t CODE);
	
extern uint16_t BH1750_Read(void);
