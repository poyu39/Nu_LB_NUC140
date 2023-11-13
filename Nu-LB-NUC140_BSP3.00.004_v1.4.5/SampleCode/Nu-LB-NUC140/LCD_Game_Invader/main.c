//
// LCD_Game_Invader : emulate Space Invader game on LCD
//
// 5 rows of aliens moving horizontally
// 1 cannon move on bottom & shoot vertically
// press keypad to move the cannon

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"

// bitmap is in upside down order
unsigned char bmp_Bullet[8] = {
	0x00,0x00,0x18,0x18,0x18,0x18,0x00,0x00};
unsigned char bmp_Cannon[8] = {
	0x00,0x40,0xE0,0xF8,0xF8,0xE0,0x40,0x00};
unsigned char bmp_Alien0[16] = {
	0x00,0x10,0x38,0x7C,0x36,0x1E,0x17,0x3F,0x3F,0x17,0x1E,0x36,0x7C,0x38,0x10,0x00};
unsigned char bmp_Alien1[16] = {
	0x00,0x70,0x18,0x7D,0x36,0x34,0x3C,0x3C,0x3C,0x3C,0x34,0x36,0x7D,0x18,0x70,0x00};
unsigned char bmp_Alien2[16] = {
	0x00,0x5C,0x30,0x1E,0x3F,0x1A,0x1A,0x1E,0x1E,0x1A,0x1A,0x3F,0x1E,0x30,0x5C,0x00};
unsigned char bmp_Alien3[16] = {
	0x00,0x48,0x4C,0x2E,0x3E,0x1A,0x1B,0x2F,0x2F,0x1B,0x1A,0x3E,0x2E,0x4C,0x48,0x00};
unsigned char bmp_Alien4[16] = {
	0x00,0x00,0x00,0x08,0x4C,0x2E,0x5B,0x2F,0x2F,0x5B,0x2E,0x4C,0x08,0x00,0x00,0x00};

