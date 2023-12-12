#include "stdio.h"
#include "string.h"
#include "NUC100Series.h"
#include "SYS.h"
#include "SPI.h"
#include "GPIO.h"	
#include "LCD.h"
#include "Font5x7.h"
#include "Font8x16.h"

char DisplayBuffer[128*8];

void init_SPI3(void)
{
    /* Configure as a master, clock idle low, 9-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 1MHz */
    SPI_Open(SPI3, SPI_MASTER, SPI_MODE_0, 9, 1000000);
    SPI_DisableAutoSS(SPI3);	
}

void lcdWriteCommand(unsigned char temp)
{
  SPI_SET_SS0_LOW(SPI3);
  SPI_WRITE_TX0(SPI3, temp); // Write Data
  SPI_TRIGGER(SPI3);         // Trigger SPI data transfer           
  while(SPI_IS_BUSY(SPI3));  // Check SPI3 busy status
  SPI_SET_SS0_HIGH(SPI3);	
}

// Wrtie data to LCD 
void lcdWriteData(uint8_t temp)
{
  SPI_SET_SS0_LOW(SPI3);
  SPI_WRITE_TX0(SPI3, 0x100 + temp); // Write Data
  SPI_TRIGGER(SPI3);         // Trigger SPI data transfer           
  while(SPI_IS_BUSY(SPI3));  // Check SPI3 busy status
  SPI_SET_SS0_HIGH(SPI3);
}

// Set Address to LCD
void lcdSetAddr(uint8_t PageAddr, uint8_t ColumnAddr)
{
	// Set PA
  SPI_SET_SS0_LOW(SPI3);
  SPI_WRITE_TX0(SPI3, 0xB0 | PageAddr); // Write Data
  SPI_TRIGGER(SPI3);         // Trigger SPI data transfer           
  while(SPI_IS_BUSY(SPI3));  // Check SPI3 busy status
  SPI_SET_SS0_HIGH(SPI3);
  // Set CA MSB	
  SPI_SET_SS0_LOW(SPI3);
  SPI_WRITE_TX0(SPI3, 0x10 | (ColumnAddr>>4)&0xF); // Write Data
  SPI_TRIGGER(SPI3);         // Trigger SPI data transfer           
  while(SPI_IS_BUSY(SPI3));  // Check SPI3 busy status
  SPI_SET_SS0_HIGH(SPI3);	
  // Set CA LSB
  SPI_SET_SS0_LOW(SPI3);
  SPI_WRITE_TX0(SPI3, 0x00 | (ColumnAddr & 0xF)); // Write Data
  SPI_TRIGGER(SPI3);         // Trigger SPI data transfer           
  while(SPI_IS_BUSY(SPI3));  // Check SPI3 busy status
  SPI_SET_SS0_HIGH(SPI3);
}

void init_LCD(void)
{
	init_SPI3();
	lcdWriteCommand(0xEB); 
	lcdWriteCommand(0x81); 
	lcdWriteCommand(0xA0);  
	lcdWriteCommand(0xC0);  
	lcdWriteCommand(0xAF); // Set Display Enable 
}

void clear_LCD(void)
{
	int16_t i,j;
	for (j=0;j<LCD_Ymax;j++)
	  for (i=0;i<LCD_Xmax;i++)
	     DisplayBuffer[i+j/8*LCD_Xmax]=0;
	
	lcdSetAddr(0x0, 0x0);			  								  
	for (i = 0; i < 132 *8; i++)
	{
		lcdWriteData(0x00);
	}
	lcdWriteData(0x0f);
}

void printC(int16_t x, int16_t y, unsigned char ascii_code)
{
  int8_t i;
  unsigned char temp;	    
  for(i=0;i<8;i++) {
	lcdSetAddr((y/8),(LCD_Xmax+1-x-i));
	temp=Font8x16[(ascii_code-0x20)*16+i];	 
	lcdWriteData(temp);
    }

  for(i=0;i<8;i++) {
	lcdSetAddr((y/8)+1,(LCD_Xmax+1-x-i));	 
	temp=Font8x16[(ascii_code-0x20)*16+i+8];
	lcdWriteData(temp);
    }
}

