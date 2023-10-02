//
// For NUC140
//
#ifndef __SYS_init_H__
#define __SYS_init_H__

   #ifdef MCU_CLOCK_SOURCE_HXT
     #define MCU_CLOCK_SOURCE_MASK_HXT  CLK_PWRCON_XTL12M_EN_Msk
     #define CLK_CLKSTATUS_STB_MASK_HXT CLK_CLKSTATUS_XTL12M_STB_Msk
   #else
     #define MCU_CLOCK_SOURCE_MASK_HXT 0
     #define CLK_CLKSTATUS_STB_MASK_HXT 0
   #endif
   #ifdef MCU_CLOCK_SOURCE_LXT
     #define MCU_CLOCK_SOURCE_MASK_LXT  CLK_PWRCON_XTL32K_EN_Msk
     #define CLK_CLKSTATUS_STB_MASK_LXT CLK_CLKSTATUS_IRC10K_STB_Msk
   #else
     #define MCU_CLOCK_SOURCE_MASK_LXT 0
     #define CLK_CLKSTATUS_STB_MASK_LXT 0
   #endif
   #ifdef MCU_CLOCK_SOURCE_HIRC
     #define MCU_CLOCK_SOURCE_MASK_HIRC  CLK_PWRCON_IRC22M_EN_Msk
     #define CLK_CLKSTATUS_STB_MASK_HIRC CLK_CLKSTATUS_IRC22M_STB_Msk
   #else
     #define MCU_CLOCK_SOURCE_MASK_HIRC 0
     #define CLK_CLKSTATUS_STB_MASK_HIRC 0
   #endif
   #ifdef MCU_CLOCK_SOURCE_LIRC
     #define MCU_CLOCK_SOURCE_MASK_LIRC  CLK_PWRCON_IRC10K_EN_Msk
     #define CLK_CLKSTATUS_STB_MASK_LIRC CLK_CLKSTATUS_IRC10K_STB_Msk
   #else
     #define MCU_CLOCK_SOURCE_MASK_LIRC 0
     #define CLK_CLKSTATUS_STB_MASK_LIRC 0
   #endif 
   #ifdef MCU_CLOCK_SOURCE
     #define MCU_CLOCK_SOURCE_MASK (MCU_CLOCK_SOURCE_MASK_HXT | MCU_CLOCK_SOURCE_MASK_LXT | MCU_CLOCK_SOURCE_MASK_HIRC | MCU_CLOCK_SOURCE_MASK_LIRC)
     #define MCU_CLOCK_STABLE_MASK (CLK_CLKSTATUS_STB_MASK_HXT | CLK_CLKSTATUS_STB_MASK_LXT | CLK_CLKSTATUS_STB_MASK_HIRC | CLK_CLKSTATUS_STB_MASK_LIRC)
   #endif

// For CLK.h compatible to Nano BSP
#define CLK_HCLK_CLK_DIVIDER(x)  ((x)-1)                             /*!< CLKDIV Setting for HCLK clock divider. It could be 1~16 */
#define CLK_USB_CLK_DIVIDER(x)  (((x)-1) << CLK_CLKDIV_USB_N_Pos)    /*!< CLKDIV Setting for USB clock divider. It could be 1~16  */
#define CLK_UART_CLK_DIVIDER(x) (((x)-1) << CLK_CLKDIV_UART_N_Pos)   /*!< CLKDIV Setting for UART clock divider. It could be 1~16 */
#define CLK_ADC_CLK_DIVIDER(x)  (((x)-1) << CLK_CLKDIV_ADC_N_Pos)    /*!< CLKDIV Setting for ADC clock divider. It could be 1~256 */
#define CLK_SC0_CLK_DIVIDER(x)  (((x)-1) << CLK_CLKDIV1_SC0_N_Pos)  /*!< CLKDIV1 Setting for SC0 clock divider. It could be 1~256*/
#define CLK_SC1_CLK_DIVIDER(x)  (((x)-1) << CLK_CLKDIV1_SC1_N_Pos)  /*!< CLKDIV1 Setting for SC1 clock divider. It could be 1~256*/
#define CLK_SC2_CLK_DIVIDER(x)  (((x)-1) << CLK_CLKDIV1_SC2_N_Pos)  /*!< CLKDIV1 Setting for SC2 clock divider. It could be 1~256*/

// For GPIO.h compatible to Nano BSP
#define GPIO_MODE_INPUT          0x0UL /*!< Input Mode */
#define GPIO_MODE_OUTPUT         0x1UL /*!< Output Mode */
#define GPIO_MODE_OPEN_DRAIN     0x2UL /*!< Open-Drain Mode */
#define GPIO_MODE_QUASI          0x3UL /*!< Quasi-bidirectional Mode */

// For ADC.h compatbile to Nano BSP
#define ADC_CH_0_MASK                    (1UL << 0)  /*!< ADC channel 0 mask */
#define ADC_CH_1_MASK                    (1UL << 1)  /*!< ADC channel 1 mask */
#define ADC_CH_2_MASK                    (1UL << 2)  /*!< ADC channel 2 mask */
#define ADC_CH_3_MASK                    (1UL << 3)  /*!< ADC channel 3 mask */
#define ADC_CH_4_MASK                    (1UL << 4)  /*!< ADC channel 4 mask */
#define ADC_CH_5_MASK                    (1UL << 5)  /*!< ADC channel 5 mask */
#define ADC_CH_6_MASK                    (1UL << 6)  /*!< ADC channel 6 mask */
#define ADC_CH_7_MASK                    (1UL << 7)  /*!< ADC channel 7 mask */
#define ADC_OPERATION_MODE_SINGLE       (0UL<<ADC_ADCR_ADMD_Pos)    /*!< Single mode                */
#define ADC_OPERATION_MODE_SINGLE_CYCLE (2UL<<ADC_ADCR_ADMD_Pos)    /*!< Single cycle scan mode     */
#define ADC_OPERATION_MODE_CONTINUOUS   (3UL<<ADC_ADCR_ADMD_Pos)    /*!< Continuous scan mode       */
#define ADC_INPUT_MODE_SINGLE_END      (0UL<<ADC_ADCR_DIFFEN_Pos)   /*!< Single end input mode      */
#define ADC_INPUT_MODE_DIFFERENTIAL    (1UL<<ADC_ADCR_DIFFEN_Pos)   /*!< Differential input type    */

