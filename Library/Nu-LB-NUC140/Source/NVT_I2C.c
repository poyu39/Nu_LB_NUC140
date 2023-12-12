#include <stdio.h>
#include "NUC100Series.h"
#include "NVT_I2C.h"

#define I2C_PORT I2C0
#define BUSERROR    1
#define ARBITLOSS   2
#define TIMEOUT     4
#define I2CFUNC TIMEOUT
#define DEBUG_PRINT printf
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t Device_W_Addr;
uint8_t Device_R_Addr;
uint8_t ReadFlag;

uint8_t Tx_Data0[MAX_I2C_DATA_LEN];
uint8_t Rx_Data0[MAX_I2C_DATA_LEN];
uint8_t DataLen0;
uint8_t RxLen0;
uint8_t ContinueLen;
volatile uint8_t EndFlag0 = 0;
volatile uint8_t ErrorFlag = 0;
uint8_t  Addr1[3] = {0};
uint8_t  DataLen1;
uint8_t  Slave_Buff1[32] = {0};
uint16_t Slave_Buff_Addr1;
//extern void DelayUsec(unsigned int usec);
//extern void DelayMsec(unsigned int usec);

typedef void (*I2C_FUNC)(uint32_t u32Status);
static volatile I2C_FUNC s_I2C0HandlerFn = NULL;
static volatile I2C_FUNC s_I2C1HandlerFn = NULL;

void DelayUsec(uint32_t us)
{
	CLK_SysTickDelay(us);
}

void DelayMsec(uint32_t ms)
{
	CLK_SysTickDelay(ms * 1000);
}

void I2CX_IRQHandler(uint8_t id)
{
	uint32_t u32Status;

	u32Status = I2C_GET_STATUS(I2C_PORT);

	if(I2C_GET_TIMEOUT_FLAG(I2C_PORT)) {
	  ErrorFlag = 1;
	  /* Clear I2C0 Timeout Flag */
	  I2C_ClearTimeoutFlag(I2C_PORT);
	}	else {
		switch (u32Status) {	
			case 0x38:{/* Arbitration loss */
				I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_STO_SI); 
				I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_STA);
				//printf("I2C Arbitration loss\n");
				break;}
			case 0x00:	/* Bus error */
			case 0x30:
			case 0xF8:
			case 0x48: {
				I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_STO_SI);
				//I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_SI);
				I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_STA);
				//printf("I2C bus error\n");
				break;}
			default: {
				if(id==0) {
				  if(s_I2C0HandlerFn != NULL)
					  s_I2C0HandlerFn(u32Status);
			  }	
				else if(id==1) {
					if(s_I2C1HandlerFn != NULL)
						s_I2C1HandlerFn(u32Status);
		        }
	      }
       }
	}
}

