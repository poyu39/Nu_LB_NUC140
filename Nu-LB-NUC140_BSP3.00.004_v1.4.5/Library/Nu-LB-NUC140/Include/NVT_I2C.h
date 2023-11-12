#ifndef _DRVI2C_NVT_H
#define _DRVI2C_NVT_H

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif
#ifndef bool
#define bool unsigned char
#endif

#include <stdint.h>
#define MAX_I2C_DATA_LEN 200
#define I2C_CTL_STA_SI            0x28UL /*!< I2CON setting for I2C control bits. It would set STA and SI bits          */
#define I2C_CTL_STA_SI_AA         0x2CUL /*!< I2CON setting for I2C control bits. It would set STA, SI and AA bits      */
#define I2C_CTL_STO_SI            0x18UL /*!< I2CON setting for I2C control bits. It would set STO and SI bits          */
#define I2C_CTL_STO_SI_AA         0x1CUL /*!< I2CON setting for I2C control bits. It would set STO, SI and AA bits      */
#define I2C_CTL_SI                0x08UL /*!< I2CON setting for I2C control bits. It would set SI bit                   */
#define I2C_CTL_SI_AA             0x0CUL /*!< I2CON setting for I2C control bits. It would set SI and AA bits           */
#define I2C_CTL_STA               0x20UL /*!< I2CON setting for I2C control bits. It would set STA bit                  */
#define I2C_CTL_STO               0x10UL /*!< I2CON setting for I2C control bits. It would set STO bit                  */
#define I2C_CTL_AA                0x04UL /*!< I2CON setting for I2C control bits. It would set AA bit                   */
#define I2C_CTL_STA_STO_SI        0x38UL

void NVT_I2C_Init(uint32_t u32BusClock);
uint8_t NVT_WriteByteContinue_addr8(uint8_t address,uint8_t* data, uint8_t len);
uint8_t NVT_ReadByteContinue_addr8(uint8_t address,uint8_t* data, uint8_t len);
void    NVT_SetDeviceAddress(uint8_t devAddr);
#endif