// For PWM.h compatible to Nano BSP
#define PWM0_IRQn PWMA_IRQn
#define PWM1_IRQn PWMB_IRQn
#define PWM_CH_0_MASK                       (1UL)    /*!< PWM channel 0 mask  \hideinitializer */
#define PWM_CH_1_MASK                       (2UL)    /*!< PWM channel 1 mask  \hideinitializer */
#define PWM_CH_2_MASK                       (4UL)    /*!< PWM channel 2 mask  \hideinitializer */
#define PWM_CH_3_MASK                       (8UL)    /*!< PWM channel 3 mask  \hideinitializer */

// For I2C.h compatible to Nano BSP
#define I2C_STA_SI            0x28UL /*!< I2CON setting for I2C control bits. It would set STA and SI bits          */
#define I2C_STA_SI_AA         0x2CUL /*!< I2CON setting for I2C control bits. It would set STA, SI and AA bits      */
#define I2C_STO_SI            0x18UL /*!< I2CON setting for I2C control bits. It would set STO and SI bits          */
#define I2C_STO_SI_AA         0x1CUL /*!< I2CON setting for I2C control bits. It would set STO, SI and AA bits      */
#define I2C_SI                0x08UL /*!< I2CON setting for I2C control bits. It would set SI bit                   */
#define I2C_SI_AA             0x0CUL /*!< I2CON setting for I2C control bits. It would set SI and AA bits           */
#define I2C_STA               0x20UL /*!< I2CON setting for I2C control bits. It would set STA bit                  */
#define I2C_STO               0x10UL /*!< I2CON setting for I2C control bits. It would set STO bit                  */
#define I2C_AA                0x04UL /*!< I2CON setting for I2C control bits. It would set AA bit                   */

// For RTC.h
#define CLK_CLKSEL2_RTC_SEL_10K_LXT     (0x0UL<<CLK_CLKSEL2_RTC_SEL_10K_Pos)   /*!< Setting WWDT clock source as external X'tal 32.768KHz */
#define CLK_CLKSEL2_RTC_SEL_10K_LIRC    (0x1UL<<CLK_CLKSEL2_RTC_SEL_10K_Pos)   /*!< Setting WWDT clock source as internal 10KHz RC clock */

// For SYS.h compatible to Nano BSP
#define SYS_PA_L_MFP_PA0_MFP_GPIO       0x00000000UL        /*!< GPA_MFP PA.0 setting for GPIO */
#define SYS_PA_L_MFP_PA0_MFP_ADC_CH0    (1UL<<0)            /*!< GPA_MFP PA.0 setting for ADC0 */
#define SYS_PA_L_MFP_PA0_MFP_SC0_PWR    (1UL<<0)            /*!< GPA_MFP PA.0 setting for SC0_PWR */
#define SYS_PA_L_MFP_PA0_MFP_Msk        (1UL<<0)            /*!< GPA_MFP PA.0 mask */

#define SYS_PA_L_MFP_PA1_MFP_GPIO       0x00000000UL        /*!< GPA_MFP PA.1 setting for GPIO */
#define SYS_PA_L_MFP_PA1_MFP_ADC_CH1    (1UL<<1)            /*!< GPA_MFP PA.1 setting for ADC1 */
#define SYS_PA_L_MFP_PA1_MFP_SC0_RST    (1UL<<1)            /*!< GPA_MFP PA.1 setting for SC0_RST */
#define SYS_PA_L_MFP_PA1_MFP_EBI_AD12   (1UL<<1)            /*!< GPA_MFP PA.1 setting for EBI_AD12 */
#define SYS_PA_L_MFP_PA1_MFP_Msk        (1UL<<1)            /*!< GPA_MFP PA.1 mask */

#define SYS_PA_L_MFP_PA2_MFP_GPIO       0x00000000UL        /*!< GPA_MFP PA.2 setting for GPIO */
#define SYS_PA_L_MFP_PA2_MFP_ADC_CH2    (1UL<<2)            /*!< GPA_MFP PA.2 setting for ADC2 */
#define SYS_PA_L_MFP_PA2_MFP_SC0_CLK    (1UL<<2)            /*!< GPA_MFP PA.2 setting for SC0_CLK */
#define SYS_PA_L_MFP_PA2_MFP_EBI_AD11   (1UL<<2)            /*!< GPA_MFP PA.2 setting for EBI_AD11 */
#define SYS_PA_L_MFP_PA2_MFP_Msk        (1UL<<2)            /*!< GPA_MFP PA.2 mask */

#define SYS_PA_L_MFP_PA3_MFP_GPIO       0x00000000UL        /*!< GPA_MFP PA.3 setting for GPIO */
#define SYS_PA_L_MFP_PA3_MFP_ADC_CH3    (1UL<<3)            /*!< GPA_MFP PA.3 setting for ADC3 */
#define SYS_PA_L_MFP_PA3_MFP_SC0_DAT    (1UL<<3)            /*!< GPA_MFP PA.3 setting for SC0_DAT */
#define SYS_PA_L_MFP_PA3_MFP_EBI_AD10   (1UL<<3)            /*!< GPA_MFP PA.3 setting for EBI_AD10 */
#define SYS_PA_L_MFP_PA3_MFP_Msk        (1UL<<3)            /*!< GPA_MFP PA.3 mask */

#define SYS_PA_L_MFP_PA4_MFP_GPIO       0x00000000UL        /*!< GPA_MFP PA.4 setting for GPIO */
#define SYS_PA_L_MFP_PA4_MFP_ADC_CH4    (1UL<<4)            /*!< GPA_MFP PA.4 setting for ADC4 */
#define SYS_PA_L_MFP_PA4_MFP_SC1_PWR    (1UL<<4)            /*!< GPA_MFP PA.4 setting for SC1_PWR */
#define SYS_PA_L_MFP_PA4_MFP_EBI_AD9    (1UL<<4)            /*!< GPA_MFP PA.4 setting for EBI_AD9 */
#define SYS_PA_L_MFP_PA4_MFP_Msk        (1UL<<4)            /*!< GPA_MFP PA.4 mask */