void I2C0_IRQHandler(void)
{
	I2CX_IRQHandler(0);
}
void I2C1_IRQHandler(void)
{
	I2CX_IRQHandler(1);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C (Master) Rx Callback Function                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_Callback_Rx(uint32_t status)
{
    if (status == 0x08)                     /* START has been transmitted and prepare SLA+W */
    {
        I2C_SetData(I2C_PORT, Device_W_Addr<<1);
        I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }   
    else if (status == 0x18)                /* SLA+W has been transmitted and ACK has been received */
    {
      I2C_SetData(I2C_PORT, Tx_Data0[DataLen0++]);
      I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }
    else if (status == 0x20)                /* SLA+W has been transmitted and NACK has been received */
    {
      I2C_Ctrl(I2C_PORT, 0, 1, 1, 0);
    }
    else if (status == 0x28)                /* DATA has been transmitted and ACK has been received */
    {
        if (DataLen0 != 2)
        {
          I2C_SetData(I2C_PORT, Tx_Data0[DataLen0++]);
          I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
        }
        else
        {
          I2C_Ctrl(I2C_PORT, 1, 0, 1, 0);
          EndFlag0 = 1;
        }
    }
    else if (status == 0x10)                /* Repeat START has been transmitted and prepare SLA+R */
    {
      I2C_SetData(I2C_PORT, Device_W_Addr<<1 | 0x01);
      I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }
    else if (status == 0x40)                /* SLA+R has been transmitted and ACK has been received */
    {
      I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }
    else if (status == 0x58)                /* DATA has been received and NACK has been returned */
    {
      Rx_Data0[0] = I2C_GetData(I2C_PORT);
      I2C_Ctrl(I2C_PORT, 0, 1, 1, 0);
      EndFlag0 = 1;
    }
    else
    {
        DEBUG_PRINT("Status 0x%x is NOT processed\n", status);
    }           
}
void I2C_Callback_Rx_Continue(uint32_t status)
{
    if (status == 0x08)                     /* START has been transmitted and prepare SLA+W */
    {
        I2C_SetData(I2C_PORT, Device_W_Addr<<1|ReadFlag);
        I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }   
    else if (status == 0x18)                /* SLA+W has been transmitted and ACK has been received */
    {
      I2C_SetData(I2C_PORT, Tx_Data0[DataLen0++]);
      I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }
    else if (status == 0x20)                /* SLA+W has been transmitted and NACK has been received */
    {
      I2C_Ctrl(I2C_PORT, 0, 1, 1, 0);
    }
    else if (status == 0x28)                /* DATA has been transmitted and ACK has been received */
    {
      I2C_Ctrl(I2C_PORT, 1, 0, 1, 0);
    }
    else if (status == 0x10)                /* Repeat START has been transmitted and prepare SLA+R */
    {
      I2C_SetData(I2C_PORT, Device_W_Addr<<1 | 0x01);
      I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }
    else if (status == 0x40)                /* SLA+R has been transmitted and ACK has been received */
    {
      if(ContinueLen>1)
        I2C_Ctrl(I2C_PORT, 0, 0, 1, 1);
      else 
        I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }       
    else if (status == 0x58)                /* DATA has been received and NACK has been returned */
    {
      Rx_Data0[RxLen0++] = I2C_GetData(I2C_PORT);
      I2C_Ctrl(I2C_PORT, 0, 1, 1, 0);
      EndFlag0 = 1;
    }
    else if (status == 0x50)                /* DATA has been received and ACK has been returned */
    {
      Rx_Data0[RxLen0++] = I2C_GetData(I2C_PORT);
      if(RxLen0<(ContinueLen-1))
        I2C_Ctrl(I2C_PORT, 0, 0, 1, 1);
      else
        I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }
    else
    {
        DEBUG_PRINT("Status 0x%x is NOT processed\n", status);
    }           
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C (Master) Tx Callback Function                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_Callback_Tx(uint32_t status)
{
    if (status == 0x08)                     /* START has been transmitted */
    {
      I2C_SetData(I2C_PORT, Device_W_Addr<<1);
      I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }   
    else if (status == 0x18)                /* SLA+W has been transmitted and ACK has been received */
    {
      I2C_SetData(I2C_PORT, Tx_Data0[DataLen0++]);
      I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }
    else if (status == 0x20)                /* SLA+W has been transmitted and NACK has been received */
    {
      I2C_Ctrl(I2C_PORT, 1, 1, 1, 0);
    }   
    else if (status == 0x28)                /* DATA has been transmitted and ACK has been received */
    {
        if (DataLen0 != 3)
        {
          I2C_SetData(I2C_PORT, Tx_Data0[DataLen0++]);
          I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
        }
        else
        {
          I2C_Ctrl(I2C_PORT, 0, 1, 1, 0);   
          EndFlag0 = 1;
        }       
    }
    else
    {
        DEBUG_PRINT("Status 0x%x is NOT processed\n", status);
    }       
}
void I2C_Callback_Tx_Continue(uint32_t status)
{
    if (status == 0x08)                     /* START has been transmitted */
    {
      I2C_SetData(I2C_PORT, Device_W_Addr<<1);
      I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }   
    else if (status == 0x18)                /* SLA+W has been transmitted and ACK has been received */
    {
      I2C_SetData(I2C_PORT, Tx_Data0[DataLen0++]);
      I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
    }
    else if (status == 0x20)                /* SLA+W has been transmitted and NACK has been received */
    {
      I2C_Ctrl(I2C_PORT, 1, 1, 1, 0);
    }   
    else if (status == 0x28)                /* DATA has been transmitted and ACK has been received */
    {
        if (DataLen0 != ContinueLen)
        {
          I2C_SetData(I2C_PORT, Tx_Data0[DataLen0++]);
          I2C_Ctrl(I2C_PORT, 0, 0, 1, 0);
        }
        else
        {
          I2C_Ctrl(I2C_PORT, 0, 1, 1, 0);        
          EndFlag0 = 1;
        }       
    }
    else
    {
        DEBUG_PRINT("Status 0x%x is NOT processed\n", status);
    }       
}

void ResetPort() 
{
	I2C_Open(I2C_PORT, 400000);
	I2C_EnableTimeout(I2C_PORT, 1);
	I2C_EnableInt(I2C_PORT);
	DelayMsec(4);
	ErrorFlag = 0;
	I2C_ClearTimeoutFlag(I2C_PORT);
}

void WaitEndFlag0(uint16_t timeout)
{
	if(timeout)
		I2C_EnableTimeout(I2C_PORT, (uint8_t) timeout);
	
	while (EndFlag0 == 0) {
		if(ErrorFlag) {
			break;
		}
	};
	I2C_DisableTimeout(I2C_PORT);
}

void NVT_WriteByte(uint16_t address,uint8_t data)
{
	Tx_Data0[0]=address>>8;
	Tx_Data0[1]=(address&0xff);
	Tx_Data0[2] = data;
	DataLen0 = 0;
	EndFlag0 = 0;
	s_I2C0HandlerFn = (I2C_FUNC)I2C_Callback_Tx;
	s_I2C1HandlerFn = (I2C_FUNC)I2C_Callback_Tx;
	while(I2C_PORT->I2CON & I2C_I2CON_STO_Msk);
	I2C_Ctrl(I2C_PORT, 1, 0, 0, 0);
	WaitEndFlag0(0);
}

void NVT_WriteByteContinue(uint16_t address,uint8_t* data, uint8_t len)
{
	uint8_t i;
	uint8_t check_sum=0;
	Tx_Data0[0]=address>>8;
	Tx_Data0[1]=(address&0xff);
	for(i=0;i<len;i++)
		Tx_Data0[2+i]=data[i];

	DataLen0 = 0;
	EndFlag0 = 0;
	ContinueLen=len+2;
	s_I2C0HandlerFn = (I2C_FUNC)I2C_Callback_Tx_Continue;
	s_I2C1HandlerFn = (I2C_FUNC)I2C_Callback_Tx_Continue;
	while(I2C_PORT->I2CON & I2C_I2CON_STO_Msk);	
	I2C_Ctrl(I2C_PORT, 1, 0, 0, 0);

	WaitEndFlag0(0);
	for(i=0;i<len;i++) {
		if((address+i)<0x80ff)
			check_sum+=Tx_Data0[2+i];
	}
	check_sum=(check_sum^0xff)+1;
}

void NVT_WriteMultiBytes(uint16_t address,uint8_t* data, uint8_t len)
{
	uint16_t i=0;
	uint8_t check_sum=0;
	for (i = 0; i < len; i++) {
		NVT_WriteByte(address+i,data[i])	;
		check_sum+=data[i];
	}
	check_sum=(check_sum^0xff)+1;
}

void NVT_ReadByte(uint16_t address, uint8_t *data)
{
	Tx_Data0[0]=address>>8;
	Tx_Data0[1]=(address&0xff);
	DataLen0 = 0;
	EndFlag0 = 0;
	s_I2C0HandlerFn = (I2C_FUNC)I2C_Callback_Rx;
	s_I2C1HandlerFn = (I2C_FUNC)I2C_Callback_Rx;	
	I2C_Ctrl(I2C_PORT, 1, 0, 0, 0);

	while (EndFlag0 == 0);
	*data=Rx_Data0[0];
}

void NVT_ReadMultiBytes(uint16_t address,uint8_t* data, uint8_t len)
{
	uint16_t i=0;
	uint8_t check_sum=0;
	for (i = 0; i < len; i++) {
		NVT_ReadByte(address+i,&data[i]);
		check_sum+=data[i];
	}
	check_sum=(check_sum^0xff)+1;
}

void NVT_ReadByteContinue(uint16_t address,uint8_t* data, uint8_t len)
{
	uint8_t i;

	for(i=0;i<len;i++) {
		RxLen0 = 0;
		DataLen0 = 0;
		EndFlag0 = 0;
		ReadFlag=0;
		ContinueLen=2;
		
		Tx_Data0[0]=address>>8;
		Tx_Data0[1]=(address&0xff);
	  s_I2C0HandlerFn = (I2C_FUNC)I2C_Callback_Rx_Continue;
	  s_I2C1HandlerFn = (I2C_FUNC)I2C_Callback_Rx_Continue;				
		I2C_Ctrl(I2C_PORT, 1, 0, 0, 0);
		
		WaitEndFlag0(0);
		ContinueLen=1;
		ReadFlag=1;
		EndFlag0 = 0;
		I2C_Ctrl(I2C_PORT, 1, 0, 0, 0);
		
		WaitEndFlag0(0);
		data[i]=Rx_Data0[0];
		address++;
	}
}

uint8_t NVT_WriteByteContinue_addr8(uint8_t address,uint8_t* data, uint8_t len)
{
	uint8_t i;
	
	Tx_Data0[0]=address;
	
	for(i=0;i<len;i++)
		Tx_Data0[1+i]=data[i];
	
	DataLen0 = 0;
	EndFlag0 = 0;
	ErrorFlag = 0;
	ContinueLen=len+1;
	s_I2C0HandlerFn = (I2C_FUNC)I2C_Callback_Tx_Continue;
	s_I2C1HandlerFn = (I2C_FUNC)I2C_Callback_Tx_Continue;	
	while(I2C_PORT->I2CON & I2C_I2CON_STO_Msk);
	I2C_Ctrl(I2C_PORT, 1, 0, 0, 0);
	WaitEndFlag0(1);
	
	return ErrorFlag;
}

uint8_t NVT_ReadByteContinue_addr8(uint8_t address,uint8_t* data, uint8_t len)
{
	uint8_t i;

	RxLen0 = 0;
	DataLen0 = 0;
	EndFlag0 = 0;
	ReadFlag = 0;
	ErrorFlag = 0;
	ContinueLen = len;
	Tx_Data0[0] = address;
		
	s_I2C0HandlerFn = (I2C_FUNC)I2C_Callback_Rx_Continue;
	s_I2C1HandlerFn = (I2C_FUNC)I2C_Callback_Rx_Continue;	
	while(I2C_PORT->I2CON & I2C_I2CON_STO_Msk);
	I2C_Ctrl(I2C_PORT, 1, 0, 0, 0);
	WaitEndFlag0(0);
	
	for(i = 0; i<len; i++)
		data[i]=Rx_Data0[i];
		
	return ErrorFlag;
}

void NVT_I2C_Init(uint32_t u32BusFreq)
{
	I2C_Open(I2C_PORT, u32BusFreq);
	//u32data = I2C_GetClockFreq(I2C_PORT);
	//printf("I2C clock: %d Hz\n", u32data);
	/* Enable I2C interrupt and set corresponding NVIC bit */
	I2C_EnableInt(I2C_PORT);
	if(I2C_PORT==I2C0) NVIC_EnableIRQ(I2C0_IRQn);	
	if(I2C_PORT==I2C1) NVIC_EnableIRQ(I2C1_IRQn);
}

void NVT_SetDeviceAddress(uint8_t devAddr)
{
	Device_W_Addr=devAddr;
	Device_R_Addr=devAddr+1;
}

