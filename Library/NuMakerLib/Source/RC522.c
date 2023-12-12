
#include <string.h>
#include <stdio.h>
#include "NUC100Series.h"
#include "SPI.h"
#include "RC522.h"

void initRC522(void)
{
	PcdReset();
	PcdAntennaOn();
}

//------------------------------------------------------------------
//Function : Find Card
//Parameter: req_code[IN] = sensing method
//                   0x52 = look for all cards that match 14443A standard 
//                   0x26 = look for non-sleep card
//          pTagType[OUT]：card type
//          0x4400 = Mifare_UltraLight
//          0x0400 = Mifare_One(S50)
//          0x0200 = Mifare_One(S70)
//          0x0800 = Mifare_Pro(X)
//          0x4403 = Mifare_DESFire
//Return:   TAG_OK if succeed
//------------------------------------------------------------------
char PcdRequest(uint8_t req_code,uint8_t *pTagType)
{
	char   status;
	uint8_t   unLen;
	uint8_t   ucComMF522Buf[MAXRLEN];

	WriteRawRC(BitFramingReg,0x07);
	ucComMF522Buf[0] = req_code;

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
	if ((status == TAG_OK) && (unLen == 0x10))
	{
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else if (status == TAG_COLLISION) {
//		printf("ATQA %02x%02x\n",ucComMF522Buf[0],ucComMF522Buf[1]);
	}
	else if (status!=TAG_NOTAG) {
		status = TAG_ERR;
	}

	return status;
}

//------------------------------------------------------------------
//Function : Avoid Collision
//Parameter: pSnr[OUT] = card serial number (4 bytes)
//Return   : TAG_OK if succeed
//------------------------------------------------------------------
char PcdAnticoll(uint8_t cascade, uint8_t *pSnr)
{
	char   status;
	uint8_t   i,snr_check=0;
	uint8_t   unLen;
	uint8_t   ucComMF522Buf[MAXRLEN];
	uint8_t   pass=32;
	uint8_t	  collbits=0;

	i=0;
	WriteRawRC(BitFramingReg,0x00);
	do {
		ucComMF522Buf[0] = cascade;
		ucComMF522Buf[1] = 0x20+collbits;
		//	WriteRawRC(0x0e,0);
		status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2+i,ucComMF522Buf,&unLen);
		if (status == TAG_COLLISION) {
			collbits=ReadRawRC(CollReg)&0x1f;
			if (collbits==0) collbits=32;
			i=(collbits-1)/8 +1;
//			printf ("--- %02x %02x %02x %02x %d\n",ucComMF522Buf[0],ucComMF522Buf[1],ucComMF522Buf[2],ucComMF522Buf[3],unLen);
			ucComMF522Buf[i-1]|=(1<<((collbits-1)%8));
			ucComMF522Buf[5]=ucComMF522Buf[3];
			ucComMF522Buf[4]=ucComMF522Buf[2];
			ucComMF522Buf[3]=ucComMF522Buf[1];
			ucComMF522Buf[2]=ucComMF522Buf[0];
			WriteRawRC(BitFramingReg,(collbits % 8));
//			printf (" %d %d %02x %d\n",collbits,i,ucComMF522Buf[i+1],collbits % 8);
		}
	} while (((--pass)>0)&&(status==TAG_COLLISION));

	if (status == TAG_OK)
	{
		for (i=0; i<4; i++)
		{
			*(pSnr+i)  = ucComMF522Buf[i];
			snr_check ^= ucComMF522Buf[i];
		}
		if (snr_check != ucComMF522Buf[i])
		{   status = TAG_ERR;    }
	}

	return status;
}

//------------------------------------------------------------------
//Function : Select Card
//Parameter: pSnr[IN] = card serial number (4 bytes)
//Return   : TAG_OK if succeed
//------------------------------------------------------------------
char PcdSelect(uint8_t cascade, uint8_t *pSnr)
{
	char   status;
	uint8_t   i;
	uint8_t   unLen;
	uint8_t   ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = cascade;
	ucComMF522Buf[1] = 0x70;
	ucComMF522Buf[6] = 0;
	for (i=0; i<4; i++)
	{
		ucComMF522Buf[i+2] = *(pSnr+i);
		ucComMF522Buf[6]  ^= *(pSnr+i);
	}
	CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);

	ClearBitMask(Status2Reg,0x08);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

	if ((status == TAG_OK) && (unLen == 0x18))
	{   status = TAG_OK;  }
	else
	{   status = TAG_ERR;    }

	return status;
}

