//
// LCD_Game_AngryBird : emulate AngryBird game on LCD
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
//
// Game States:
// IDLE_STATE  - select a bird
// SHOW_STATE  - show bird & pig
// ANGLE_STATE - angle the bird and wait for firing
// SHOOT_STATE - bird is flying & detect if hit the pig
// HIT_STATE   - when bird reach ground, check hit/miss
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"

#define PI  3.1415926

#define SCENE_Xmax  256
#define PIG_Xsize   32
#define PIG_Ysize   32
#define BIRD_Xsize  16
#define BIRD_Ysize  16

#define IDLE_STATE  0 // to select a bird
#define SHOW_STATE  1 // show bird & pig
#define ANGLE_STATE 2 // angle the bird to shoot
#define SHOOT_STATE 3 // bird flying to hit pig
#define HIT_STATE   4 // hit/miss & retry

#define G   0.2 // Gravity
// bitmap is in upside down order
unsigned char bmp_PIG[32*4] = {
	0x00,0x00,0x00,0x00,0x78,0xD8,0xFC,0x5C,0x38,0x38,0x30,0x10,0x10,0x10,0x10,0x1C,0x16,0x1B,0x17,0x1B,0x3E,0x30,0x60,0x60,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
	0x80,0xF0,0x18,0x0C,0x17,0x91,0x90,0x00,0x00,0x00,0x00,0x80,0xC0,0x60,0x20,0x20,0x20,0x20,0x20,0x60,0xC0,0x00,0x00,0x00,0x00,0x81,0x93,0x97,0x9E,0x30,0xE0,0x80,
	0x1F,0xF9,0x80,0x0C,0x3F,0x2C,0x2C,0x31,0x13,0x1E,0x33,0x41,0x9C,0x12,0x12,0x0C,0x80,0x80,0x9C,0x84,0x9C,0x41,0x76,0x10,0x1F,0x11,0x30,0x2C,0x1F,0x1A,0xF1,0x7F,
	0x00,0x00,0x03,0x07,0x1C,0x10,0x10,0x30,0x20,0x60,0x40,0x40,0xC1,0xC2,0x84,0x84,0x84,0x84,0x86,0xC2,0xC1,0x40,0x60,0x60,0x20,0x30,0x10,0x10,0x1E,0x07,0x01,0x00
};
unsigned char bmp_BIRD1[16*2] = {
	0x00,0x80,0xC0,0xF0,0xFC,0xFE,0x3E,0x3E,0xFC,0xFC,0xF8,0x30,0x30,0xC0,0x80,0x00,
	0x01,0x01,0x0F,0x1F,0x3F,0x7F,0x4F,0x4F,0x47,0x4D,0x4D,0x7F,0x3F,0x3F,0x0F,0x00
};
unsigned char bmp_BIRD2[16*2] = {
	0x00,0xC0,0xE0,0xF0,0xF0,0xF8,0x9C,0xFC,0xF7,0xFC,0xD8,0xF0,0xF0,0xC0,0x80,0x00,
	0x00,0x1F,0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0xFE,0xFF,0x7F,0x3F,0x1F,0x00
};
unsigned char bmp_BIRD3[16*2] = {
	0x00,0x00,0x00,0x00,0xC0,0x60,0x10,0x0E,0x0C,0x08,0x30,0x60,0x80,0x00,0x00,0x00,
	0x00,0x7E,0x5C,0x43,0x80,0x80,0x80,0x87,0x8F,0x9E,0xAE,0x9E,0x9F,0x8F,0x5C,0x70
};
unsigned char bmp_BIRD4[16*2] = {
	0xC0,0x80,0xF8,0x18,0x04,0x37,0xFB,0xD9,0xF3,0xFB,0xFC,0x78,0xF8,0xC0,0xC0,0x00,
	0x01,0x01,0x1E,0x38,0x20,0x40,0x40,0x41,0x46,0x66,0x26,0x3B,0x1F,0x01,0x00,0x00
};
unsigned char bmp_BIRD5[16*2] = {
	0xC0,0x80,0xF8,0x18,0x04,0x37,0xFB,0xD9,0xF3,0xFB,0xFC,0x78,0xF8,0xC0,0xC0,0x00,
	0x01,0x01,0x1E,0x38,0x20,0x40,0x40,0x41,0x46,0x66,0x26,0x3B,0x1F,0x01,0x00,0x00
};
unsigned char bmp_BIRD[16*2];

