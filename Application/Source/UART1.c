#include "stc15f2k60s2.h"
#include "PCF85063BTL.h"
#include <stdlib.h>
//#include "UART1.h"
#define TBAUD (65536-FOSC/4/BAUD)
#define FOSC 18432000L
#define BAUD 115200




unsigned char SPI_ReadTime(unsigned char addr);
void SPI_WriteTime(unsigned char val,unsigned char addr);
unsigned char ASCIItoBCD(unsigned char ascii[2]); // time format hh:mm:ss
void SendUART1(unsigned char dat);





void initUART1(void)
{
	SCON=0x50; //0101 0000 8-bit uart,  baud rate variable
	AUXR |=0x95;//10010101 Timer2 as Baud Rate generator
	T2L=TBAUD;
	T2H=TBAUD>>8;	
	ES=1; 					// enable uart1 interrupt
	EA=1;						// each interrupt source will be enable or disable by setting its interrupt bit
	//PS=0; 					// Serial Port 1 interrupt priority control bit, DS page 561
}




 void SendString(char *s)
{
	while(*s)
	{
		SendUART1(*s++);
	}
}

unsigned char ten(unsigned char BCD)
{
	return (BCD)>>4;
}
unsigned char unit(unsigned char BCD)
{

	//unit=BCD&0x0f;
	//dec=ten*10+unit;
	return BCD&0x0f;
}
unsigned char DectoBCD(unsigned char dec)
{
	
	return dec;
}
//if dat=1, we need to use ASCII value to display "1" on UART1
unsigned char ASCII_Letter(unsigned char dat)
{
	
	return dat+48;
}

unsigned char ASCIItoBCD(unsigned char ascii[2]) // time format hh:mm:ss
{
	unsigned char dec_val,ten, unit;
	dec_val=atoi(ascii);
	ten=dec_val/10;
	unit=dec_val%10;
	return ten<<4|unit;
	
}
void Display_time(unsigned char *hours,unsigned char *mins,unsigned char *seconds)
{
		//unsigned char seconds,mins, hours;

		*hours=SPI_ReadTime(Hours);
		SendUART1(ten(*hours)+48);
		SendUART1(unit(*hours)+48);		
		SendUART1(0x3A);// ":" ASCII letter
		
		*mins=SPI_ReadTime(Minutes);
		SendUART1(ten(*mins)+48);
		SendUART1(unit(*mins)+48);		
		SendUART1(0x3A);// ":" ASCII letter
		
		*seconds=SPI_ReadTime(Seconds);
		P0=*seconds;
		SendUART1(ten(*seconds&0x7f)+48);
		SendUART1(unit(*seconds&0x7f)+48);
		SendUART1(10);			//new line
}

 