//
// RTC_Alarm : RTC generate alarm interrrupt in 5 second
//
// external 32.768KHz clock source (LXT) needed
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

volatile int32_t   g_bAlarm  = FALSE;

void RTC_AlarmHandler(void)
{
    printf(" Alarm!!!\n");
    g_bAlarm = TRUE;
}

void RTC_IRQHandler(void)
{
    if ( (RTC->RIER & RTC_RIER_AIER_Msk) && (RTC->RIIR & RTC_RIIR_AIF_Msk) ) {      /* alarm interrupt occurred */
        RTC->RIIR = 0x1;

        RTC_AlarmHandler();
    }
}

int32_t main(void)
{
    S_RTC_TIME_DATA_T sInitTime;
    S_RTC_TIME_DATA_T sCurTime;

    SYS_Init();

    // Time Setting
    sInitTime.u32Year       = 2015;
    sInitTime.u32Month      = 4;
    sInitTime.u32Day        = 26;
    sInitTime.u32Hour       = 12;
    sInitTime.u32Minute     = 30;
    sInitTime.u32Second     = 0;
    sInitTime.u32DayOfWeek  = RTC_SUNDAY;
    sInitTime.u32TimeScale  = RTC_CLOCK_24;

    RTC_Open(&sInitTime);

    printf("RTC Alarm Test Begin...\n");
	  printf("(Alarm after 5 seconds)\n\n");

    g_bAlarm = FALSE;

    // Get the current time
    RTC_GetDateAndTime(&sCurTime);

    printf(" Current Time:%d/%02d/%02d %02d:%02d:%02d\n",sCurTime.u32Year,sCurTime.u32Month,
           sCurTime.u32Day,sCurTime.u32Hour,sCurTime.u32Minute,sCurTime.u32Second);

    // The alarm time setting
    sCurTime.u32Second = sCurTime.u32Second + 5;
    
    RTC_SetAlarmDateAndTime(&sCurTime); // Set the alarm time
    RTC_EnableInt(RTC_RIER_AIER_Msk);   // Enable RTC Alarm Interrupt
    NVIC_EnableIRQ(RTC_IRQn);

    while(!g_bAlarm);

    printf(" Current Time:%d/%02d/%02d %02d:%02d:%02d\n",sCurTime.u32Year,sCurTime.u32Month,
           sCurTime.u32Day,sCurTime.u32Hour,sCurTime.u32Minute,sCurTime.u32Second);

    // Disable RTC Alarm Interrupt
    RTC_DisableInt(RTC_RIER_AIER_Msk);
    NVIC_DisableIRQ(RTC_IRQn);

    printf("\nRTC Alarm Test End...\n");
    while(1);

}
