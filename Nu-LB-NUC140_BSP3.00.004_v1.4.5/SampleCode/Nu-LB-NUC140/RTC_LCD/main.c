//
// RTC_LCD : RTC update time on LCD every second
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

volatile uint32_t  g_u32TICK = 0;

void RTC_IRQHandler(void)
{
    S_RTC_TIME_DATA_T sCurTime;
    char Text[16];

    // Get the current time to printf
    RTC_GetDateAndTime(&sCurTime);

    sprintf(Text,"%4d/%2d/%2d",	sCurTime.u32Year,sCurTime.u32Month,sCurTime.u32Day);
    print_Line(1, Text); // Display Date	
    sprintf(Text,"  %2d:%2d:%2d",	sCurTime.u32Hour,sCurTime.u32Minute,sCurTime.u32Second);
    print_Line(2, Text); // Display Time
	
    RTC_CLEAR_TICK_INT_FLAG();
}

void Delay(uint32_t ucnt)
{
    volatile uint32_t i = ucnt;
    while (i--);
}

void Init_RTC(void)
{
  S_RTC_TIME_DATA_T sInitTime;

    // Time Setting
    sInitTime.u32Year       = 2015;
    sInitTime.u32Month      = 11;
    sInitTime.u32Day        = 29;
    sInitTime.u32Hour       = 9;
    sInitTime.u32Minute     = 15;
    sInitTime.u32Second     = 0;
    sInitTime.u32DayOfWeek  = RTC_SUNDAY;
    sInitTime.u32TimeScale  = RTC_CLOCK_24;

    RTC_Open(&sInitTime);
    RTC_SetTickPeriod(RTC_TICK_1_SEC);
    RTC_EnableInt(RTC_RIER_TIER_Msk);
    NVIC_EnableIRQ(RTC_IRQn);		  
}

int32_t main(void)
{
    SYS_Init();

    init_LCD();
    clear_LCD();
    print_Line(0, "RTC demo on LCD");

    Init_RTC();

    while(1);
}
