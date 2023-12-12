//
// HTU21D : Digital Relative Humidity Sensor with Temperature Output
// 
#include "NVT_I2C.h"

/** Default I2C address for the HTU21D. */
#define HTU21DF_I2CADDR         (0x40)

/** Read temperature register. */
#define HTU21DF_READTEMP        (0xE3)

/** Read humidity register. */
#define HTU21DF_READHUM         (0xE5)

/** Write register command. */
#define HTU21DF_WRITEREG        (0xE6)

/** Read register command. */
#define HTU21DF_READREG         (0xE7)

/** Reset command. */
#define HTU21DF_RESET           (0xFE)

extern bool  HTU21DF_begin(void);
extern void  HTU21DF_reset(void);
extern uint8_t  HTU21DF_ReadHumid(void);
extern uint16_t HTU21DF_ReadTemp(void);
extern float    HTU21DF_Humidity(void);
extern float    HTU21DF_Temperature(void);