// print char function using Font5x7
void printC_5x7 (int16_t x, int16_t y, unsigned char ascii_code) 
{
	uint8_t i;
	if (x<(LCD_Xmax-5) && y<(LCD_Ymax-7)) {
	   if      (ascii_code<0x20) ascii_code=0x20;
     else if (ascii_code>0x7F) ascii_code=0x20;
	   else           ascii_code=ascii_code-0x20;
	   for (i=0;i<5;i++) {
			  lcdSetAddr((y/8),(LCD_Xmax+1-x-i)); 
        lcdWriteData(Font5x7[ascii_code*5+i]);
		 }
	}
}

void print_Line(int8_t line, char text[])
{
	int8_t i;
	for (i=0;i<strlen(text);i++) 
		printC(i*8,line*16,text[i]);
}

void printS(int16_t x, int16_t y, char text[])
{
	int8_t i;
	for (i=0;i<strlen(text);i++) 
		printC(x+i*8, y,text[i]);
}

void printS_5x7(int16_t x, int16_t y, char text[])
{
	uint8_t i;
	for (i=0;i<strlen(text);i++) {
		printC_5x7(x,y,text[i]);
	  x=x+5;
	}
}

void draw_Bmp8x8(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,k, kx,ky;
	if (x<(LCD_Xmax-7) && y<(LCD_Ymax-7)) // boundary check		
		 for (i=0;i<8;i++){
			   kx=x+i;
				 t=bitmap[i];					 
				 for (k=0;k<8;k++) {
					      ky=y+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
				}
		     //lcdSetAddr(y/8,(LCD_Xmax+1-x));
	       //lcdWriteData(bitmap[i]);
		 }
}

void draw_Bmp32x8(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,k, kx,ky;
	if (x<(LCD_Xmax-7) && y<(LCD_Ymax-7)) // boundary check		
		 for (i=0;i<32;i++){
			   kx=x+i;
				 t=bitmap[i];					 
				 for (k=0;k<8;k++) {
					      ky=y+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
				}
		     //lcdSetAddr(y/8,(LCD_Xmax+1-x));
	       //lcdWriteData(bitmap[i]);
		 }
}

void draw_Bmp120x8(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,k, kx,ky;
	if (x<(LCD_Xmax-7) && y<(LCD_Ymax-7)) // boundary check		
		 for (i=0;i<120;i++){
			   kx=x+i;
				 t=bitmap[i];					 
				 for (k=0;k<8;k++) {
					      ky=y+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
				}
		     //lcdSetAddr(y/8,(LCD_Xmax+1-x));
	       //lcdWriteData(bitmap[i]);
		 }
}

void draw_Bmp8x16(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,k, kx,ky;
	if (x<(LCD_Xmax-7) && y<(LCD_Ymax-7)) // boundary check		
		 for (i=0;i<8;i++){
			   kx=x+i;
				 t=bitmap[i];					 
				 for (k=0;k<8;k++) {
					      ky=y+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
				}
				 t=bitmap[i+8];					 
				 for (k=0;k<8;k++) {
					      ky=y+k+8;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
				}				 
		     //lcdSetAddr(y/8,(LCD_Xmax+1-x));
	       //lcdWriteData(bitmap[i]);
		 }
}

void draw_Bmp16x8(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,k,kx,ky;
	if (x<(LCD_Xmax-15) && y<(LCD_Ymax-7)) // boundary check
		 for (i=0;i<16;i++)
	   {
			   kx=x+i;
				 t=bitmap[i];					 
				 for (k=0;k<8;k++) {
					      ky=y+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
					}
		     //lcdSetAddr(y/8,(LCD_Xmax+1-x));
	       //lcdWriteData(bitmap[i]);
			   //x=x++;
		 }
}

