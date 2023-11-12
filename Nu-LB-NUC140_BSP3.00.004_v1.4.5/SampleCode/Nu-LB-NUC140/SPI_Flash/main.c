//
// SPI_Flash : SPI interfacing with on-board flash memory
//
// MCU : NUC140VE3CN
// EVB : Nu-LB-NUC140
// SPI : W25X16 2MB (on-board flash memory)
//
// W25X16 connections 
// SS    : to GPD0/SPI2_SS20
// SCLK  : to GPD1/SPI2_SPCLK
// MISO0 : to GPD2/SPI2_MISO0
// MOSI0 : to GPD3/SPI2_MOSI0
// MISO1 : to GPD4/SPI2_MISO1
// MOSI1 : to GPD5/SPI2_MOSI1

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "W25Q16CV.h"

char Text[16];


int main( void )
{
  uint8_t id;
  uint16_t mfid;
  uint8_t data[4];
  SYS_Init();

  init_LCD();  // initialize LCD
  clear_LCD();
  print_Line(0,"SPI-Flash W25Q16");
	
  init_W25Q16();  // initialize SPI2

  // Read Device ID
  id = W25Q16_ReleasePowerDown_DeviceID();
  sprintf(Text,"ID= %2x", id);
  print_Line(1,Text);
  
  // Read Manufacturer ID & Device ID
  mfid = W25Q16_ReadManufacturerDeviceID();	
  sprintf(Text,"MF ID = %4x", mfid);
  print_Line(2,Text);
	
  // SPI Flash Write
  W25Q16_WriteEnable();
  data[0]=0x55; data[1]=0xAA; data[2]=0x12; data[3]=0x34;
  W25Q16_PageProgram(0x01000, data);	
  W25Q16_WriteDisable();
  
  // SPI Flash Read
  data[0] = W25Q16_ReadData(0x01000);	
  data[1] = W25Q16_ReadData(0x01001);	
  data[2] = W25Q16_ReadData(0x01002);	
  data[3] = W25Q16_ReadData(0x01003);	
	
  sprintf(Text,"Data=%2x%2x%2x%2x", data[0], data[1], data[2], data[3]);
  print_Line(3,Text);
}
