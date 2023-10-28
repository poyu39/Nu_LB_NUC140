//
// HTU21D : Digital Relative Humidity Sensor with Temperature Output
// 
#define HTU21D_I2C_PORT      I2C1
#define HTU21D_I2C_SLA       0x80

#define HTU21D_TriggerTempHold    0xE3 // Hold I2C master
#define HTU21D_TriggerHumidHold   0xE5 // Hold I2C mater
#define HTU21D_TriggerTempnoHold  0xF3 // no Hold I2C master
#define HTU21D_TriggerHumidnoHold 0xF5 // no Hold I2C master
#define HTU21D_WriteUserregister  0xF6
#define HTU21D_ReadUserregister   0xF7
#define HTU21D_SoftReset          0xFE

extern uint8_t  HTU21D_Init(void);
extern uint8_t  HTU21D_ReadHumid(void);
extern uint16_t HTU21D_ReadTemp(void);
extern float    HTU21D_Humidity(void);
extern float    HTU21D_Temperature(void);