unsigned char GameTitle[128*8] ={
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x8F,0x8F,0x8F,0x8F,0x7F,0x7F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x7F,0x7F,0x7F,0x8F,0x8F,0x8F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x1F,0x1F,0x1F,0x0F,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x1F,0x1F,0x1F,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0x3F,0x3F,0x3F,0x03,0x03,0x03,0x03,0xC0,0xC0,0xC0,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0xC0,0xC0,0xC0,0xC0,0x03,0x03,0x03,0x3F,0x3F,0x3F,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x0F,0x0F,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x81,0x81,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x81,0x81,0x81,0x01,0x01,0x01,0x01,0x01,0x01,0x0F,0x0F,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x0F,0x0F,0x0F,0x01,0x00,0x00,0x00,0xF0,0xF0,0xF0,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0xF0,0xF0,0x01,0x00,0x00,0x01,0x0F,0x0F,0x0F,0xFF,
	0x01,0x01,0x01,0x01,0xF0,0xF0,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0xF0,0xF0,0xF0,0xA0,0x01,0x01,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xC0,0x03,0x03,0x03,0x03,0x03,0x01,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x03,0x03,0x03,0x03,0x03,0x03,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,0xF0,0xF0,0xF0,0x70,0x70,0x78,0x70,0x80,0x80,0x80,0x80,0x78,0x70,0x70,0x70,0x70,0x70,0x78,0x80,0x80,0x80,0x80,0x78,0x70,0x78,0x70,0xF0,0xF0,0xF0,0xFF,
	0xF8,0xF8,0xF8,0xF8,0xFF,0xFF,0xFF,0xF8,0xF8,0xF8,0xF8,0x87,0xC7,0xC7,0xC7,0xC7,0x87,0xFF,0xFF,0xFF,0xFF,0x87,0x87,0xC7,0xC7,0x87,0x87,0xC7,0xF8,0xF8,0xF8,0xFF,0xFF,0xFF,0xFF,0xF8,0xF8,0xF8,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xCF,0xC7,0xC7,0xC7,0xC7,0xC7,0xC1,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xFF,0xFF,0xFF,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xFF,0xFF,0xFF,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xC7,0xC7,0xC7,0xC7,0xC7,0xC7,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC7,0xC7,0xC7,0xC7,0xF8,0xF8,0xF8,0xF8,0xC7,0xC7,0xC7,0xC7,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xC7,0xC7,0xC7,0xC7,0xF8,0xF8,0xF8,0xC2,0xC7,0xC7,0xC7,0xFF,
	0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x7C,0xE6,0xC6,0xC6,0xC6,0x8C,0x00,0x00,0xF0,0xF0,0x60,0x30,0x30,0x70,0xF0,0xE0,0x00,0x00,0x20,0xB0,0xB0,0xB0,0xB0,0xF0,0xE0,0x00,0x00,0xC0,0xE0,0x70,0x30,0x30,0x70,0x00,0x00,0xC0,0xE0,0xB0,0xB0,0xB0,0xB0,0xE0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0x00,0x00,0xF0,0xF0,0x60,0x30,0x30,0x70,0xF0,0xE0,0x00,0x10,0xF0,0xF0,0x80,0x00,0x00,0xE0,0xF0,0x10,0x00,0x20,0xB0,0xB0,0xB0,0xB0,0xF0,0xE0,0x00,0x00,0xE0,0xF0,0x70,0x30,0x30,0x60,0xFF,0xFF,0x00,0x00,0xC0,0xE0,0xB0,0xB0,0xB0,0xB0,0xE0,0xC0,0x00,0x00,0xF0,0xF0,0xE0,0x30,0x30,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x18,0x18,0x18,0x19,0x0F,0x0F,0x00,0x00,0xFF,0xFF,0x0C,0x18,0x18,0x1C,0x1F,0x0F,0x00,0x00,0x0F,0x1F,0x19,0x19,0x1D,0x1F,0x1F,0x00,0x00,0x07,0x0F,0x1C,0x18,0x18,0x18,0x00,0x00,0x07,0x0F,0x1D,0x19,0x19,0x19,0x19,0x0D,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x00,0x00,0x1F,0x1F,0x00,0x00,0x00,0x00,0x1F,0x1F,0x00,0x00,0x00,0x07,0x1F,0x1C,0x1F,0x07,0x00,0x00,0x00,0x0F,0x1F,0x19,0x19,0x1D,0x1F,0x1F,0x00,0x00,0x0F,0x1F,0x18,0x18,0x18,0x0C,0x1F,0x1F,0x00,0x00,0x07,0x0F,0x1D,0x19,0x19,0x19,0x19,0x0D,0x00,0x00,0x1F,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

void Buzz(int8_t no)
{
	while(no!=0) {
		PB11=0;
		CLK_SysTickDelay(50000);
		PB11=1;
		CLK_SysTickDelay(50000);
	  no--;
	}
}

void Init_Buzz(void)
{
	GPIO_SetMode(PB, BIT11, GPIO_PMD_OUTPUT);
	PB11=1;
}

int main(void)
{
	char Text[25];
	int8_t x,y;
	int8_t i,j,keyin;
	int8_t hit;
	int16_t score;
	int8_t alien_no, bullet_no;

	int8_t alien0_x   , alien0_y, alien0_speed;
	int8_t alien1_x[5], alien1_y, alien1_speed;
	int8_t alien2_x[5], alien2_y, alien2_speed;
	int8_t alien3_x[5], alien3_y, alien3_speed;
	int8_t alien4_x[5], alien4_y, alien4_speed;	
	int8_t cannon_x;
	int8_t cannon_y;
	
	int8_t bullet_x[8];
  int8_t bullet_y[8];
	int8_t bullet_speed;	
	
	int8_t alien_map[5*5];
	int16_t alien_score[5] = {300, 30, 25, 20, 10};
	
	SYS_Init();
	  
	init_LCD();
	clear_LCD();
  OpenKeyPad();
	Init_Buzz();
	
	draw_LCD(GameTitle);   // draw Game Title scene
	for (i=0;i<3;i++) CLK_SysTickDelay(1000000);
	clear_LCD();

	printS_5x7(0, 0,"LCD Game : Space Invader");
	draw_Bmp16x8(26,16,FG_COLOR,BG_COLOR, bmp_Alien1);	
	printS_5x7(46,16,"=  30 POINTS");
	draw_Bmp16x8(26,24,FG_COLOR,BG_COLOR, bmp_Alien2);	
	printS_5x7(46,24,"=  25 POINTS");
	draw_Bmp16x8(26,32,FG_COLOR,BG_COLOR, bmp_Alien3);	
	printS_5x7(46,32,"=  20 POINTS");	
	draw_Bmp16x8(26,40,FG_COLOR,BG_COLOR, bmp_Alien4);	
	printS_5x7(46,40,"=  10 POINTS");
	draw_Bmp16x8(26,48,FG_COLOR,BG_COLOR, bmp_Alien0);	
	printS_5x7(46,48,"= 300 POINTS");	
	printS_5x7(0,56,"Left:4, Shoot:2, Right:6");
	
	keyin=ScanKey();
  while(keyin==0) { keyin=ScanKey();}
	clear_LCD();
	
  // set initial location of bus & its speed
	alien0_speed=4; alien0_y= 8; alien0_x= 0;
	alien1_speed=1; alien1_y=16;
	alien1_x[0]= 0; alien1_x[1]= 16; alien1_x[2]= 48; alien1_x[3]=64; alien1_x[4]=96;
	alien2_speed=1; alien2_y=24;
	alien2_x[0]= 0; alien2_x[1]= 16; alien2_x[2]= 48; alien2_x[3]=64; alien2_x[4]=96; 
	alien3_speed=1; alien3_y=32;
	alien3_x[0]= 0; alien3_x[1]= 16; alien3_x[2]= 48; alien3_x[3]=64; alien3_x[4]=96; 
	alien4_speed=1; alien4_y=40;
	alien4_x[0]= 0; alien4_x[1]= 16; alien4_x[2]= 48; alien4_x[3]=64; alien4_x[4]=96;
	
	bullet_speed=1;
	hit=0; score=0; bullet_no=0;
	cannon_x=56; cannon_y=56;	
	for(j=0; j<8; j++) bullet_y[j]=0;
	alien_map[0]=1;
	for (j=1; j<5; j++)
	  for (i=0; i<5; i++)
        alien_map[j*5+i]=1;	

	while(1) {		
	// Display Score & Life
	sprintf(Text,"SCORE %3d       ALIEN %2d",score, alien_no);	
	y=0; x=0;	printS_5x7(x,y,Text);	
	  if (hit==0)
	  {
		  // Display Ojbects : draw foreground color to display object
			if (alien_map[0]==1)
		    draw_Bmp16x8(alien0_x   , alien0_y,FG_COLOR,BG_COLOR, bmp_Alien0);
		  for (i=0; i<5; i++) {
			  if (alien_map[1*5+i]==1) 
				draw_Bmp16x8(alien1_x[i], alien1_y,FG_COLOR,BG_COLOR, bmp_Alien1);
				if (alien_map[2*5+i]==1)
		    draw_Bmp16x8(alien2_x[i], alien2_y,FG_COLOR,BG_COLOR, bmp_Alien2);
				if (alien_map[3*5+i]==1)
		    draw_Bmp16x8(alien3_x[i], alien3_y,FG_COLOR,BG_COLOR, bmp_Alien3);
				if (alien_map[4*5+i]==1)
		    draw_Bmp16x8(alien4_x[i], alien4_y,FG_COLOR,BG_COLOR, bmp_Alien4);
			}
      for (i=0; i<8; i++) {
			  if (bullet_y[i]>8)
	         draw_Bmp8x8(bullet_x[i], bullet_y[i],FG_COLOR,BG_COLOR, bmp_Bullet); }
	    draw_Bmp8x8(cannon_x, cannon_y, FG_COLOR,BG_COLOR, bmp_Cannon); 
		
		  // Delay for Vision
		  CLK_SysTickDelay(1000);
		
		  // Erase Ojbects : draw background color at the ojbect location
		    draw_Bmp16x8(alien0_x   , alien0_y,BG_COLOR,BG_COLOR, bmp_Alien0);
		  for (i=0; i<5; i++) {
				draw_Bmp16x8(alien1_x[i], alien1_y,BG_COLOR,BG_COLOR, bmp_Alien1);
		    draw_Bmp16x8(alien2_x[i], alien2_y,BG_COLOR,BG_COLOR, bmp_Alien2);
		    draw_Bmp16x8(alien3_x[i], alien3_y,BG_COLOR,BG_COLOR, bmp_Alien3);
		    draw_Bmp16x8(alien4_x[i], alien4_y,BG_COLOR,BG_COLOR, bmp_Alien4);
      }
      for (i=0; i<8; i++) {
			  if (bullet_y[i]>8)
	        draw_Bmp8x8(bullet_x[i], bullet_y[i],BG_COLOR,BG_COLOR, bmp_Bullet); }
				 
	    draw_Bmp8x8(cannon_x, cannon_y, BG_COLOR,BG_COLOR, bmp_Cannon);		
		
	    // Object Move by changing its x,y		
		  keyin=ScanKey(); // get keypad
		  switch(keyin) {  
			case 4: cannon_x--;  
				      break;
			case 6: cannon_x++;  
				      break;
			case 2: bullet_no++;
				      if (bullet_no>=8) bullet_no=0;
					    bullet_x[bullet_no] =cannon_x;
				      bullet_y[bullet_no] =cannon_y-8;
				      break;
			default:break;
		  }	
			
			for (i=0; i<8; i++)
			  if (bullet_y[i]>8) 
			  bullet_y[i] -= bullet_speed;
				
	    //inc/dec Alien x for moving horizontally
		  alien0_x    += alien0_speed;			
			for (i=0; i<5; i++) {
		  alien1_x[i] += alien1_speed;
		  alien2_x[i] += alien2_speed;
		  alien3_x[i] += alien3_speed;
		  alien4_x[i] += alien4_speed;
			}
	 			
	    // Boundary Check : if >max, set to min; if <min, set to max;
			if (alien_map[0]==1) {
		    if (alien0_x<0)                  alien0_x=LCD_Xmax-16;
		    if (alien0_x>(LCD_Xmax-16))      alien0_x=0;			
			}
			for (i=0; i<5; i++) {
				if (alien_map[1*5+i]==1) {
		      if (alien1_x[i]<0)             alien1_x[i]=LCD_Xmax-16;
		      if (alien1_x[i]>(LCD_Xmax-16)) alien1_x[i]=0; }
				if (alien_map[2*5+i]==1) {
		      if (alien2_x[i]<0)             alien2_x[i]=LCD_Xmax-16;
		      if (alien2_x[i]>(LCD_Xmax-16)) alien2_x[i]=0;	}
				if (alien_map[3*5+i]==1) {
		      if (alien3_x[i]<0)             alien3_x[i]=LCD_Xmax-16;
		      if (alien3_x[i]>(LCD_Xmax-16)) alien3_x[i]=0; }
				if (alien_map[4*5+i]==1) {
		      if (alien4_x[i]<0)             alien4_x[i]=LCD_Xmax-16;
		      if (alien4_x[i]>(LCD_Xmax-16)) alien4_x[i]=0;	}
			}
		
		  if (cannon_x>(LCD_Xmax-8))	cannon_x=0;
      if (cannon_x<0)             cannon_x=LCD_Xmax-8;
				
	    // Collision Detection : check if Frog hit by Bus		
			for (j=0; j<8; j++) {// check bullets (max=8)
				if ((alien_map[0]!=0) &&
		        (bullet_y[j]+4 >=alien0_y && bullet_y[j]+3 <alien0_y+8))   // if bullet overlap alien
				   if (bullet_x[j]+4 >=alien0_x && bullet_x[j]+3 <alien0_x+16) // check bullet_x				
				      {hit=1; bullet_y[j]=0; score+=alien_score[0]; alien_map[0]=0;} 
			  for (i=0; i<5; i++) {
				if ((alien_map[1*5+i]!=0) &&
		        (bullet_y[j]+4>=alien1_y && bullet_y[j]+3<alien1_y+8))
			     if (bullet_x[j]+4>=alien1_x[i] && bullet_x[j]+3<alien1_x[i]+16) 
					    {hit=1; bullet_y[j]=0; score+=alien_score[1]; alien_map[1*5+i]=0;}
				if ((alien_map[2*5+i]!=0) &&							
		        (bullet_y[j]+4>=alien2_y && bullet_y[j]+3<alien2_y+8))					
			     if (bullet_x[j]+4>=alien2_x[i] && bullet_x[j]+3<alien2_x[i]+16) 
						  {hit=1; bullet_y[j]=0; score+=alien_score[2]; alien_map[2*5+i]=0;}
			  if ((alien_map[3*5+i]!=0) &&								
		        (bullet_y[j]+4>=alien3_y && bullet_y[j]+3<alien3_y+8))
			     if (bullet_x[j]+4>=alien3_x[i] && bullet_x[j]+3<alien3_x[i]+16)
						  {hit=1; bullet_y[j]=0; score+=alien_score[3]; alien_map[3*5+i]=0;}
			  if ((alien_map[4*5+i]!=0) &&							
		        (bullet_y[j]+4>=alien4_y && bullet_y[j]+3<alien4_y+8))
			     if (bullet_x[j]+4>=alien4_x[i] && bullet_x[j]+3<alien4_x[i]+16)
						  {hit=1; bullet_y[j]=0; score+=alien_score[4]; alien_map[4*5+i]=0;}
	      }			
			}
			alien_no=0;
			for (j=0; j<5; j++)
		    for (i=0; i<5; i++)
           alien_no = alien_no + alien_map[j*5+i];	
	  }	else { 
			Buzz(1); // buzz for hit
			hit=0; 			
			if (alien_no==0) {
				x=0; y=32; printS_5x7(x,y,"  =====  YOU WON  =====  ");
			  printS_5x7( 0,56,"Press Any Key To Start!");
	      keyin=ScanKey();
        while(keyin==0) { keyin=ScanKey();}
	      clear_LCD();				
	      alien0_x= 0;  
	      alien1_x[0]= 0; alien1_x[1]= 16; alien1_x[2]= 48; alien1_x[3]=64; alien1_x[4]=96;
	      alien2_x[0]= 0; alien2_x[1]= 16; alien2_x[2]= 48; alien2_x[3]=64; alien2_x[4]=96; 
	      alien3_x[0]= 0; alien3_x[1]= 16; alien3_x[2]= 48; alien3_x[3]=64; alien3_x[4]=96; 
	      alien4_x[0]= 0; alien4_x[1]= 16; alien4_x[2]= 48; alien4_x[3]=64; alien4_x[4]=96;				
	      hit=0; score=0; bullet_no=0;
	      cannon_x=56; cannon_y=56;	
	      for(j=0; j<8; j++) bullet_y[j]=0;
	      alien_map[0]=1;
	      for (j=1; j<5; j++)
	        for (i=0; i<5; i++)
               alien_map[j*5+i]=1;	
		  }					
	  }
  }
}