#include "NUC100Series.h"

#define W25Q16_SPI_PORT SPI2

#define W25Q16_WRITE_ENABLE 0x06
#define W25Q16_WRITE_ENABLE_FOR_VOLATILE_STATUS_REGISTER 0x50
#define W25Q16_WRITE_DISABLE 0x04
#define W25Q16_READ_STATUS_REGISTER1 0x05
#define W25Q16_READ_STATUS_REGISTER2 0x35
#define W25Q16_WRITE_STATUS_REGISTER 0x01
#define W25Q16_PAGE_PROGRAM 0x02
#define W25Q16_QUAD_PAGE_PROGRAM 0x32
#define W25Q16_SECTOR_ERASE 0x20
#define W25Q16_BLOCK_ERASE_32KB 0x52
#define W25Q16_BLOCK_ERASE_64KB 0xD8
#define W25Q16_CHIP_ERASE 0x60  // or 0xC7
#define W25Q16_ERASE_SUSPEND 0x75
#define W25Q16_ERASE_RESUME  0x7A
#define W25Q16_POWER_DOWN 0xB9
#define W25Q16_CONTINUOUS_READ_MODE_RESET 0xFF

#define W25Q16_READ_DATA 0x03
#define W25Q16_FAST_READ 0x0B
#define W25Q16_FAST_READ_DUAL_OUTPUT 0x3B
#define W25Q16_FAST_READ_QUAD_OUTPUT 0x6B
#define W25Q16_FAST_READ_DUAL_IO 0xBB
#define W25Q16_FAST_READ_QUAD_IO 0xEB
#define W25Q16_WORD_READ_QUAD_IO 0xE7
#define W25Q16_QCTAL_WORD_READ_QUAD_IO 0xE3
#define W25Q16_SET_BURST_W_WRAP 0x77

#define W25Q16_RELEASE_PWRDN_DEVICE_ID 0xAB
#define W25Q16_MANUFACTURER_DEVICE_ID 0x90
#define W25Q16_MANUFACTURER_DEVICE_ID_DUAL_IO 0x92
#define W25Q16_MANUFACTURER_DEVICE_ID_QUAD_IO 0x94
#define W25Q16_READ_JEDEC_ID 0x9F
#define W25Q16_READ_UNIQUE_ID 0x4B
#define W25Q16_READ_SFDP_REGISTER 0x5A
#define W25Q16_ERASE_SECURITY_REGISTERS 0x44
#define W25Q16_PROGRAM_SECURITY_REGISTERS 0x42
#define W25Q16_READ_SECURITY_REGISTERS 0x48
#define W25Q16_ENABLE_RESET 0x66
#define W25Q16_RESET 0x99

extern void init_W25Q16(void);
void W25Q16_Command(uint8_t command);
extern void W25Q16_WriteEnable(void);
extern void W25Q16_WriteEnableForVolatileStatusRegister(void);
extern void W25Q16_WriteDisable(void);
extern uint8_t W25Q16_ReadStatusRegister1(void);
extern uint8_t W25Q16_ReadStatusRegister2(void);
extern void W25Q16_WriteStatusRegister(uint16_t u16data);
extern void W25Q16_PageProgram(uint32_t u24addr, uint8_t *data);
extern void W25Q16_SectorErase(uint32_t u24addr);
extern void W25Q16_BlockErase32KB(uint32_t u24addr);
extern void W25Q16_BlockErase64KB(uint32_t u24addr);
extern void W25Q16_ChipErase(void);
extern void W25Q16_EraseSuspend(void);
extern void W25Q16_EraseResume(void);
extern void W25Q16_PowerDown(void);
extern void W25Q16_ContinuouseReadModeReset(void);
extern uint8_t W25Q16_ReadData(uint32_t u24addr);
extern uint8_t W25Q16_ReleasePowerDown_DeviceID(void);
extern uint16_t W25Q16_ReadManufacturerDeviceID(void);
extern uint32_t W25Q16_ReadJEDECID(void);
extern uint8_t W25Q16_ReadUniqueID(void);
extern uint8_t W25Q16_ReadSFDPRegister(uint8_t addr);
extern void W25Q16_EraseSecurityRegisters(uint32_t u24addr);
extern void W25Q16_ProgramSecurityRegiters(uint32_t u24addr, uint8_t data);
extern uint8_t W25Q16_ReadSecurityRegisters(uint32_t u24addr);