//------------------------------------------------------------------
//Function : verify card password
//Parameter: auth_mode[IN] = password authentication mode
//                    0x60 = authentication key A
//                    0x61 = authentication key B  
//           addr[IN] = block address
//           pKey[IN] = password
//           pSnr[IN] = card serial number (4 bytes)
//Return   : TAG_OK if succeed
//------------------------------------------------------------------
char PcdAuthState(uint8_t   auth_mode,uint8_t   addr,uint8_t *pKey,uint8_t *pSnr)
{
	char   status;
	uint8_t   unLen;
	uint8_t   ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = auth_mode;
	ucComMF522Buf[1] = addr;
	memcpy(&ucComMF522Buf[2], pKey, 6);
	memcpy(&ucComMF522Buf[8], pSnr, 4);

	status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
	if ((status != TAG_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
	{   status = TAG_ERR;   }

	return status;
}

//------------------------------------------------------------------
//Function : read data from M1 card
//Parameter: addr[IN] = block address
//         pData[OUT] = read data (16 bytes)
//Return   : TAG_OK if succeed
//------------------------------------------------------------------
char PcdRead(uint8_t addr,uint8_t *p )
{
	char   status;
	uint8_t   unLen;
	uint8_t   i,ucComMF522Buf[MAXRLEN];
	uint8_t   CRC_buff[2];

	memset(ucComMF522Buf,0,sizeof(ucComMF522Buf));
	ucComMF522Buf[0] = PICC_READ;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	CalulateCRC(ucComMF522Buf,16,CRC_buff);

	if ((status == TAG_OK) && (unLen == 0x90))
	{
		if ((CRC_buff[0]!=ucComMF522Buf[16])||(CRC_buff[1]!=ucComMF522Buf[17])) { status = TAG_ERRCRC; }
		for (i=0; i<16; i++)
		{    *(p +i) = ucComMF522Buf[i];   }
	}
	else
	{   status = TAG_ERR;   }

	return status;
}

//------------------------------------------------------------------
//Function : read 1 block from M1 card
//Parameter: addr[IN] = block address
//         pData[OUT] = read data (16 bytes)
//Return   : TAG_OK if succeed
//------------------------------------------------------------------
char Read_Block(unsigned char Block,unsigned char *Buf)
{
  char result;
  result = PcdAuthState(0x60,Block,Password,UID);
  if(result!=TAG_OK)
    return result;
  result = PcdRead(Block, Buf);
  
  if(result!=TAG_OK)
	  return   result;
  if(Block!=0x00&&des_on)
  {
//    Des_Decrypt((char *)Buf    ,KK,(char *)Buf    );
//    Des_Decrypt((char *)&Buf[8],KK,(char *)&Buf[8]);  
  }
  return 1; 
}

//------------------------------------------------------------------
//Function : write data to M1 card
//Parameter: addr[IN] = block address
//         pData[OUT] = write data (16 bytes)
//Return   : TAG_OK if succeed
//------------------------------------------------------------------
char PcdWrite(uint8_t   addr,uint8_t *p )
{
	char   status;
	uint8_t   unLen;
	uint8_t   i,ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = PICC_WRITE;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

	if ((status != TAG_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
	{   status = TAG_ERR;   }

	if (status == TAG_OK)
	{
		for (i=0; i<16; i++)
		{
			ucComMF522Buf[i] = *(p +i);
		}
		CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

		status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
		if ((status != TAG_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
		{   status = TAG_ERR;   }
	}

	return status;
}

//------------------------------------------------------------------
//Function : write 1 block from M1 card
//Parameter: addr[IN] = block address
//         pData[OUT] = write data (16 bytes)
//Return   : TAG_OK if succeed
//------------------------------------------------------------------
char Write_Block(unsigned char Block)
{
  char result;
  if(des_on)
  {
//    Des_Encrypt((char *)RF_Buffer    ,KK,
//               (char *)RF_Buffer        );// for debug
//    Des_Encrypt((char *)&RF_Buffer[8],KK,
//               (char *)&RF_Buffer[8]    );// for debug  
  }
  result = PcdAuthState(0x60,Block,Password,UID)   ;
  if(result!=TAG_OK)
    return result;  
  result = PcdWrite(Block,RF_Buffer);
  return result;  
}

//------------------------------------------------------------------
//Function : Deduction & Charge
//Parameter: dd_mode[IN] = command
//           0xC0 = Deduction
//           0xC1 = Charge
//           addr[IN] = Wallet Address
//          pValue[IN]：4 bytes increment/decduction value (LSB first)
//Return   : TAG_OK if succeed
//------------------------------------------------------------------
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    char status;
    unsigned char unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != TAG_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = TAG_ERR;   }
        
    if (status == TAG_OK)
    {
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != TAG_ERR)
        {    status = TAG_OK;    }
    }
    
    if (status == TAG_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != TAG_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = TAG_ERR;   }
    }
    return status;
}

char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    char status;
    unsigned char unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != TAG_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = TAG_ERR;   }
    
    if (status == TAG_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != TAG_ERR)
        {    status = TAG_OK;    }
    }
    
    if (status != TAG_OK)
    {    return TAG_ERR;   }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != TAG_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = TAG_ERR;   }

    return status;
}