#define SYS_PA_L_MFP_PA5_MFP_GPIO       0x00000000UL        /*!< GPA_MFP PA.5 setting for GPIO */
#define SYS_PA_L_MFP_PA5_MFP_ADC_CH5    (1UL<<5)            /*!< GPA_MFP PA.5 setting for ADC5 */
#define SYS_PA_L_MFP_PA5_MFP_SC1_RST    (1UL<<5)            /*!< GPA_MFP PA.5 setting for SC1_RST */
#define SYS_PA_L_MFP_PA5_MFP_EBI_AD8    (1UL<<5)            /*!< GPA_MFP PA.5 setting for EBI_AD8 */
#define SYS_PA_L_MFP_PA5_MFP_Msk        (1UL<<5)            /*!< GPA_MFP PA.5 mask */

#define SYS_PA_L_MFP_PA6_MFP_GPIO       0x00000000UL        /*!< GPA_MFP PA.6 setting for GPIO */
#define SYS_PA_L_MFP_PA6_MFP_ADC_CH6    (1UL<<6)            /*!< GPA_MFP PA.6 setting for ADC6 */
#define SYS_PA_L_MFP_PA6_MFP_SC1_CLK    (1UL<<6)            /*!< GPA_MFP PA.6 setting for SC1_CLK */
#define SYS_PA_L_MFP_PA6_MFP_EBI_AD7    (1UL<<6)            /*!< GPA_MFP PA.6 setting for AD7 */ 
#define SYS_PA_L_MFP_PA6_MFP_Msk        (1UL<<6)            /*!< GPA_MFP PA.6 mask */

#define SYS_PA_L_MFP_PA7_MFP_GPIO       0x00000000UL        /*!< GPA_MFP PA.7 setting for GPIO */
#define SYS_PA_L_MFP_PA7_MFP_ADC_CH7    (1UL<<7)            /*!< GPA_MFP PA.7 setting for ADC7 */
#define SYS_PA_L_MFP_PA7_MFP_SPI2_SS1   (1UL<<7)            /*!< GPA_MFP PA.7 setting for SPI2_SS1 */
#define SYS_PA_L_MFP_PA7_MFP_SC1_DAT    (1UL<<7)            /*!< GPA_MFP PA.7 setting for SC1_DAT */
#define SYS_PA_L_MFP_PA7_MFP_EBI_AD6    (1UL<<7)            /*!< GPA_MFP PA.7 setting for EBI_AD6 */  
#define SYS_PA_L_MFP_PA7_MFP_Msk        (1UL<<7)            /*!< GPA_MFP PA.7 mask */

#define SYS_PA_H_MFP_PA8_MFP_GPIO       0x00000000UL        /*!< GPA_MFP PA.8 setting for GPIO */
#define SYS_PA_H_MFP_PA8_MFP_I2C0_SDA   (1UL<<8)            /*!< GPA_MFP PA.8 setting for I2C0_SDA */
#define SYS_PA_H_MFP_PA8_MFP_Msk        (1UL<<8)            /*!< GPA_MFP PA.8 mask */

#define SYS_PA_H_MFP_PA9_MFP_GPIO       0x00000000UL        /*!< GPA_MFP PA.9 setting for GPIO */
#define SYS_PA_H_MFP_PA9_MFP_I2C0_SCL   (1UL<<9)            /*!< GPA_MFP PA.9 setting for I2C0_SCL */
#define SYS_PA_H_MFP_PA9_MFP_Msk        (1UL<<9)            /*!< GPA_MFP PA.9 mask */

#define SYS_PA_H_MFP_PA10_MFP_GPIO      0x00000000UL        /*!< GPA_MFP PA.10 setting for GPIO */
#define SYS_PA_H_MFP_PA10_MFP_I2C1_SDA  (1UL<<10)           /*!< GPA_MFP PA.10 setting for I2C1_SDA */
#define SYS_PA_H_MFP_PA10_MFP_EBI_nWR   (1UL<<10)           /*!< GPA_MFP PA.10 setting for EBI_nWR */  
#define SYS_PA_H_MFP_PA10_MFP_Msk       (1UL<<10)           /*!< GPA_MFP PA.10 mask */

#define SYS_PA_H_MFP_PA11_MFP_GPIO      0x00000000UL        /*!< GPA_MFP PA.11 setting for GPIO */
#define SYS_PA_H_MFP_PA11_MFP_I2C1_SCL  (1UL<<11)           /*!< GPA_MFP PA.11 setting for I2C1_SCL */
#define SYS_PA_H_MFP_PA11_MFP_EBI_nRD   (1UL<<11)           /*!< GPA_MFP PA.11 setting for EBI_nRD */ 
#define SYS_PA_H_MFP_PA11_MFP_Msk       (1UL<<11)           /*!< GPA_MFP PA.11 mask */

#define SYS_PA_H_MFP_PA12_MFP_GPIO      0x00000000UL        /*!< GPA_MFP PA.12 setting for GPIO */
#define SYS_PA_H_MFP_PA12_MFP_PWM_CH0   (1UL<<12)           /*!< GPA_MFP PA.12 setting for PWM0 */
#define SYS_PA_H_MFP_PA12_SC2_DAT       (1UL<<12)           /*!< GPA_MFP PA.12 setting for SC2_DAT */
#define SYS_PA_H_MFP_PA12_EBI_AD13      (1UL<<12)           /*!< GPA_MFP PA.12 setting for EBI_AD13 */
#define SYS_PA_H_MFP_PA12_MFP_Msk       (1UL<<12)           /*!< GPA_MFP PA.12 mask */

#define SYS_PA_H_MFP_PA13_MFP_GPIO      0x00000000UL        /*!< GPA_MFP PA.13 setting for GPIO */
#define SYS_PA_H_MFP_PA13_MFP_PWM_CH1   (1UL<<13)           /*!< GPA_MFP PA.13 setting for PWM1 */
#define SYS_PA_H_MFP_PA13_MFP_SC2_CLK   (1UL<<13)           /*!< GPA_MFP PA.13 setting for SC2_CLK */
#define SYS_PA_H_MFP_PA13_MFP_EBI_AD14  (1UL<<13)           /*!< GPA_MFP PA.13 setting for EBI_AD14 */
#define SYS_PA_H_MFP_PA13_MFP_Msk       (1UL<<13)           /*!< GPA_MFP PA.13 mask */

