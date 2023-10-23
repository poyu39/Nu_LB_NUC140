//
// PowerDown calls
//
#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "sys.h"

uint32_t Tmp_NVIC_ISER;

// Global variables
__IO int32_t   _Wakeup_Flag = 0;    /* 1 indicates system wake up from power down mode */
__IO uint32_t  _Pin_Setting[9];    /* store Px_H_MFP and Px_L_MFP */
__IO uint32_t  _PullUp_Setting[6];  /* store GPIOx_PUEN */

// Store original setting of multi-function pin selection.
void SavePinSetting(void)
{
    // Save Pin selection setting 
    _Pin_Setting[0] = SYS->GPA_MFP;
    _Pin_Setting[1] = SYS->GPB_MFP;
    _Pin_Setting[2] = SYS->GPC_MFP;
    _Pin_Setting[3] = SYS->GPD_MFP;
    _Pin_Setting[4] = SYS->GPE_MFP;
    _Pin_Setting[5] = SYS->GPF_MFP;
    _Pin_Setting[6] = SYS->ALT_MFP;
    _Pin_Setting[7] = SYS->ALT_MFP1;
    _Pin_Setting[8] = SYS->ALT_MFP2;

    // Save Pull-up setting 
    //_PullUp_Setting[0] =  PA->PUEN;
    //_PullUp_Setting[1] =  PB->PUEN;
    //_PullUp_Setting[2] =  PC->PUEN;
    //_PullUp_Setting[3] =  PD->PUEN;
    //_PullUp_Setting[4] =  PE->PUEN;
    //_PullUp_Setting[5] =  PF->PUEN;
}

void RestorePinSetting(void)
{
    // Restore Pin selection setting
    SYS->GPA_MFP = _Pin_Setting[0];
    SYS->GPB_MFP = _Pin_Setting[1];
    SYS->GPC_MFP = _Pin_Setting[2];
    SYS->GPD_MFP = _Pin_Setting[3];
    SYS->GPE_MFP = _Pin_Setting[4];
    SYS->GPF_MFP = _Pin_Setting[5];
    SYS->ALT_MFP = _Pin_Setting[6];
    SYS->ALT_MFP1 = _Pin_Setting[7];
    SYS->ALT_MFP2 = _Pin_Setting[8];

    // Restore Pull-up setting
    //PA->PUEN = _PullUp_Setting[0];
    //PB->PUEN = _PullUp_Setting[1];
    //PC->PUEN = _PullUp_Setting[2];
    //PD->PUEN = _PullUp_Setting[3];
    //PE->PUEN = _PullUp_Setting[4];
    //PF->PUEN = _PullUp_Setting[5];
}

void Enter_PowerDown(void)
{
    // Back up original setting 
	  SavePinSetting();

    SYS_UnlockReg();
	  CLK_PowerDown();
	  SYS_LockReg();
}

void Leave_PowerDown()
{
    RestorePinSetting();
}