char PcdHalt(void)
{
	uint8_t status;
	uint8_t unLen;
	uint8_t ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = PICC_HALT;
	ucComMF522Buf[1] = 0;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

	return status;
}

//------------------------------------------------------------------
// MF522 CRC16 
//------------------------------------------------------------------
void CalulateCRC(uint8_t *pIn ,uint8_t   len,uint8_t *pOut )
{
	uint8_t   i,n;
	ClearBitMask(DivIrqReg,0x04);
	WriteRawRC(CommandReg,PCD_IDLE);
	SetBitMask(FIFOLevelReg,0x80);
	for (i=0; i<len; i++)
	{   WriteRawRC(FIFODataReg, *(pIn +i));   }
	WriteRawRC(CommandReg, PCD_CALCCRC);
	i = 0xFF;
	do
	{
		n = ReadRawRC(DivIrqReg);
		i--;
	}
	while ((i!=0) && !(n&0x04));
	pOut [0] = ReadRawRC(CRCResultRegL);
	pOut [1] = ReadRawRC(CRCResultRegM);
}

char PcdReset(void)
{
	WriteRawRC(CommandReg,PCD_RESETPHASE);
	CLK_SysTickDelay(10000);
	ClearBitMask(TxControlReg,0x03);
	CLK_SysTickDelay(10000);
	SetBitMask(TxControlReg,0x03);
	WriteRawRC(TModeReg,0x8D);
	WriteRawRC(TPrescalerReg,0x3E);
	WriteRawRC(TReloadRegL,30);
	WriteRawRC(TReloadRegH,0);
	WriteRawRC(TxASKReg,0x40);
	WriteRawRC(ModeReg,0x3D);            //6363
	//	WriteRawRC(DivlEnReg,0x90);
	WriteRawRC(RxThresholdReg,0x84);
	WriteRawRC(RFCfgReg,0x68);
	WriteRawRC(GsNReg,0xff);
	WriteRawRC(CWGsCfgReg,0x2f);
	//	WriteRawRC(ModWidthReg,0x2f);

	return TAG_OK;
}

/*
char M500PcdConfigISOType(uint8_t   type)
{
	if (type == 'A')
	{
		ClearBitMask(Status2Reg,0x08);
		WriteRawRC(ModeReg,0x3D);
		WriteRawRC(RxSelReg,0x86);
		WriteRawRC(RFCfgReg,0x7F);
		WriteRawRC(TReloadRegL,30);
		WriteRawRC(TReloadRegH,0);
		WriteRawRC(TModeReg,0x8D);
		WriteRawRC(TPrescalerReg,0x3E);
		PcdAntennaOn();
	}
	else{ return 1; }

	return TAG_OK;
}
 */

uint8_t ReadRawRC(uint8_t Address)
{
  uint8_t result;
  SPI_SET_SS0_LOW(RC522_SPI_PORT);
	
  SPI_WRITE_TX0(RC522_SPI_PORT, (Address<<1) | 0x80); // bit7 = 1 : Read
  SPI_TRIGGER(RC522_SPI_PORT);
  while(SPI_IS_BUSY(RC522_SPI_PORT));
	
  SPI_WRITE_TX0(RC522_SPI_PORT, 0xff);
  SPI_TRIGGER(RC522_SPI_PORT);
  while(SPI_IS_BUSY(RC522_SPI_PORT));
	
  result = RC522_SPI_PORT->RX[0];
  SPI_SET_SS0_HIGH(RC522_SPI_PORT);
	
  return result;	
}

