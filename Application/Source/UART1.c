#include "stc15f2k60s2.h"
#include "PCF85063BTL.h"
#include <stdlib.h>
#include "UART1.h"
#include <string.h>
#define TBAUD (65536-FOSC/4/BAUD)
#define FOSC 18432000L
#define BAUD 115200




unsigned char SPI_ReadTime(unsigned char addr);
void SPI_WriteTime(unsigned char val,unsigned char addr);
unsigned char ASCIItoBCD(unsigned char ascii[2]); // time format hh:mm:ss
void SendUART1(unsigned char dat);

void uart1_InitTCPConn();

void uart1_SendToTCPServer(char *str);
void Delay_ms(unsigned int ms);


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

void uart1_InitTCPConn()
{
	//SendString("AT\r\n");
	//while(RI==0);
	//RI=0;
	SendString("AT+CIPSTART=\"TCP\",\"192.168.11.203\",8080\r\n");	
}

void uart1_SendToTCPServer(char *str)
{
	SendString("AT+CIPSEND=16\r\n");
	Delay_ms(1000);
	SendString("Hello from ESP\r\n");
	//SendString("\r\n");
	//Delay_ms(1000);

}


 