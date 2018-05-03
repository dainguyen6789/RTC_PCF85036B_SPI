#include "PCF85063BTL.h"

#include <intrins.h>

/***********************************************

************************************************/
void PCF85063BTL_Delay(unsigned int us)
{
  while( us--)
  {
    _nop_(); 
  }             
}
/***********************************************
************************************************/
void SPI_SendByte(unsigned char dat)// Send dat to SPI device/Slave
{
  unsigned char i;
  for (i=0; i<8; i++)            //
  {
    PCF85063BTL_SCK_Clr();          //		
		if( (dat&0x80)==0x80 )	  	 //
	 	{
	   		PCF85063BTL_SDI_Set();
	 	}
	 	else
	 	{
	   		PCF85063BTL_SDI_Clr();
	 	}
		PCF85063BTL_Delay(1);           //
    PCF85063BTL_SCK_Set();          //
		PCF85063BTL_Delay(1);           //
		dat <<= 1;                   //
  }
}
/***********************************************
************************************************/
unsigned char SPI_ReceiveByte(void)
{
	unsigned char i;
  unsigned char dat = 0;

	PCF85063BTL_SDO_Set();					//
  for (i=0; i<8; i++)         //
  {
		dat <<= 1;	              //
		PCF85063BTL_SCK_Clr();       //
		PCF85063BTL_Delay(1);        //
        
		if (PCF85063BTL_SDO) 				//If  PCF85063BTL_SDO is HIGH, dat will be 1(one) 
		{
			dat |= 0x01;            //
		}
		PCF85063BTL_SCK_Set();       //
		PCF85063BTL_Delay(1);        //	
	}
  return dat;
}
////////////////////////////////////////////////////////////
unsigned char SPI_ReadTime(unsigned char addr)
{
	//unsigned char i;
  unsigned char dat = 0;
	PCF85063BTL_CS_Set();
	SPI_SendByte(Read_Mode|addr);//send Read_mode and Seconds register's address
	dat=SPI_ReceiveByte();
	PCF85063BTL_CS_Clr();
  return dat;
}
void SPI_WriteTime(unsigned char val,unsigned char addr)
{
	//PCF85063BTL_CS_Clr();
	PCF85063BTL_CS_Set();
	SPI_SendByte(Write_Mode|addr);//send Read_mode and Seconds register's address
	SPI_SendByte(val);
	SPI_SendByte(val);
	PCF85063BTL_CS_Clr();
}
/***********************************************
************************************************/

void SPI_Init(void)
{
	//P20,P21,P22 P23 quasi-bidirectional
	//P2M1 =0x00;  
	//P2M0 =0x00;
	
	P2M1 &=~( (1<<0) | (1<<1) | (1<<2) );  //~ bitwise NOT
	P2M0 &=~( (1<<0) | (1<<1) | (1<<2) );  
	PCF85063BTL_CS_Set();
	PCF85063BTL_CS_Clr();
	PCF85063BTL_SCK_Clr();
}