void draw_Bmp16x16(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,j,k, kx,ky;
	if (x<(LCD_Xmax-15) && y<(LCD_Ymax-15)) // boundary check
	   for (j=0;j<2; j++){		 
		     for (i=0;i<16;i++) {	
            kx=x+i;
					  t=bitmap[i+j*16];					 
					  for (k=0;k<8;k++) {
					      ky=y+j*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}
		     //lcdSetAddr(y/8+j,(LCD_Xmax+1-x-i));
	       //lcdWriteData(bitmap[i+j*16]);
		     }
		 }
}

void draw_Bmp16x24(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,j,k, kx,ky;
	if (x<(LCD_Xmax-15) && y<(LCD_Ymax-15)) // boundary check
	   for (j=0;j<3; j++){		 
		     for (i=0;i<16;i++) {	
            kx=x+i;
					  t=bitmap[i+j*16];					 
					  for (k=0;k<8;k++) {
					      ky=y+j*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}
		     //lcdSetAddr(y/8+j,(LCD_Xmax+1-x-i));
	       //lcdWriteData(bitmap[i+j*16]);
		     }
		 }
}

void draw_Bmp16x32(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t, i,j,k, kx,ky;
	if (x<(LCD_Xmax-15) && y<(LCD_Ymax-31)) // boundary check
	   for (j=0;j<4; j++)	{			 
		     for (i=0;i<16;i++) {
            kx=x+i;
					  t=bitmap[i+j*16];					 
					  for (k=0;k<8;k++) {
					      ky=y+j*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}					 
		     //lcdSetAddr(y/8+j,(LCD_Xmax+1-x));
	       //lcdWriteData(bitmap[i+j*16]);
			   //x=x++;
		     }		 
		 }
}

void draw_Bmp16x40(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t, i,j,k, kx,ky;
	if (x<(LCD_Xmax-15) && y<(LCD_Ymax-31)) // boundary check
	   for (j=0;j<5; j++)	{			 
		     for (i=0;i<16;i++) {
            kx=x+i;
					  t=bitmap[i+j*16];					 
					  for (k=0;k<8;k++) {
					      ky=y+j*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}					 
		     //lcdSetAddr(y/8+j,(LCD_Xmax+1-x));
	       //lcdWriteData(bitmap[i+j*16]);
			   //x=x++;
		     }		 
		 }
}

void draw_Bmp16x48(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,j,k,kx,ky;
	if (x<(LCD_Xmax-15) && y<(LCD_Ymax-47)) // boundary check
	   for (j=0;j<6; j++)	{
         k=x;			 
		     for (i=0;i<16;i++) {
            kx=x+i;
					  t=bitmap[i+j*16];					 
					  for (k=0;k<8;k++) {
					      ky=y+j*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}						 
		     //lcdSetAddr(y/8+j,(LCD_Xmax+1-k));
	       //lcdWriteData(bitmap[i+j*16]);
			   //k=k++;
		     }		 
		 }
}

void draw_Bmp16x64(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,j,k,kx,ky;
	if (x<(LCD_Xmax-15) && y==0) // boundary check
	   for (j=0;j<8; j++) {
				 k=x;
		     for (i=0;i<16;i++) {
            kx=x+i;
					  t=bitmap[i+j*16];					 
					  for (k=0;k<8;k++) {
					      ky=y+j*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}						 
		     //lcdSetAddr(y/8+j,(LCD_Xmax+1-k));
	       //lcdWriteData(bitmap[i+j*16]);
			   //k=k++;
		     }
		 }
}

void draw_Bmp32x16(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,jx,jy,k,kx,ky;
	if (x<(LCD_Xmax-31) && y<(LCD_Ymax-15)) // boundary check
		for (jy=0;jy<2;jy++)
	   for (jx=0;jx<2;jx++)	{
			   k=x;
		     for (i=0;i<16;i++) {
            kx=x+jx*16+i;
					  t=bitmap[i+jx*16+jy*32];					 
					  for (k=0;k<8;k++) {
					      ky=y+jy*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}						 
		     //lcdSetAddr(y/8+jy,(LCD_Xmax+1-k)-jx*16);
	       //lcdWriteData(bitmap[i+jx*16+jy*32]);
			   //k=k++;
		     }
			}
}

