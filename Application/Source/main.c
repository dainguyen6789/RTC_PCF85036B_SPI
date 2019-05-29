
#include "stc15f2k60s2.h"
#include "a25lc040.h"
#include <stdio.h>
#include <stdlib.h>
 //#include "hc595.h"
#include "PCF85063BTL.h"
//#include "UART1.h"

 	 

void Delay_ms(unsigned int ms);
void SendString(char *s);
void SendUART1(unsigned char dat);
void initUART1(void);
unsigned char BCDtoDec(unsigned char BCD);
unsigned char ten(unsigned char BCD);
unsigned char unit(unsigned char BCD);
unsigned char SPI_ReadTime(unsigned char addr);
void SPI_WriteTime(unsigned char val,unsigned char addr);
unsigned char ASCIItoBCD(unsigned char ascii[2]); // time format hh:mm:ss
void SendUART1(unsigned char dat);
void SPI_Init(void);

bit busy;
unsigned char Rec_data_hour[]="hh",Rec_data_min[]="mm",hour_count,min_count;
int RX_Data_Uart_Cnt=0;
int st_time=0;


void main(void)
{
	unsigned char seconds,mins, hours;
	int sec_decimal;
	char numStr[5];
	SPI_Init();
	initUART1();
	EA=1; 			// each interrupt source will be enable or disable by setting its interrupt bit	   
	SPI_WriteTime(0x12,Hours);		// data , register address
	Delay_ms(500);
	SPI_WriteTime(0x12,Minutes);
	Delay_ms(15000);
	SendString("AT+CIPSTART=\"TCP\",\"192.168.11.203\",8080\r\n");
	Delay_ms(1200);

	while(1)

	{
			SendString("AT+CIPSEND=3\r\n");
			Delay_ms(2200);

			SendString("123\r\n");
			Delay_ms(2200);
			SendString("AT+CIPSEND=3\r\n");
			Delay_ms(2200);		
			SendString("50\r\n");
			Delay_ms(2200);
			SendString("AT+CIPSEND=3\r\n");
			Delay_ms(2200);		
			SendString("200\r\n");
			Delay_ms(2200);
//		if(st_time)
//		{
//			SPI_WriteTime(hour_count,Hours);		// data , register address
//			SPI_WriteTime(min_count,Minutes);
//			st_time=0;
//			SendUART1(hour_count);
//			SendUART1(min_count);
//		}	
//		else
//		{
//			Display_time();
//			Delay_ms(1200);
//		}
	}
	
} 

void Delay_ms(unsigned int ms)
{
  unsigned int De_Cnt;
  while( (ms--) != 0)
  {
    for(De_Cnt = 0; De_Cnt < 600; De_Cnt++); 
  }             
}


void Uart() interrupt 4 using 1
{
	if(RI) 
	{
		RX_Data_Uart_Cnt++;
		RI=0;
		if (RX_Data_Uart_Cnt<=2)
		Rec_data_hour[RX_Data_Uart_Cnt-1]=SBUF;
		else if (RX_Data_Uart_Cnt>=3)
		{
			//RI=0; //SW clear
			//P0=Rec_data;
			Rec_data_min[RX_Data_Uart_Cnt-3]=SBUF;
			if (RX_Data_Uart_Cnt==4)
			{
				RX_Data_Uart_Cnt=0;
				hour_count=ASCIItoBCD(Rec_data_hour);
				min_count=ASCIItoBCD(Rec_data_min);
				st_time=1;
				//SendUART1(hour_count);
				//SendUART1(min_count);
				//SPI_WriteTime(hour_count,Hours);		// data , register address
				//SPI_WriteTime(min_count,Minutes);
			}
		}

		
	}
	if(TI)
	{
		TI=0;
		busy=0;
	}
}


void SendUART1(unsigned char dat)
{
	while(busy);
	busy=1;
	ACC=dat;
	SBUF=ACC;
}
