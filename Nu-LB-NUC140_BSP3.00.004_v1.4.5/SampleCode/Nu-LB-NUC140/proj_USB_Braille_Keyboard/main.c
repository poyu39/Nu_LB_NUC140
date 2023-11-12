//
// proj_USB_Braille_Keyboard : USB Braille Keyboard
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// GPIO: to keypad

// Connections:
// PA0(pin71)=key-1         PA15(pin62)=key-Command  PA3(pin74)=key-4
// PA1(pin72)=key-2                                  PA4(pin75)=key-5
// PA2(pin73)=key-3         PA14(pin63)=key-Escape   PA5(pin76)=key-6
// PA6(pin77)=key-BackSpace PA13(pin64)=key-Space    PA7(pin78)=key-Enter

// key-Command + key-1         = key-Up
// key-Command + key-2         = key-Down
// Key-Command + key-3         = key-Left
// Key-Command + key-BackSpace = key-Right

// key-Command + key-4         = key-PageUp
// key-Command + key-5         = key-PageDown
// Key-Command + key-6         = key-Home
// Key-Command + key-Enter     = key-End

#include <stdio.h>
#include "NUC100Series.h"
#include "hid_kb.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
char Text[16];

uint8_t volatile g_u8EP2Ready = 0;
static uint16_t preKey;
uint8_t CapsLock_flag =0;
uint8_t Numeric_flag =0;

void Init_Keyboard(void)
{
  GPIO_SetMode(PA, 0xE0FF, GPIO_MODE_INPUT);		
}

