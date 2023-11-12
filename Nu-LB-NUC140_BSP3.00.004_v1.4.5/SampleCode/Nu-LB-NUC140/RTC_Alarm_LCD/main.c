//
// RTC_Alarm_LCD : RTC generate alarm interrrupt per EEPROM content
//
// external 32.768KHz clock source (LXT) needed
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

#define MAX_CMDLEN 5  //Maximum Command Lengthh
static uint8_t cmddat[MAX_CMDLEN] ={'F', 'B', 'L', 'R', 'S'};
static uint8_t timedat[MAX_CMDLEN]={  1,   2,   3,   4,  5};
	
volatile int32_t   g_bAlarm  = FALSE;
char Text[16];

void RTC_AlarmHandler(void)
{
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
	uint8_t cmd_count=0;
	uint8_t command, duration;
	
  S_RTC_TIME_DATA_T sInitTime;
  S_RTC_TIME_DATA_T sCurTime;

  SYS_Init();
	init_LCD();
	clear_LCD();
	
//
// Setup RTC
//	
    // Time Setting
    sInitTime.u32Year       = 2018;
    sInitTime.u32Month      = 5;
    sInitTime.u32Day        = 21;
    sInitTime.u32Hour       = 17;
    sInitTime.u32Minute     = 14;
    sInitTime.u32Second     = 0;
    sInitTime.u32DayOfWeek  = RTC_MONDAY;
    sInitTime.u32TimeScale  = RTC_CLOCK_24;

    RTC_Open(&sInitTime);
    RTC_SetTickPeriod(RTC_TICK_1_SEC);	
    RTC_EnableInt(RTC_RIER_AIER_Msk);   // Enable RTC Alarm Interrupt
    NVIC_EnableIRQ(RTC_IRQn);
    g_bAlarm = FALSE;

    cmd_count=0;
		while (cmd_count<MAX_CMDLEN) {
			
      // Get the current time
      RTC_GetDateAndTime(&sCurTime);

      sprintf(Text,"Date:%4d/%2d/%2d",sCurTime.u32Year,sCurTime.u32Month, sCurTime.u32Day);
		  print_Line(2, Text);
		  sprintf(Text,"Time:  %2d:%2d:%2d",sCurTime.u32Hour,sCurTime.u32Minute,sCurTime.u32Second);
		  print_Line(3, Text);
			
			command =cmddat[cmd_count];
			duration=timedat[cmd_count];
			
			sprintf(Text, "Command = %c ", command);
			print_Line(0, Text);
			sprintf(Text, "Duration= %3d", duration);
			print_Line(1, Text);
			
      // The alarm time setting
      sCurTime.u32Second = sCurTime.u32Second + duration;
		
      RTC_SetAlarmDateAndTime(&sCurTime); // Set the alarm time
			
      while(!g_bAlarm);
			g_bAlarm=FALSE;
			cmd_count++;
    }
		
    // Disable RTC Alarm Interrupt
    RTC_DisableInt(RTC_RIER_AIER_Msk);
    NVIC_DisableIRQ(RTC_IRQn);		
    while(1);
}
