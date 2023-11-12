
#include "rfid.h"
#include "LCD.h"

uint8_t buff[MAXRLEN];

tag_stat find_tag(uint16_t *cardType) {
	tag_stat status;
	
	status=PcdRequest(PICC_REQIDL,buff);
	if (status==TAG_OK) {
		*cardType = buff[0]<<8 | buff[1];
	}
	return status;
}

tag_stat select_tag_sn(uint8_t * sn, uint8_t * len){

	if (PcdAnticoll(PICC_ANTICOLL1,buff)!=TAG_OK) {return TAG_ERR;}
	if (PcdSelect(PICC_ANTICOLL1,buff)!=TAG_OK) {return TAG_ERR;}
	if (buff[0]==0x88) {
		memcpy(sn,&buff[1],3);
		if (PcdAnticoll(PICC_ANTICOLL2,buff)!=TAG_OK) {
			return TAG_ERR;}
		if (PcdSelect(PICC_ANTICOLL2,buff)!=TAG_OK) {return TAG_ERR;}
		if (buff[0]==0x88) {
			memcpy(sn+3,&buff[1],3);
			if (PcdAnticoll(PICC_ANTICOLL3,buff)!=TAG_OK) {
				return TAG_ERR;}
			if (PcdSelect(PICC_ANTICOLL3,buff)!=TAG_OK) {return TAG_ERR;}
			memcpy(sn+6,buff,4);
			*len=10;
		}else{
			memcpy(sn+3,buff,4);
			*len=7;
		}
	}else{
		memcpy(sn,&buff[0],4);
		*len=4;
	}
	return TAG_OK;
}

tag_stat read_tag_str(uint8_t addr, char * str) {
	tag_stat tmp;
	char text1[16], text2[16];
	uint8_t i;

	uint8_t buff[MAXRLEN];

	tmp=PcdRead(addr,buff);
	if (tmp==TAG_OK) {
		for (i=0;i<8;i++) {
			sprintf(text1,"%2x",(char)buff[i]);
			*text1+=2;
		}
		for (i=8;i<16;i++) {
			sprintf(text2,"%2x",(char)buff[i]);
			*text2+=2;
		}		
	}else if (tmp==TAG_ERRCRC){
		sprintf(text1,"CRC Error");
		print_Line(3, text1);
	}else{
		sprintf(text1,"Unknown error");
		print_Line(3, text1);
	}
	return tmp;
}
