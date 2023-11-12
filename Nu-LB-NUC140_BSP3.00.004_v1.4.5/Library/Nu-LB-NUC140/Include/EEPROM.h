// Global variables

typedef void (*I2C_FUNC)(uint32_t u32Status);
static I2C_FUNC s_I2C1HandlerFn = NULL;

void I2C_MasterRx(uint32_t u32Status);
void I2C_MasterTx(uint32_t u32Status);
void Close_EEPROM(void);
void Init_EEPROM(void);
void EEPROM_Write(uint16_t addr, uint8_t data);
uint8_t EEPROM_Read(uint16_t addr);