#define SYS_PA_H_MFP_PA14_MFP_GPIO      0x00000000UL        /*!< GPA_MFP PA.14 setting for GPIO */
#define SYS_PA_H_MFP_PA14_MFP_PWM_CH2   (1UL<<14)           /*!< GPA_MFP PA.14 setting for PWM2 */
#define SYS_PA_H_MFP_PA14_MFP_SC2_RST   (1UL<<14)           /*!< GPA_MFP PA.14 setting for SC2_RST */
#define SYS_PA_H_MFP_PA14_MFP_EBI_AD15  (1UL<<14)           /*!< GPA_MFP PA.14 setting for EBI_AD15 */
#define SYS_PA_H_MFP_PA14_MFP_Msk       (1UL<<14)           /*!< GPA_MFP PA.14 mask */

#define SYS_PA_H_MFP_PA15_MFP_GPIO      0x00000000UL        /*!< GPA_MFP PA.15 setting for GPIO */
#define SYS_PA_H_MFP_PA15_MFP_PWM_CH3   (1UL<<15)           /*!< GPA_MFP PA.15 setting for PWM3 */
#define SYS_PA_H_MFP_PA15_MFP_I2S_MCLK  (1UL<<15)           /*!< GPA_MFP PA.15 setting for I2S_MCLK */
#define SYS_PA_H_MFP_PA15_MFP_SC2_PWR   (1UL<<15)           /*!< GPA_MFP PA.15 setting for SC2_PWR */
#define SYS_PA_H_MFP_PA15_MFP_Msk       (1UL<<15)           /*!< GPA_MFP PA.15 mask */

#define SYS_PB_L_MFP_PB0_MFP_GPIO       0x00000000UL    /*!< GPB_MFP PB.0 setting for GPIO */
#define SYS_PB_L_MFP_PB0_MFP_UART0_RX   (1UL<<0)        /*!< GPB_MFP PB.0 setting for UART0_RXD */
#define SYS_PB_L_MFP_PB0_MFP_Msk        (1UL<<0)        /*!< GPB_MFP PB.0 mask */

#define SYS_PB_L_MFP_PB1_MFP_GPIO       0x00000000UL    /*!< GPB_MFP PB.1 setting for GPIO */
#define SYS_PB_L_MFP_PB1_MFP_UART0_TX   (1UL<<1)        /*!< GPB_MFP PB.1 setting for UART0_TXD */
#define SYS_PB_L_MFP_PB1_MFP_Msk        (1UL<<1)        /*!< GPB_MFP PB.1 mask */

#define SYS_PB_L_MFP_PB2_MFP_GPIO       0x00000000UL    /*!< GPB_MFP PB.2 setting for GPIO */
#define SYS_PB_L_MFP_PB2_MFP_UART0_nRTS (1UL<<2)        /*!< GPB_MFP PB.2 setting for UART0_nRTS */
#define SYS_PB_L_MFP_PB2_MFP_EBI_nWRL   (1UL<<2)        /*!< GPB_MFP PB.2 setting for EBI_nWRL */    
#define SYS_PB_L_MFP_PB2_MFP_TC2        (1UL<<2)        /*!< GPB_MFP PB.2 setting for TC2 */
#define SYS_PB_L_MFP_PB2_MFP_TM2_EXT    (1UL<<2)        /*!< GPB_MFP PB.2 setting for TM2_EXT */
#define SYS_PB_L_MFP_PB2_MFP_Msk        (1UL<<2)        /*!< GPB_MFP PB.2 mask */

#define SYS_PB_L_MFP_PB3_MFP_GPIO       0x00000000UL    /*!< GPB_MFP PB.3 setting for GPIO */
#define SYS_PB_L_MFP_PB3_MFP_ART0_nCTS  (1UL<<3)        /*!< GPB_MFP PB.3 setting for UART0_nCTS */
#define SYS_PB_L_MFP_PB3_MFP_TC3        (1UL<<3)        /*!< GPB_MFP PB.3 setting for TC3 */
#define SYS_PB_L_MFP_PB3_MFP_T3EX       (1UL<<3)        /*!< GPB_MFP PB.3 setting for T3EX */
#define SYS_PB_L_MFP_PB3_MFP_TM3_EXT    (1UL<<3)        /*!< GPB_MFP PB.3 setting for TM3_EXT */
#define SYS_PB_L_MFP_PB3_MFP_SC2_CD     (1UL<<3)        /*!< GPB_MFP PB.3 setting for SC2_CD */
#define SYS_PB_L_MFP_PB3_MFP_EBI_nWRH   (1UL<<3)        /*!< GPB_MFP PB.3 setting for EBI_nWRH */ 
#define SYS_PB_L_MFP_PB3_MFP_Msk        (1UL<<3)        /*!< GPB_MFP PB.3 mask */

#define SYS_PB_L_MFP_PB4_MFP_GPIO       0x00000000UL    /*!< GPB_MFP PB.4 setting for GPIO */
#define SYS_PB_L_MFP_PB4_MFP_UART1_RX   (1UL<<4)        /*!< GPA_MFP PB.4 setting for UART1_RXD */
#define SYS_PB_L_MFP_PB4_MFP_Msk        (1UL<<4)        /*!< GPA_MFP PB.4 mask */

#define SYS_PB_L_MFP_PB5_MFP_GPIO       0x00000000UL    /*!< GPB_MFP PB.5 setting for GPIO */
#define SYS_PB_L_MFP_PB5_MFP_UART1_TX   (1UL<<5)        /*!< GPA_MFP PB.5 setting for UART1_RXD */
#define SYS_PB_L_MFP_PB5_MFP_Msk        (1UL<<5)        /*!< GPA_MFP PB.5 mask */

#define SYS_PB_L_MFP_PB6_MFP_GPIO       0x00000000UL    /*!< GPB_MFP PB.6 setting for GPIO */
#define SYS_PB_L_MFP_PB6_MFP_UART1_nRTS (1UL<<6)        /*!< GPB_MFP PB.6 setting for UART1_nRTS */
#define SYS_PB_L_MFP_PB6_MFP_EBI_ALE    (1UL<<6)        /*!< GPB_MFP PB.6 setting for EBI_ALE */   
#define SYS_PB_L_MFP_PB6_MFP_Msk        (1UL<<6)        /*!< GPB_MFP PB.6 mask */

