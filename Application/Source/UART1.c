#include "stc15f2k60s2.h"
#include "PCF85063BTL.h"
#include <stdlib.h>
#include "UART1.h"
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

void SendDecValtoUART(unsigned char dat)
{
	unsigned char hundred, ten, unit;
	hundred=dat/100;
	ten=(dat-hundred*100)/10;
	unit=dat-hundred*100-ten*10;
	SendUART1(hundred+48);// ASCII Value
	SendUART1(ten+48);
	SendUART1(unit+48);
	
}

void SendBCDValtoUART(unsigned char dat)
{
	unsigned char ten, unit;
	ten=dat/16;
	unit=dat%16;
	SendUART1(ten+48);
	SendUART1(unit+48);	
	
}
// Change to read the ALL the data +/-20mm range and record the data every 0.5mm movement.
void SendSPIDataToUART(unsigned char dat,unsigned long int adr)
{
	// 4bytes*(81 calib)=324+11
	if(adr%335==0)
	{
		//SendString("\r \n Sample (ADC_SUN,ADC_Cell,calib_pos_floor_float): ");
		SendString("\r\n");
		SendDecValtoUART(dat);
		SendString(";");
	}
	//ADC val of solar cell when calib
	else if(adr%335<324  && adr%335!=0)
	{
		SendDecValtoUART(dat);
		if ((adr%335)%4==3)//  remain 0, 
		{
			if(adr%335!=323 && adr%335!=0)
				SendString("\r\n");// make new line
			else
				SendString(";");
		}
		else if((adr%335)%4!=3)
		{
			SendString(";");
		}
		
	}
	
	else if (adr%335>=324)
	{
		if(adr%335==324 )
		{
			SendString("MM-DD-HH-MN-MAX_CELL_VOL-MAX_CALIB_POS[2]-SUNLIGHT_ADC-ADC_LUT-PosLUT[2] ");

			SendBCDValtoUART(dat);
			SendString(",");
		}
		else if(adr%335==325)
		{
			SendBCDValtoUART(dat);
			SendString(",");
		}	
		else if(adr%335==326)
		{
			SendBCDValtoUART(dat);
			SendString(",");
		}				
		else if(adr%335==327)
		{
			SendBCDValtoUART(dat);
			SendString(",");
		}			
		else
		{
			SendDecValtoUART(dat);
			SendString(":");
		}
	}
}

void SendUART1(unsigned char dat)
{
	while(busy);
	busy=1;
	ACC=dat;
	SBUF=ACC;
}



 