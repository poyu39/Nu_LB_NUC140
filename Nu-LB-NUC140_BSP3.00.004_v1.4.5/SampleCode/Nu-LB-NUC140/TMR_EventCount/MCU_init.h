//Define Clock source
#define MCU_CLOCK_SOURCE      
#define MCU_CLOCK_SOURCE_HXT  // HXT, LXT, HIRC, LIRC 
#define MCU_CLOCK_FREQUENCY   50000000  //Hz

//Define MCU Interfaces
#define MCU_INTERFACE_TMR0
#define TMR0_CLOCK_SOURCE_HXT // HXT, LXT, HCLK, EXT, LIRC, HIRC
#define TMR0_CLOCK_DIVIDER    1
#define TMR0_OPERATING_MODE   TIMER_PERIODIC_MODE // ONESHOT, PERIODIC, TOGGLE, CONTINUOUS
#define TMR0_OPERATING_FREQ   1000000 //Hz
#define TMR0_COUNTING_EDGE TIMER_COUNTER_FALLING_EDGE // FALLING, RISING
#define TMR0_CAPTURE_MODE TIMER_CAPTURE_FREE_COUNTING_MODE // FREE_COUNTING, COUNTER_RESET
#define TMR0_CAPTURE_EDGE TIMER_CAPTURE_FALLING_AND_RISING_EDGE // FALLING, RISING, FALLING_AND_RISING
#define PIN_TM0_PB8  // TM0_PB8, TM1_PB9, TM2_PB10, TM3_PB11