#define SYS_PB_L_MFP_PB7_MFP_GPIO       0x00000000UL    /*!< GPB_MFP PB.7 setting for GPIO */
#define SYS_PB_L_MFP_PB7_MFP_UART1_nCTS (1UL<<7)        /*!< GPB_MFP PB.7 setting for UART1_nCTS */
#define SYS_PB_L_MFP_PB7_MFP_EBI_nCS    (1UL<<7)        /*!< GPB_MFP PB.7 setting for EBI_nCS */
#define SYS_PB_L_MFP_PB7_MFP_Msk        (1UL<<7)        /*!< GPB_MFP PB.7 mask */

#define SYS_PB_H_MFP_PB8_MFP_GPIO       0x00000000UL    /*!< GPB_MFP PB.8 setting for GPIO */
#define SYS_PB_H_MFP_PB8_MFP_TM0        (1UL<<8)        /*!< GPA_MFP PB.8 setting for TM0 */
#define SYS_PB_H_MFP_PB8_MFP_Msk        (1UL<<8)        /*!< GPB_MFP PB.8 mask */

#define SYS_PB_H_MFP_PB9_MFP_GPIO       0x00000000UL    /*!< GPB_MFP PB.9 setting for GPIO */
#define SYS_PB_H_MFP_PB9_MFP_TM1        (1UL<<9)        /*!< GPB_MFP PB.9 setting for TM1 */
#define SYS_PB_H_MFP_PB9_MFP_SPI1_SS1   (1UL<<9)        /*!< GPB_MFP PB.9 setting for SPI1_SS1 */
#define SYS_PB_H_MFP_PB9_MFP_Msk        (1UL<<9)        /*!< GPB_MFP PB.9 mask */

#define SYS_PB_H_MFP_PB10_MFP_GPIO      0x00000000UL    /*!< GPB_MFP PB.10 setting for GPIO */
#define SYS_PB_H_MFP_PB10_MFP_TM2       (1UL<<10)       /*!< GPB_MFP PB.10 setting for TM2 */
#define SYS_PB_H_MFP_PB10_MFP_SPI0_SS1  (1UL<<10)       /*!< GPB_MFP PB.10 setting for SPI0_SS1 */
#define SYS_PB_H_MFP_PB10_MFP_Msk       (1UL<<10)       /*!< GPB_MFP PB.10 mask */

#define SYS_PB_H_MFP_PB11_MFP_GPIO      0x00000000UL    /*!< GPB_MFP PB.11 setting for GPIO */
#define SYS_PB_H_MFP_PB11_MFP_TM3       (1UL<<11)       /*!< GPB_MFP PB.11 setting for TM3 */
#define SYS_PB_H_MFP_PB11_MFP_PWM_CH4   (1UL<<11)       /*!< GPB_MFP PB.11 setting for PWM4 */
#define SYS_PB_H_MFP_PB11_MFP_Msk       (1UL<<11)       /*!< GPB_MFP PB.11 mask */

#define SYS_PB_H_MFP_PB12_MFP_GPIO      0x00000000UL    /*!< GPB_MFP PB.12 setting for GPIO */
#define SYS_PB_H_MFP_PB12_MFP_ACMP0_O   (1UL<<12)       /*!< GPB_MFP PB.12 setting for ACMP0_O */ 
#define SYS_PB_H_MFP_PB12_MFP_CLKO      (1UL<<12)       /*!< GPB_MFP PB.12 setting for CLKO */ 
#define SYS_PB_H_MFP_PB12_MFP_EBI_AD0   (1UL<<12)       /*!< GPB_MFP PB.12 setting for AD0 */ 
#define SYS_PB_H_MFP_PB12_MFP_Msk       (1UL<<12)       /*!< GPB_MFP PB.12 mask */

#define SYS_PB_H_MFP_PB13_MFP_GPIO      0x00000000UL    /*!< GPB_MFP PB.13 setting for GPIO */
#define SYS_PB_H_MFP_PB13_MFP_ACMP1_O   (1UL<<13)       /*!< GPB_MFP PB.13 setting for ACMP1_O */
#define SYS_PB_H_MFP_PB13_MFP_EBI_AD1   (1UL<<13)       /*!< GPB_MFP PB.13 setting for EBI_AD1 */
#define SYS_PB_H_MFP_PB13_MFP_Msk       (1UL<<13)       /*!< GPB_MFP PB.13 mask */

#define SYS_PB_H_MFP_PB14_MFP_GPIO      0x00000000UL    /*!< GPB_MFP PB.14 setting for GPIO */
#define SYS_PB_H_MFP_PB14_MFP_EINT0     (1UL<<14)       /*!< GPB_MFP PB.14 setting for INT0 */
#define SYS_PB_H_MFP_PB14_MFP_SPI3_SS1  (1UL<<14)       /*!< GPB_MFP PB.14 setting for SPI3_SS1 */
#define SYS_PB_H_MFP_PB14_MFP_Msk       (1UL<<14)       /*!< GPB_MFP PB.14 mask */

#define SYS_PB_H_MFP_PB15_MFP_GPIO      0x00000000UL    /*!< GPB_MFP PB.15 setting for GPIO */
#define SYS_PB_H_MFP_PB15_MFP_EINT1     (1UL<<15)       /*!< GPB_MFP PB.15 setting for INT1 */
#define SYS_PB_H_MFP_PB15_MFP_TC0       (1UL<<15)       /*!< GPB_MFP PB.15 setting for TC0 */
#define SYS_PB_H_MFP_PB15_MFP_TM0_EXT   (1UL<<15)       /*!< GPB_MFP PB.15 setting for TM0_EXT */
#define SYS_PB_H_MFP_PB15_MFP_Msk       (1UL<<15)       /*!< GPB_MFP PB.15 mask */

#define SYS_PC_L_MFP_PC0_MFP_GPIO      0x00000000UL    /*!< GPC_MFP PC.0 setting for GPIO */
#define SYS_PC_L_MFP_PC0_MFP_SPI0_SS0  (1UL<<0)        /*!< GPC_MFP PC.0 setting for SPI0_SS0 */
#define SYS_PC_L_MFP_PC0_MFP_I2S_LRCLK (1UL<<0)        /*!< GPC_MFP PC.0 setting for I2S_LRCLK */
#define SYS_PC_L_MFP_PC0_MFP_I2S_LRCK  (1UL<<0)        /*!< GPC_MFP PC.0 setting for I2S_LRCK */
#define SYS_PC_L_MFP_PC0_MFP_Msk       (1UL<<0)        /*!< GPC_MFP PC.0 mask */

