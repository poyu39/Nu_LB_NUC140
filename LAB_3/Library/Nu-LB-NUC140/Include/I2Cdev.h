#define I2C_STA_SI            0x28UL /*!< I2CON setting for I2C control bits. It would set STA and SI bits          */
#define I2C_STA_SI_AA         0x2CUL /*!< I2CON setting for I2C control bits. It would set STA, SI and AA bits      */
#define I2C_STO_SI            0x18UL /*!< I2CON setting for I2C control bits. It would set STO and SI bits          */
#define I2C_STO_SI_AA         0x1CUL /*!< I2CON setting for I2C control bits. It would set STO, SI and AA bits      */
#define I2C_SI                0x08UL /*!< I2CON setting for I2C control bits. It would set SI bit                   */
#define I2C_SI_AA             0x0CUL /*!< I2CON setting for I2C control bits. It would set SI and AA bits           */
#define I2C_STA               0x20UL /*!< I2CON setting for I2C control bits. It would set STA bit                  */
#define I2C_STO               0x10UL /*!< I2CON setting for I2C control bits. It would set STO bit                  */
#define I2C_AA                0x04UL /*!< I2CON setting for I2C control bits. It would set AA bit                   */

extern void I2C_readBytes(I2C_T *i2c, uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);

extern void I2C_writeBytes(I2C_T *i2c, uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