void HID_UpdateKbData(void)
{
	int32_t i;
	uint8_t *buf;
	uint16_t Key[5];
	uint16_t volatile key, key_;
	uint8_t scancode;
	uint8_t caps_flag;
	
  caps_flag=0;
	scancode=0;
	
  if(g_u8EP2Ready)
  {
	  buf = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));	
	
    // software-debounced
	  Key[0] = (~PA->PIN)&0xE0FF; // read GPIO PA[15:13,7:0]
	  CLK_SysTickDelay(20000);    // delay 20ms
	  Key[1] = (~PA->PIN)&0xE0FF; // read GPIO PA[15:13,7:0]
	  CLK_SysTickDelay(20000);	  // delay 20ms
	  Key[2] = (~PA->PIN)&0xE0FF; // read GPIO PA[15:13,7:0]
	  CLK_SysTickDelay(20000);	  // delay 20ms
	  Key[3] = (~PA->PIN)&0xE0FF; // read GPIO PA[15:13,7:0]
	  CLK_SysTickDelay(20000);	  // delay 20ms
	  Key[4] = (~PA->PIN)&0xE0FF; // read GPIO PA[15:13,7:0]		
		
	  if (Key[0]==Key[1]&&Key[1]==Key[2]&&Key[2]==Key[3]&&Key[3]==Key[4])
	  { 
		  //key = (~PA->PIN)&0xE0FF; // read GPIO PA[15:13,7:0]
		  key_=Key[0];
		
      if(key_==0)
      {
        for(i = 0; i < 8; i++) buf[i] = 0;
        USBD_SET_PAYLOAD_LEN(EP2, 8); // Trigger to note key release			
      } else {
			
		    // Command +
		    if (key_==0x8001) scancode=0x52; // Up    Arrow
		    if (key_==0x8002) scancode=0x51; // Down  Arrow
		    if (key_==0x8004) scancode=0x50; // Left  Arrow
		    if (key_==0x8040) scancode=0x4F; // Right Arrow
				
		    if (key_==0x8008) scancode=0x4B; // PageUp
		    if (key_==0x8010) scancode=0x4E; // PageDown
		    if (key_==0x8020) scancode=0x4A; // Home
		    if (key_==0x8080) scancode=0x4D; // End
			
		    if (key_==0x4000) scancode=0x29; // Escape
				
		    if (key_==0x0040) scancode=0x2A; // BackSpace
		    if (key_==0x0080) scancode=0x28; // Return
				
		    if (key_==0x2000) {
			    scancode=0x2C;                 // Space
					CapsLock_flag =0;
		    }	

				if (scancode!=0) key =0;	
				else 		         key =key_ &0x003F; // Unified English Braille
					         
		    //alphabet
		    if (key==0x01) scancode=0x04; // a
		    if (key==0x03) scancode=0x05; // b 
		    if (key==0x09) scancode=0x06; // c
		    if (key==0x19) scancode=0x07; // d
		    if (key==0x11) scancode=0x08; // e
		    if (key==0x0B) scancode=0x09; // f 
		    if (key==0x1B) scancode=0x0A; // g
		    if (key==0x13) scancode=0x0B; // h
		    if (key==0x0A) scancode=0x0C; // i
		    if (key==0x1A) scancode=0x0D; // j
		    if (key==0x05) scancode=0x0E; // k
		    if (key==0x07) scancode=0x0F; // l
		    if (key==0x0D) scancode=0x10; // m
		    if (key==0x1D) scancode=0x11; // n
		    if (key==0x15) scancode=0x12; // o
		    if (key==0x0F) scancode=0x13; // p
		    if (key==0x1F) scancode=0x14; // q
		    if (key==0x17) scancode=0x15; // r
		    if (key==0x0E) scancode=0x16; // s
		    if (key==0x1E) scancode=0x17; // t
		    if (key==0x25) scancode=0x18; // u
		    if (key==0x27) scancode=0x19; // v
		    if (key==0x3A) scancode=0x1A; // w	
		    if (key==0x2D) scancode=0x1B; // x
		    if (key==0x3D) scancode=0x1C; // y
		    if (key==0x35) scancode=0x1D; // z	
		    //digit
				
		    if (Numeric_flag==1){
		      if      (key==0x01) scancode=0x1E; // 1
		      else if (key==0x03) scancode=0x1F; // 2 
		      else if (key==0x09) scancode=0x20; // 3
		      else if (key==0x19) scancode=0x21; // 4
		      else if (key==0x11) scancode=0x22; // 5
		      else if (key==0x0B) scancode=0x23; // 6 
		      else if (key==0x1B) scancode=0x24; // 7
		      else if (key==0x13) scancode=0x25; // 8
		      else if (key==0x0A) scancode=0x26; // 9
		      else if (key==0x1A) scancode=0x27; // 0
					else                Numeric_flag=0;
		    }
				if (key==0x3C) Numeric_flag=1;
				
		    if (key==0x02) scancode=0x36; // ,
		    if (key==0x32) scancode=0x37; // .			
		    if (key==0x06) scancode=0x33; // ; 
		    if (key==0x04) scancode=0x34; // ' single quotation mark
		    if (key==0x24) scancode=0x2D; // -
			
	      if (key==0x12) {              // :
	        scancode=0x33;			
		      buf[2] = 0x82; // send Caps first
		      USBD_SET_PAYLOAD_LEN(EP2, 8);
					caps_flag=1;
        }
		    if (key==0x16) {              // !
			    scancode=0x1E;			
			    buf[2] = 0x82; // send Caps first
		      USBD_SET_PAYLOAD_LEN(EP2, 8);
					caps_flag=1;
        }
		    if (key==0x26) {              // ?
			    scancode=0x34;
			    buf[2] = 0x82; // send Caps first
		      USBD_SET_PAYLOAD_LEN(EP2, 8);
					caps_flag=1;
        }
				// special Braille in preKey			
		    if (preKey==0x08) {             // special Braille
		      if (key==0x01) scancode=0x1F; // @
		      if (key==0x23) {              // <
			      scancode=0x36;                  
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
				    caps_flag=1;
			    }
		      if (key==0x22) {                // ^
			      scancode=0x23;                  
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
			      caps_flag=1;
		      }
		      if (key==0x14) {                // ~
			      scancode=0x35;                  
		        buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
			      caps_flag=1;
		      }	
		      if (key==0x1C) {                // >
			      scancode=0x37;                  
		        buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
			      caps_flag=1;
		      }
		      if (key==0x2F) {                // & Ampersand
		        scancode=0x24;                  
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
			      caps_flag=1;
		      }					
        }
				// special Braille in preKey		
		    if (preKey==0x38) {             // special Braille
		      if (key==0x21) scancode=0x31; // "\"
			    if (key==0x0C) scancode=0x38; // "/"			
			    if (key==0x1C) {              // { opening curly bracket
				    scancode=0x2F;                  
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
			      caps_flag=1;
			    }	
			    if (key==0x39) {              // #
				    scancode=0x20;
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
			      caps_flag=1;
			    }
			    if (key==0x33) {              // |
				    scancode=0x31;
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
				    caps_flag=1;
			    }
			    if (key==0x1C) {              // } closing curly bracket
				    scancode=0x30;
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
				    caps_flag=1;
			    }					
		    }
				// special Braille in preKey
		    if (preKey==0x10) {             
			    if (key==0x23) {              // ( opening parenthesis
				    scancode=0x26;
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
				    caps_flag=1;
			    }	
			    if (key==0x1C) {              // ) closing parenthesis
				    scancode=0x27;
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
				    caps_flag=1;
			    }	
			    if (key==0x16) {              // + plus
				    scancode=0x2E;
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
				    caps_flag=1;
			    }
			    if (key==0x36) scancode=0x2E; // = equals	
		  	  if (key==0x16) {              // * asterisk
				    scancode=0x25;
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
				    caps_flag=1;
			    }				
		    }
		    if (preKey==0x28) {             // special Braille
			    if (key==0x23) scancode=0x2F; // [ opening square bracket
			    if (key==0x1C) scancode=0x30; // ] closing square bracket			           
			    if (key==0x24) {              // underscore
				    scancode=0x2D;
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
				    caps_flag=1;
			    }	
			    if (key==0x34) {              // % percent
				    scancode=0x22;
			      buf[2] = 0x82; // send Caps first
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
				    caps_flag=1;
			    }					
		    }
			 
		    if (key==0x36) {                // " double quotation mark
			    scancode=0x34;
		      buf[2] = 0x82; // send Caps first
		      USBD_SET_PAYLOAD_LEN(EP2, 8);
			    caps_flag=1;
		    }
				
		    if (key==0x08) { 
		      if (preKey==0x08) { // preKey and key : Caps_Lock
		        buf[2] = 0x82;			
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
		  	    CapsLock_flag=1;					
          } 
		    } else {              // not preKey and key
			    if ((CapsLock_flag!=1)&&(preKey==0x08)) caps_flag=1;
		    }
			
        // send scancode if not digit, not special Braille
				if (scancode!=0) {
				  if (!((key==0x3C)||(key==0x08)||(key==0x38)||(key==0x10)||(key==0x28)||(key==0x20))) {
		        buf[2] = scancode;			
		        USBD_SET_PAYLOAD_LEN(EP2, 8);
					}
        }
			
		    if (caps_flag==1){
			    buf[2] = 0x82; // release Caps
		      USBD_SET_PAYLOAD_LEN(EP2, 8);
		    }
			
		    // store preKey	
		    if (scancode==0x2C) preKey=0;
		    else                preKey=key;
				
				// print on LCD
			  sprintf(Text,"scan= %4x", scancode);
		    print_Line(1,Text);				
		    sprintf(Text,"key_= %4x", key_);
		    print_Line(2,Text);			
		    sprintf(Text,"key = %4x", key);
		    print_Line(3,Text);

	    }
    }
  }
}

int32_t main(void)
{
	SYS_Init();
  init_LCD();
  clear_LCD();
  print_Line(0, "Braille keyboard");
	
	Init_Keyboard();
  USBD_Open(&gsInfo, HID_ClassRequest, NULL);

  // Endpoint configuration
  HID_Init();
  USBD_Start();
  NVIC_EnableIRQ(USBD_IRQn);

  // start to IN data
  g_u8EP2Ready = 1;

  while(1)
  {
		HID_UpdateKbData();
  }
}