#define SYS_PC_L_MFP_PC1_MFP_GPIO      0x00000000UL    /*!< GPC_MFP PC.1 setting for GPIO */
#define SYS_PC_L_MFP_PC1_MFP_SPI0_CLK  (1UL<<1)        /*!< GPC_MFP PC.1 setting for SPI0_CLK */
#define SYS_PC_L_MFP_PC1_MFP_I2S_BCLK  (1UL<<1)        /*!< GPC_MFP PC.1 setting for I2S_BCLK */
#define SYS_PC_L_MFP_PC1_MFP_Msk       (1UL<<1)        /*!< GPC_MFP PC.1 mask */

#define SYS_PC_L_MFP_PC2_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.2 setting for GPIO */
#define SYS_PC_L_MFP_PC2_MFP_SPI0_MISO0 (1UL<<2)        /*!< GPC_MFP PC.2 setting for SPI0_MISO0 */
#define SYS_PC_L_MFP_PC2_MFP_I2S_DI     (1UL<<2)        /*!< GPC_MFP PC.2 setting for I2S_DI */
#define SYS_PC_L_MFP_PC2_MFP_Msk        (1UL<<2)        /*!< GPC_MFP PC.2 mask */

#define SYS_PC_L_MFP_PC3_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.3 setting for GPIO */
#define SYS_PC_L_MFP_PC3_MFP_SPI0_MOSI0 (1UL<<3)        /*!< GPC_MFP PC.3 setting for SPI0_MOSI0 */
#define SYS_PC_L_MFP_PC3_MFP_I2S_DO     (1UL<<3)        /*!< GPC_MFP PC.3 setting for I2S_DO */
#define SYS_PC_L_MFP_PC3_MFP_Msk        (1UL<<3)        /*!< GPC_MFP PC.3 mask */

#define SYS_PC_L_MFP_PC4_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.4 setting for GPIO */
#define SYS_PC_L_MFP_PC4_MFP_SPI0_MISO1 (1UL<<4)        /*!< GPC_MFP PC.4 setting for SPI0_MISO1 */
#define SYS_PC_L_MFP_PC4_MFP_Msk        (1UL<<4)        /*!< GPC_MFP PC.4 mask */

#define SYS_PC_L_MFP_PC5_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.5 setting for GPIO */
#define SYS_PC_L_MFP_PC5_MFP_SPI0_MOSI1 (1UL<<5)        /*!< GPC_MFP PC.5 setting for SPI0_MOSI1 */
#define SYS_PC_L_MFP_PC5_MFP_Msk        (1UL<<5)        /*!< GPC_MFP PC.5 mask */

#define SYS_PC_L_MFP_PC6_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.6 setting for GPIO */
#define SYS_PC_L_MFP_PC6_MFP_ACMP0_P    (1UL<<6)        /*!< GPC_MFP PC.6 setting for ACMP0_P */
#define SYS_PC_L_MFP_PC6_MFP_SC0_CD     (1UL<<6)        /*!< GPC_MFP PC.6 setting for SC0_CD */
#define SYS_PC_L_MFP_PC6_MFP_EBI_AD4    (1UL<<6)        /*!< GPC_MFP PC.6 setting for EBI_AD4 */
#define SYS_PC_L_MFP_PC6_MFP_Msk        (1UL<<6)        /*!< GPC_MFP PC.6 mask */

#define SYS_PC_L_MFP_PC7_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.7 setting for GPIO */
#define SYS_PC_L_MFP_PC7_MFP_ACMP0_N    (1UL<<7)        /*!< GPC_MFP PC.7 setting for ACMP0_N */
#define SYS_PC_L_MFP_PC7_MFP_SC1_CD     (1UL<<7)        /*!< GPC_MFP PC.7 setting for SC1_CD */
#define SYS_PC_L_MFP_PC7_MFP_EBI_AD5    (1UL<<7)        /*!< GPC_MFP PC.7 setting for EBI_AD5 */
#define SYS_PC_L_MFP_PC7_MFP_Msk        (1UL<<7)        /*!< GPC_MFP PC.7 mask */

#define SYS_PC_H_MFP_PC8_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.8 setting for GPIO */
#define SYS_PC_H_MFP_PC8_MFP_SPI1_SS0   (1UL<<8)        /*!< GPC_MFP PC.8 setting for SPI1_SS0 */
#define SYS_PC_H_MFP_PC8_MFP_EBI_MCLK   (1UL<<8)        /*!< GPC_MFP PC.8 setting for EBI_MCLK */ 
#define SYS_PC_H_MFP_PC8_MFP_Msk        (1UL<<8)        /*!< GPC_MFP PC.8 mask */

#define SYS_PC_H_MFP_PC9_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.9 setting for GPIO */
#define SYS_PC_H_MFP_PC9_MFP_SPI1_CLK   (1UL<<9)        /*!< GPC_MFP PC.9 setting for SPI1_CLK */
#define SYS_PC_H_MFP_PC9_MFP_Msk        (1UL<<9)        /*!< GPC_MFP PC.9 mask */

#define SYS_PC_H_MFP_PC10_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.10 setting for GPIO */
#define SYS_PC_H_MFP_PC10_MFP_SPI1_MISO0 (1UL<<10)       /*!< GPC_MFP PC.10 setting for SPI1_MISO0 */
#define SYS_PC_H_MFP_PC10_MFP_Msk        (1UL<<10)       /*!< GPC_MFP PC.10 mask */

#define SYS_PC_H_MFP_PC11_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.11 setting for GPIO */
#define SYS_PC_H_MFP_PC11_MFP_SPI1_MOSI0 (1UL<<11)       /*!< GPC_MFP PC.11 setting for SPI1_MOSI0 */
#define SYS_PC_H_MFP_PC11_MFP_Msk        (1UL<<11)       /*!< GPC_MFP PC.11 mask */

