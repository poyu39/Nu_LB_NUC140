//
// LM75A : Temperature Sensor
// 
#define LM75A_devAddr          0x4F // 0x48~4F set by pin A2,A1,A0
// LM75A Internal Registers 
#define LM75A_TEMP             0x00 // read only
#define LM75A_CONF             0x01 // R/W
#define LM75A_THYST            0x02 // R/W
#define LM75A_TOS              0x03 // R/W

extern void Init_LM75A(void);
extern uint8_t  LM75A_ReadConfig(void);
extern uint16_t LM75A_ReadTemp(void);
