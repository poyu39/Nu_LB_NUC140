//Define Clock source
#define MCU_CLOCK_SOURCE      
#define MCU_CLOCK_SOURCE_HXT   // HXT, LXT, HIRC, LIRC 
#define MCU_CLOCK_FREQUENCY     48000000  //Hz

//Define MCU Interfaces
// SPI3 interface to LCD
//#define MCU_INTERFACE_SPI3
//#define SPI3_CLOCK_SOURCE_HCLK // HCLK, PLL
//#define PIN_SPI3_SS0_PD8
//#define PIN_SPI3_SCLK_PD9
//#define PIN_SPI3_MISO0_PD10
//#define PIN_SPI3_MOSI0_PD11

// Timer0 for counting System Time
#define MCU_INTERFACE_TMR0
#define TMR0_CLOCK_SOURCE_HXT // HXT, LXT, HCLK, EXT, LIRC, HIRC
#define TMR0_CLOCK_DIVIDER    1
#define TMR0_OPERATING_MODE   TIMER_PERIODIC_MODE // ONESHOT, PERIODIC, TOGGLE, CONTINUOUS
#define TMR0_OPERATING_FREQ   1000 //Hz = 1ms

// I2C0 interface to IMU
#define MCU_INTERFACE_I2C0
#define I2C0_CLOCK_FREQUENCY  400000 //Hz
#define PIN_I2C0_SCL_PA9
#define PIN_I2C0_SDA_PA8

// USB interface to PC
#define MCU_INTERFACE_USB
#define USB_CLOCK_DIVIDER     1

// UART0 for Virtual Com port
#define MCU_INTERFACE_UART0
#define UART_CLOCK_SOURCE_HXT  // HXT, LXT, PLL, HIRC
#define UART_CLOCK_DIVIDER     3
#define PIN_UART0_RX_PB0
#define PIN_UART0_TX_PB1