#define SYS_PC_H_MFP_PC12_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.12 setting for GPIO */
#define SYS_PC_H_MFP_PC12_MFP_SPI1_MISO1 (1UL<<12)       /*!< GPC_MFP PC.12 setting for SPI1_MISO1 */
#define SYS_PC_H_MFP_PC12_MFP_Msk        (1UL<<12)       /*!< GPC_MFP PC.12 mask */

#define SYS_PC_H_MFP_PC13_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.13 setting for GPIO */
#define SYS_PC_H_MFP_PC13_MFP_SPI1_MOSI0 (1UL<<13)       /*!< GPC_MFP PC.13 setting for SPI1_MOSI1 */
#define SYS_PC_H_MFP_PC13_MFP_Msk        (1UL<<13)       /*!< GPC_MFP PC.13 mask */

#define SYS_PC_H_MFP_PC14_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.14 setting for GPIO */
#define SYS_PC_H_MFP_PC14_MFP_ACMP1_P    (1UL<<14)       /*!< GPC_MFP PC.14 setting for ACMP1_P */
#define SYS_PC_H_MFP_PC14_MFP_EBI_AD2    (1UL<<14)       /*!< GPC_MFP PC.14 setting for EBI_AD2 */ 
#define SYS_PC_H_MFP_PC14_MFP_Msk        (1UL<<14)       /*!< GPC_MFP PC.14 mask */

#define SYS_PC_H_MFP_PC15_MFP_GPIO       0x00000000UL    /*!< GPC_MFP PC.15 setting for GPIO */
#define SYS_PC_H_MFP_PC15_MFP_ACMP1_N    (1UL<<15)       /*!< GPC_MFP PC.15 setting for ACMP1_N */
#define SYS_PC_H_MFP_PC15_MFP_EBI_AD3    (1UL<<15)       /*!< GPC_MFP PC.15 setting for EBI_AD3 */ 
#define SYS_PC_H_MFP_PC15_MFP_Msk        (1UL<<15)       /*!< GPC_MFP PC.15 mask */

#define SYS_PD_L_MFP_PD0_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PD.0 setting for GPIO */
#define SYS_PD_L_MFP_PD0_MFP_SPI2_SS0   (1UL<<0)        /*!< GPD_MFP PD.0 setting for SPI2_SS0 */
#define SYS_PD_L_MFP_PD0_MFP_Msk        (1UL<<0)        /*!< GPD_MFP PD.0 mask */

#define SYS_PD_L_MFP_PD1_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PD.1 setting for GPIO */
#define SYS_PD_L_MFP_PD1_MFP_SPI2_CLK   (1UL<<1)        /*!< GPD_MFP PD.1 setting for SPI2_CLK */
#define SYS_PD_L_MFP_PD1_MFP_Msk        (1UL<<1)        /*!< GPD_MFP PD.1 mask */

#define SYS_PD_L_MFP_PD2_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PD.2 setting for GPIO */
#define SYS_PD_L_MFP_PD2_MFP_SPI2_MISO0 (1UL<<2)        /*!< GPD_MFP PD.2 setting for SPI2_MISO0 */
#define SYS_PD_L_MFP_PD2_MFP_Msk        (1UL<<2)        /*!< GPD_MFP PD.2 mask */

#define SYS_PD_L_MFP_PD3_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PD.3 setting for GPIO */
#define SYS_PD_L_MFP_PD3_MFP_SPI2_MOSI0 (1UL<<3)        /*!< GPD_MFP PD.3 setting for SPI2_MOSI0 */
#define SYS_PD_L_MFP_PD3_MFP_Msk        (1UL<<3)        /*!< GPD_MFP PD.3 mask */

#define SYS_PD_L_MFP_PD4_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PD.4 setting for GPIO */
#define SYS_PD_L_MFP_PD4_MFP_SPI2_MISO1 (1UL<<4)        /*!< GPD_MFP PD.4 setting for SPI2_MISO1 */
#define SYS_PD_L_MFP_PD4_MFP_Msk        (1UL<<4)        /*!< GPD_MFP PD.4 mask */

#define SYS_PD_L_MFP_PD5_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PD.5 setting for GPIO */
#define SYS_PD_L_MFP_PD5_MFP_SPI2_MOSI1 (1UL<<5)        /*!< GPD_MFP PD.5 setting for SPI2_MOSI1 */
#define SYS_PD_L_MFP_PD5_MFP_Msk        (1UL<<5)        /*!< GPD_MFP PD.5 mask */

#define SYS_PD_L_MFP_PD6_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PD.6 setting for GPIO */
#define SYS_PD_L_MFP_PD6_MFP_CAN0_RX    (1UL<<6)        /*!< GPD_MFP PD.5 setting for SPI2_MOSI1 */
#define SYS_PD_L_MFP_PD6_MFP_Msk        (1UL<<6)        /*!< GPD_MFP PD.6 mask */

#define SYS_PD_L_MFP_PD7_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PD.7 setting for GPIO */
#define SYS_PD_L_MFP_PD7_MFP_CAN0_TX    (1UL<<7)        /*!< GPD_MFP PD.5 setting for SPI2_MOSI1 */
#define SYS_PD_L_MFP_PD7_MFP_Msk        (1UL<<7)        /*!< GPD_MFP PD.7 mask */

#define SYS_PD_H_MFP_PD8_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PD.8 setting for GPIO */
#define SYS_PD_H_MFP_PD8_MFP_SPI3_SS0   (1UL<<8)        /*!< GPD_MFP PD.8 setting for SPI3_SS0 */
#define SYS_PD_H_MFP_PD8_MFP_Msk        (1UL<<8)        /*!< GPD_MFP PD.8 mask */

#define SYS_PD_H_MFP_PD9_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PD.9 setting for GPIO */
#define SYS_PD_H_MFP_PD9_MFP_SPI3_CLK   (1UL<<9)        /*!< GPD_MFP PD.9 setting for SPI3_CLK */
#define SYS_PD_H_MFP_PD9_MFP_Msk        (1UL<<9)        /*!< GPD_MFP PD.9 mask */

#define SYS_PD_H_MFP_PD10_MFP_GPIO       0x00000000UL     /*!< GPD_MFP PD.10 setting for GPIO */
#define SYS_PD_H_MFP_PD10_MFP_SPI3_MISO0 (1UL<<10)        /*!< GPD_MFP PD.10 setting for SPI3_MISO0 */
#define SYS_PD_H_MFP_PD10_MFP_Msk        (1UL<<10)        /*!< GPD_MFP PD.10 mask */