unsigned char GameTitle[128*8] ={
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF8,0xE0,0xE0,0xF0,0xF0,0xB8,0xB8,0x98,0x98,0x08,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0xF0,0xF8,0xFC,0xFC,0xFC,0xF8,0xF8,0xF0,0xF0,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0xC0,0xC0,0xE0,0xE0,0xE0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xFF,0xFF,0xFF,0xF3,0xF0,0xF0,0xE0,0xE0,0xE0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x07,0x07,0x07,0x07,0x0F,0x1F,0x37,0x45,0x85,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC1,0xE3,0xF3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFC,0xF8,0xF0,0xF0,0xE0,0xF8,0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xCF,0x87,0x03,0x03,0x37,0x37,0x87,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x7F,0x7F,0xFF,0xFF,0xDF,0x97,0x93,0x87,0x8F,0xFD,0xF9,0xE1,0xC1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x30,0x08,0x06,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0x81,0x63,0xFC,0xF8,0xE0,0xE0,0xF8,0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x8F,0x07,0x06,0x06,0x02,0x02,0x02,0x02,0x06,0x06,0x06,0x86,0xE4,0xFD,0xFD,0xFB,0xFF,0xFF,0xFE,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x86,0x66,0x1E,0x7E,0x8F,0x8C,0x4C,0x6C,0x28,0x38,0x20,0x20,0x58,0xDC,0x8C,0x8C,0x86,0x86,0xB7,0x4F,0x07,0x03,0x03,0x03,0x1B,0x1B,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE1,0xD9,0x9D,0x83,0x83,0x43,0x23,0x23,0x3F,0x3B,0x4B,0x43,0x83,0x83,0xC3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x3E,0x31,0x22,0xE3,0xC3,0xC2,0xC2,0xC7,0xC5,0xC9,0xC9,0x89,0x89,0x8D,0x85,0x83,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xC0,0xC0,0xC0,0xC0,0x6F,0x7F,0x7F,0xFF,0xFF,0xFF,0x7F,0x3F,0x1F,0x1F,0x0F,0x0F,0x07,0x07,0x03,0x03,0x03,0x03,0x03,0x03,0x07,0x0F,0x18,0x28,0x48,0x88,0x08,0x88,0x88,0x48,0x68,0xF0,0xF1,0xF3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x3F,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xF8,0x08,0x18,0x30,0xE0,0x00,0x00,0x80,0xF0,0x08,0x18,0xF0,0x08,0x10,0x38,0xE0,0x00,0x00,0xE0,0x38,0x08,0x08,0xF0,0x10,0x1C,0x04,0x0C,0x30,0x60,0xF8,0x0C,0x06,0x02,0x06,0xFC,0x60,0x30,0x18,0x0C,0x04,0x84,0xA4,0xFE,0x9C,0x08,0x0C,0x04,0x04,0x83,0xC3,0xC2,0x02,0x02,0x02,0x06,0x0C,0x38,0x78,0xD8,0xD8,0x70,0x38,0x18,0x38,0x18,0xF8,0xF8,0xF8,0x98,0x09,0x0D,0x04,0x06,0x02,0x03,0x03,0x81,0x81,0x81,0x01,0x03,0x03,0x13,0x07,0xFF,0x07,0x03,0x03,0x07,0x0F,0x07,0x07,0x07,0x03,0xC3,0xC3,0x03,0x03,0x07,0x0F,0x1F,0x3F,0x3F,0x17,0x17,0x13,0x11,0x10,0x10,0x10,0x30,0x60,0x20,0x10,0x08,0x0C,0x04,0x06,0x06,0x86,0xFE,0x24,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0x00,0x00,0x00,0xC3,0x46,0xC4,0xC7,0x00,0x00,0x00,0xFF,0x00,0x00,0x8E,0xFF,0x00,0x00,0xC1,0xFF,0x00,0x00,0x3F,0xC0,0x00,0x00,0x00,0xF8,0x60,0x81,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x7C,0x7E,0x43,0x63,0x03,0x01,0x01,0xF9,0xE0,0x00,0x00,0x43,0xC1,0x80,0x04,0x08,0x1C,0x36,0x6C,0xF0,0xE0,0x21,0x03,0x00,0x00,0xC0,0x78,0x0E,0x01,0x00,0x01,0x03,0x06,0x06,0xFE,0x00,0x00,0x00,0x00,0x87,0x86,0x83,0x80,0x00,0x00,0x0C,0x1E,0xFF,0x00,0x00,0x00,0xFF,0xFE,0xC0,0x00,0x00,0x82,0x83,0x01,0x00,0x18,0x3C,0x26,0x64,0xFC,0x80,0x00,0x00,0xFE,0xE2,0x36,0x1C,0x00,0x00,0xC0,0x60,0xB0,0xA0,0x60,0x00,0x06,0x0D,0x19,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x7F,0x20,0x38,0x08,0x0F,0x00,0x0F,0x38,0x20,0x20,0x38,0x0F,0x0C,0x30,0x41,0x43,0x42,0x43,0x41,0x20,0x10,0x08,0x08,0x07,0x04,0x06,0x06,0x03,0x00,0x01,0x03,0x03,0x06,0x02,0x03,0x04,0x04,0x0C,0x0C,0x04,0x04,0x0C,0x1C,0x30,0x3F,0x03,0x06,0x06,0x03,0x01,0x00,0x01,0x03,0x02,0x04,0x04,0x3F,0x23,0x20,0x30,0x30,0x0E,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x37,0x66,0x40,0x40,0x40,0x63,0x23,0x21,0x10,0x10,0x18,0x0C,0x06,0x03,0x02,0x06,0x04,0x03,0x00,0x07,0x06,0x04,0x06,0x01,0x01,0x03,0x02,0x02,0x06,0x0F,0x1B,0x13,0x10,0x10,0x08,0x08,0x04,0x06,0x02,0x01,0x03,0x07,0x0D,0x08,0x08,0x08,0x0C,0x04,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

int main(void)
{
	char Text[25];
	int8_t GameState;
	int8_t x,y;
	int8_t i,keyin;
	int8_t hit;
	int8_t score, life;
	
	float  move_x, move_y;
	int8_t move_dir;	
	int16_t pig_x, bird_x;
	int16_t pig_y, bird_y;
	int8_t  bird_no;
  int8_t  bird_angle;
	
	SYS_Init();
	  
	init_LCD();
	clear_LCD();
  OpenKeyPad();// initialize 3x3 keypad
	draw_LCD(GameTitle);   // draw Game Title scene
	for (i=0;i<3;i++) CLK_SysTickDelay(1000000);
	clear_LCD();
	
	GameState=IDLE_STATE;
  bird_no=0;
	
	while(1) {		
	
  switch(GameState) {
		case IDLE_STATE:
	    sprintf(Text,"SCORE %3d   IDLE  BIRD %2d",score,life);	
	    y=0; x=0;	printS_5x7(x,y,Text);
      CLK_SysTickDelay(500000);
		
			if (bird_no==0) {
	      y=56; x=0;	printS_5x7(x,y,"Key 1~5 to select a bird");	  
        keyin=0;
        while(keyin==0 || keyin>5) {keyin=ScanKey(); }
        bird_no=keyin;
        switch(bird_no) {
				  case 1: for (i=0;i<16*2;i++) bmp_BIRD[i]=bmp_BIRD1[i];
				          break;
				  case 2: for (i=0;i<16*2;i++) bmp_BIRD[i]=bmp_BIRD2[i];
				          break;
				  case 3: for (i=0;i<16*2;i++) bmp_BIRD[i]=bmp_BIRD3[i];
				          break;
				  case 4: for (i=0;i<16*2;i++) bmp_BIRD[i]=bmp_BIRD4[i];
				          break;
				  case 5: for (i=0;i<16*2;i++) bmp_BIRD[i]=bmp_BIRD5[i];
				          break;
				  default:break;
			  }			
			} else {	
		    // Draw Bird
			  bird_x=0; bird_y=LCD_Ymax-BIRD_Ysize;
		    draw_Bmp16x16(bird_x, bird_y, FG_COLOR,BG_COLOR, bmp_BIRD);
				CLK_SysTickDelay(1000000);
			
			  GameState=SHOW_STATE;
				clear_LCD();
			  score =0; life =3;
	      pig_x =SCENE_Xmax - PIG_Xsize; pig_y=LCD_Ymax-PIG_Ysize;			
				bird_angle=45;
				move_dir=-1;
			}
		  break;		
			
		case SHOW_STATE:
	    sprintf(Text,"SCORE %3d   SHOW  BIRD %2d",score,life);		
      y=0; x=0;	printS_5x7(x,y,Text);
		
		  while (bird_y<LCD_Ymax) {
		     // Display Ojbects : draw foreground color to display object				
		     draw_Bmp16x16(bird_x%LCD_Xmax, bird_y,FG_COLOR,BG_COLOR, bmp_BIRD);
         if (bird_x>=SCENE_Xmax-LCD_Xmax)
	       draw_Bmp32x32(pig_x%LCD_Xmax,  pig_y, FG_COLOR,BG_COLOR, bmp_PIG);
	
		     CLK_SysTickDelay(150000);	 // Delay for Vision
		
		     // Erase Ojbects : draw background color at the ojbect location
		     draw_Bmp16x16(bird_x%LCD_Xmax, bird_y, BG_COLOR,BG_COLOR, bmp_BIRD);
         if (bird_x>=SCENE_Xmax-LCD_Xmax)
	       draw_Bmp32x32(pig_x%LCD_Xmax,  pig_y, BG_COLOR,BG_COLOR, bmp_PIG);
				 
	       // Move Bird				
			  move_x = cos(bird_angle * PI/180) *10;
			  move_y = sin(bird_angle * PI/180) *10;
				bird_x = bird_x + move_x;
				bird_y = bird_y + move_y * move_dir;
				if (bird_y<8) move_dir = 1;
			}
	
			
			GameState=ANGLE_STATE;
			break;
			
		case ANGLE_STATE:
	    sprintf(Text,"SCORE %3d  ANGLE  BIRD %2d",score,life);
      y=0; x=0;	printS_5x7(x,y,Text);
		  y=8 ;x=0; printS_5x7(x,y,"Key 6/9: angle up/down");
	    y=16;x=0; printS_5x7(x,y,"Key   8: fire the bird");		
		  bird_x=0; bird_y=LCD_Ymax-BIRD_Ysize;
	    draw_Bmp16x16(bird_x, bird_y, FG_COLOR,BG_COLOR, bmp_BIRD);	 		
      CLK_SysTickDelay(500000);
			keyin=0;
		  while(keyin==0) {keyin=ScanKey();}
			switch(keyin) {
				case 6: bird_angle++;
				        break;
				case 9: bird_angle--;
				        break;
				case 8: GameState=SHOOT_STATE;
                move_dir=-1;
				        hit=0;
                bird_x=0; bird_y=LCD_Ymax-BIRD_Ysize;				
				        clear_LCD();
				        break;
				default:break;
			}
			if (bird_angle<=0) bird_angle=0;
			if (bird_angle>90) bird_angle=90;
	    sprintf(Text,"a=%3d",bird_angle);
      y=56; x=56, printS_5x7(x,y,Text);
			break;

		case SHOOT_STATE:
	    sprintf(Text,"SCORE %3d  SHOOT  BIRD %2d",score,life);	
      y=0; x=0;	printS_5x7(x,y,Text);
		
		  while (bird_y<LCD_Ymax) {
		    draw_Bmp16x16(bird_x%LCD_Xmax, bird_y, FG_COLOR,BG_COLOR, bmp_BIRD);		
        if (bird_x>=(pig_x/LCD_Xmax)*LCD_Xmax && bird_x+BIRD_Xsize<(pig_x/LCD_Xmax+1)*LCD_Xmax)
	         draw_Bmp32x32(pig_x%LCD_Xmax,   pig_y, FG_COLOR,BG_COLOR, bmp_PIG);
			
		    // Delay for Vision
		    CLK_SysTickDelay(150000);
		
		    // Erase Ojbects : draw background color at the ojbect location
		    draw_Bmp16x16(bird_x%LCD_Xmax, bird_y, BG_COLOR,BG_COLOR, bmp_BIRD);
        if (bird_x>=(pig_x/LCD_Xmax)*LCD_Xmax && bird_x+BIRD_Xsize<(pig_x/LCD_Xmax+1)*LCD_Xmax)
	         draw_Bmp32x32(pig_x%LCD_Xmax,   pig_y, BG_COLOR,BG_COLOR, bmp_PIG);
			
	      //inc/dec bird x for moving horizontally
			  move_x = cos(bird_angle * PI/180) *10;
			  move_y = sin(bird_angle * PI/180) *10;
				bird_x = bird_x + move_x;
				bird_y = bird_y + move_y * move_dir;
				if (bird_y<8) move_dir = 1;
						
	      // Collision Detection : check if pig hit by bird
			  if ((bird_x+BIRD_Xsize-1)>=pig_x && bird_x<(pig_x+PIG_Xsize)) { // check if bird reach pig in x
		      if ((bird_y+BIRD_Ysize-1)>=pig_y && bird_y<(pig_y+PIG_Ysize)) // check if hit					  
					   hit=1;
					else
					   hit=0;
					break;
			  }
			}
			GameState=HIT_STATE;
			if (hit==0) life--;
			else        score++;
			break;
			
		case HIT_STATE:
			if (hit) sprintf(Text,"SCORE %3d   HIT   BIRD %2d",score,life);
			else     sprintf(Text,"SCORE %3d   MISS  BIRD %2d",score,life);				
      y=0; x=0;	printS_5x7(x,y,Text);
		  CLK_SysTickDelay(1000000);
		
			if (life<=0) {
        y=0; x=0;	printS_5x7(x,y,"  ===== GAME OVER =====  ");
				GameState=IDLE_STATE; 
				bird_no=0; }
			else {		
			  y=56;x=0;	printS_5x7(x,y,"Press Any Key to Retry ! ");
			  keyin=0;
		    while(keyin==0) {keyin=ScanKey();}				
				GameState=ANGLE_STATE;	
			}
			CLK_SysTickDelay(1000000);
			clear_LCD();
			break;
		default: break;
	}
	
 }
}
