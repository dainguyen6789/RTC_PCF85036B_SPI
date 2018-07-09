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

void Read_time(unsigned char *months,unsigned char *days,unsigned char *hours,unsigned char *mins,unsigned char *seconds)
{
		//unsigned char seconds,mins, hours;
		*months=SPI_ReadTime(Months);
	
		*days=SPI_ReadTime(Days);
	
		*hours=SPI_ReadTime(Hours);
		/*SendUART1(ten(*hours)+48);
		SendUART1(unit(*hours)+48);		
		SendUART1(0x3A);// ":" ASCII letter*/
		
		*mins=SPI_ReadTime(Minutes);
		/*SendUART1(ten(*mins)+48);
		SendUART1(unit(*mins)+48);		
		SendUART1(0x3A);// ":" ASCII letter*/
		
		*seconds=SPI_ReadTime(Seconds);
		//P0=*seconds;
		/*SendUART1(ten(*seconds&0x7f)+48);
		SendUART1(unit(*seconds&0x7f)+48);
		SendUART1(10);			//new line*/
}

int Day_Of_Year(unsigned char months,unsigned char days) // this function is used to count the date in a year example: 22 March is the 80th day of the year
{
	
	switch(months)
	{
	case 1: // Jan
		return days;
		break;
	case 2:// Feb
		return 31+days;
		break;
	case 3:// Mar
		return 31+28+days;
		break;
	case 4: //April
		return 31*2+28+days;
		break;
	case 5: // May
		return 31*2+28+30+days;
		break;
	case 6: // June
		return 31*3+28+30+days;
		break;	
	case 7: // July
		return 31*3+28+30*2+days;
		break;		
	case 8: //August
		return 31*4+28+30*2+days;
		break;		
	case 9: // September
		return 31*5+28+30*2+days;
		break;
	case 10: //	Oct
		return 31*5+28+30*3+days;
		break;	
	case 11: // Nov
		return 31*6+28+30*3+days;
		break;
	case 12: // Dec
		return 31*6+28+30*4+days;
		break;
}	
}