void WriteRawRC(uint8_t Address, uint8_t Data)
{
  SPI_SET_SS0_LOW(RC522_SPI_PORT);
	
  SPI_WRITE_TX0(RC522_SPI_PORT, (Address<<1) & 0x7F); // bit7 = 0 : Write
  SPI_TRIGGER(RC522_SPI_PORT);
  while(SPI_IS_BUSY(RC522_SPI_PORT));

  SPI_WRITE_TX0(RC522_SPI_PORT, Data);
  SPI_TRIGGER(RC522_SPI_PORT);
  while(SPI_IS_BUSY(RC522_SPI_PORT));
	
  SPI_SET_SS0_HIGH(RC522_SPI_PORT);
}

void SetBitMask(uint8_t   reg,uint8_t   mask)
{
	char   tmp = 0x0;
	tmp = ReadRawRC(reg);
	WriteRawRC(reg,tmp | mask);  // set bit mask
}

void ClearBitMask(uint8_t   reg,uint8_t   mask)
{
	char   tmp = 0x0;
	tmp = ReadRawRC(reg);
	WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

char PcdComMF522(uint8_t   Command,
		uint8_t *pIn ,
		uint8_t InLenByte,
		uint8_t *pOut ,
		uint8_t *pOutLenBit)
{
	char   status = TAG_ERR;
	uint8_t   irqEn   = 0x00;
	uint8_t   waitFor = 0x00;
	uint8_t   lastBits;
	uint8_t   n;
	uint32_t   i;
	uint8_t PcdErr;

	//	printf("CMD %02x\n",pIn[0]);
	switch (Command)
	{
	case PCD_AUTHENT:
		irqEn   = 0x12;
		waitFor = 0x10;
		break;
	case PCD_TRANSCEIVE:
		irqEn   = 0x77;
		waitFor = 0x30;
		break;
	default:
		break;
	}

	WriteRawRC(ComIEnReg,irqEn|0x80);
	//	WriteRawRC(ComIEnReg,irqEn);
	ClearBitMask(ComIrqReg,0x80);
	SetBitMask(FIFOLevelReg,0x80);
	WriteRawRC(CommandReg,PCD_IDLE);

	for (i=0; i<InLenByte; i++) {
		WriteRawRC(FIFODataReg, pIn [i]);
	}

	WriteRawRC(CommandReg, Command);

	if (Command == PCD_TRANSCEIVE) {
		SetBitMask(BitFramingReg,0x80);
	}

	//i = 600;//���ʱ��Ƶ�ʵ������M1�����ȴ�ʱ��25ms
	i = 150;
	do
	{
		CLK_SysTickDelay(200);
		n = ReadRawRC(ComIrqReg);
		i--;
	}
	while ((i!=0) && (!(n&0x01)) && (!(n&waitFor)));

	ClearBitMask(BitFramingReg,0x80);

	if (i!=0)
	{
		PcdErr=ReadRawRC(ErrorReg);
		if (!(PcdErr & 0x11))
		{
			status = TAG_OK;
			if (n & irqEn & 0x01) {status = TAG_NOTAG;}
			if (Command == PCD_TRANSCEIVE) {
				n = ReadRawRC(FIFOLevelReg);
				lastBits = ReadRawRC(ControlReg) & 0x07;
				if (lastBits) {*pOutLenBit = (n-1)*8 + lastBits;}
				else {*pOutLenBit = n*8;}

				if (n == 0) {n = 1;}
				if (n > MAXRLEN) {n = MAXRLEN;}

				for (i=0; i<n; i++) {
					pOut [i] = ReadRawRC(FIFODataReg);
					//printf (".%02X ",pOut[i]);
				}
			}
		}
		else {
			//printf (stderr,"Err %02x\n",PcdErr);
			status = TAG_ERR;}

		if (PcdErr&0x08) {
			status = TAG_COLLISION;

		}

	}


	//    SetBitMask(ControlReg,0x80);           // stop timer now
	//    WriteRawRC(CommandReg,PCD_IDLE); ???????
//	printf ("PCD Err %02x\n",PcdErr);
	return status;
}

void PcdAntennaOn(void)
{
	uint8_t   i;
	i = ReadRawRC(TxControlReg);
	if (!(i & 0x03))
	{
		SetBitMask(TxControlReg, 0x03);
	}
}

void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}