#define SYS_PD_H_MFP_PD11_MFP_GPIO       0x00000000UL     /*!< GPD_MFP PD.11 setting for GPIO */
#define SYS_PD_H_MFP_PD11_MFP_SPI3_MOSI0 (1UL<<11)        /*!< GPD_MFP PD.11 setting for SPI3_MOSI0 */
#define SYS_PD_H_MFP_PD11_MFP_Msk        (1UL<<11)        /*!< GPD_MFP PD.11 mask */

#define SYS_PD_H_MFP_PD12_MFP_GPIO       0x00000000UL     /*!< GPD_MFP PD.12 setting for GPIO */
#define SYS_PD_H_MFP_PD12_MFP_SPI3_MISO1 (1UL<<12)        /*!< GPD_MFP PD.12 setting for SPI3_MISO1 */
#define SYS_PD_H_MFP_PD12_MFP_Msk        (1UL<<12)        /*!< GPD_MFP PD.12 mask */

#define SYS_PD_H_MFP_PD13_MFP_GPIO       0x00000000UL     /*!< GPD_MFP PD.13 setting for GPIO */
#define SYS_PD_H_MFP_PD13_MFP_SPI3_MOSI1 (1UL<<13)        /*!< GPD_MFP PD.13 setting for SPI3_MOSI1 */
#define SYS_PD_H_MFP_PD13_MFP_Msk        (1UL<<13)        /*!< GPD_MFP PD.13 mask */

#define SYS_PD_H_MFP_PD14_MFP_GPIO       0x00000000UL     /*!< GPD_MFP PD.14 setting for GPIO */
#define SYS_PD_H_MFP_PD14_MFP_UART2_RX   (1UL<<14)        /*!< GPD_MFP PD.14 setting for UART2_RXD */
#define SYS_PD_H_MFP_PD14_MFP_Msk        (1UL<<14)        /*!< GPD_MFP PD.14 mask */

#define SYS_PD_H_MFP_PD15_MFP_GPIO       0x00000000UL     /*!< GPD_MFP PD.15 setting for GPIO */
#define SYS_PD_H_MFP_PD15_MFP_UART2_TX   (1UL<<15)        /*!< GPD_MFP PD.15 setting for UART2_TXD */
#define SYS_PD_H_MFP_PD15_MFP_Msk        (1UL<<15)        /*!< GPD_MFP PD.15 mask */

#define SYS_PE_L_MFP_PE0_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PE.0 setting for GPIO */
#define SYS_PE_L_MFP_PE0_MFP_PWM_CH6    (1UL<<0)        /*!< GPE_MFP PE.0 setting for PWM6 */
#define SYS_PE_L_MFP_PE0_MFP_Msk        (1UL<<0)        /*!< GPD_MFP PE.0 mask */

#define SYS_PE_L_MFP_PE1_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PE.1 setting for GPIO */
#define SYS_PE_L_MFP_PE1_MFP_PWM_CH7        (1UL<<1)        /*!< GPE_MFP PE.1 setting for PWM7 */
#define SYS_PE_L_MFP_PE1_MFP_Msk        (1UL<<1)        /*!< GPD_MFP PE.1 mask */

#define SYS_PE_L_MFP_PE2_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PE.2 setting for GPIO */
#define SYS_PE_L_MFP_PE2_MFP_Msk        (1UL<<2)        /*!< GPD_MFP PE.2 mask */

#define SYS_PE_L_MFP_PE3_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PE.3 setting for GPIO */
#define SYS_PE_L_MFP_PE3_MFP_Msk        (1UL<<3)        /*!< GPD_MFP PE.3 mask */

#define SYS_PE_L_MFP_PE4_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PE.4 setting for GPIO */
#define SYS_PE_L_MFP_PE4_MFP_Msk        (1UL<<4)        /*!< GPD_MFP PE.4 mask */

#define SYS_PE_L_MFP_PE5_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PE.5 setting for GPIO */
#define SYS_PE_L_MFP_PE5_MFP_PWM_CH5    (1UL<<5)        /*!< GPE_MFP PE.5 setting for PWM5 */
#define SYS_PE_L_MFP_PE5_MFP_TC1        (1UL<<5)        /*!< GPE_MFP PE.5 setting for TC1  */
#define SYS_PE_L_MFP_PE5_MFP_TM1_EXT    (1UL<<5)        /*!< GPE_MFP PE.5 setting for TM1_EXT */
#define SYS_PE_L_MFP_PE5_MFP_Msk        (1UL<<5)        /*!< GPD_MFP PE.5 mask */

#define SYS_PF_L_MFP_PF0_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PF.0 setting for GPIO */
#define SYS_PF_L_MFP_PF0_MFP_XT1_OUT    (1UL<<0)        /*!< GPF_MFP PF.0 setting for XT1_OUT */
#define SYS_PF_L_MFP_PF0_MFP_Msk        (1UL<<0)        /*!< GPD_MFP PF.0 mask */

#define SYS_PF_L_MFP_PF1_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PF.1 setting for GPIO */
#define SYS_PF_L_MFP_PF1_MFP_XT1_IN     (1UL<<1)        /*!< GPF_MFP PF.1 setting for XT1_IN  */
#define SYS_PF_L_MFP_PF1_MFP_Msk        (1UL<<1)        /*!< GPD_MFP PF.1 mask */

#define SYS_PF_L_MFP_PF2_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PF.2 setting for GPIO */
#define SYS_PF_L_MFP_PF2_MFP_PS2_DAT    (1UL<<2)        /*!< GPF_MFP PF.2 setting for PS2_DAT */
#define SYS_PF_L_MFP_PF2_MFP_Msk        (1UL<<2)        /*!< GPD_MFP PF.2 mask */

#define SYS_PF_L_MFP_PF3_MFP_GPIO       0x00000000UL    /*!< GPD_MFP PF.3 setting for GPIO */
#define SYS_PF_L_MFP_PF3_MFP_PS2_CLK    (1UL<<3)        /*!< GPF_MFP PF.3 setting for PS2_CLK */
#define SYS_PF_L_MFP_PF3_MFP_Msk        (1UL<<3)        /*!< GPD_MFP PF.3 mask */

extern void SYS_Init(void);

#endif