void draw_Bmp32x32(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,jx,jy,k, kx,ky;
	if (x<(LCD_Xmax-31) && y<(LCD_Ymax-31)) // boundary check
		for (jy=0;jy<4;jy++)
	   for (jx=0;jx<2;jx++)	{
			   k=x;
		     for (i=0;i<16;i++) {
            kx=x+jx*16+i;
					  t=bitmap[i+jx*16+jy*32];					 
					  for (k=0;k<8;k++) {
					      ky=y+jy*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}						 
		     //lcdSetAddr(y/8+jy,(LCD_Xmax+1-k)-jx*16);
	       //lcdWriteData(bitmap[i+jx*16+jy*32]);
			   //k=k++;
		     }
			}
}

void draw_Bmp32x48(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,jx,jy,k, kx,ky;
	if (x<(LCD_Xmax-31) && y<(LCD_Ymax-47)) // boundary check
		for (jy=0;jy<6;jy++)
	   for (jx=0;jx<2;jx++)	{
			   k=x;
		     for (i=0;i<16;i++) {
					  kx=x+jx*16+i;
					  t=bitmap[i+jx*16+jy*32];					 
					  for (k=0;k<8;k++) {
					      ky=y+jy*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}	
		     //lcdSetAddr(y/8+jy,(LCD_Xmax+1-k)-jx*16);
	       //lcdWriteData(bitmap[i+jx*16+jy*32]);
			   //k=k++;
		     }		 
		 }
}

void draw_Bmp32x64(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t,i,jx,jy,k, kx,ky;
	if (x<(LCD_Xmax-31) && y==0) // boundary check
		for (jy=0;jy<8;jy++)
	   for (jx=0;jx<2;jx++)	{
			   k=x;
		     for (i=0;i<16;i++) {
					  kx=x+jx*16+i;
					  t=bitmap[i+jx*16+jy*32];					 
					  for (k=0;k<8;k++) {
					      ky=y+jy*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}						 
		     //lcdSetAddr(y/8+jy,(LCD_Xmax+1-k)-jx*16);
	       //lcdWriteData(bitmap[i+jx*16+jy*32]);
			   //k=k++;
		     }
			}				 
}

void draw_Bmp64x64(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[])
{
	uint8_t t, i,jx,jy,k, kx,ky;
	if (x<(LCD_Xmax-63) && y==0) // boundary check
		for (jy=0;jy<8;jy++)
	   for (jx=0;jx<4;jx++)	{
	       k=x;
		     for (i=0;i<16;i++) {
					  kx=x+jx*16+i;
					  t=bitmap[i+jx*16+jy*64];					 
					  for (k=0;k<8;k++) {
					      ky=y+jy*8+k;
					      if (t&(0x01<<k)) draw_Pixel(kx,ky,fgColor,bgColor);
						}					 
		     //lcdSetAddr(y/8+jy,(LCD_Xmax+1-k)-jx*16);
	       //lcdWriteData(bitmap[i+jx*16+jy*64]);
			   //k=k++;
		     }
			}
}

void draw_Pixel(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor)
{	
	if (fgColor!=0) 
		DisplayBuffer[x+y/8*LCD_Xmax] |= (0x01<<(y%8));
	else 
		DisplayBuffer[x+y/8*LCD_Xmax] &= (0xFE<<(y%8));
		
	lcdSetAddr(y/8,(LCD_Xmax+1-x));
	lcdWriteData(DisplayBuffer[x+y/8*LCD_Xmax]);
}

void draw_LCD(unsigned char *buffer)
{
  uint8_t x,y;
	for (x=0; x<LCD_Xmax; x++) {
    	for (y=0; y<(LCD_Ymax/8); y++) {
			   lcdSetAddr(y,(LCD_Xmax+1-x));				
			   lcdWriteData(buffer[x+y*LCD_Xmax]);
			}
		}			
}
