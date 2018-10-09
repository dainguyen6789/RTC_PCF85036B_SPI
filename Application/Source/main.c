
#include "stc15f2k60s2.h"
#include "a25lc040.h"
#include <stdio.h>
#include <stdlib.h>
 //#include "hc595.h"
#include "PCF85063BTL.h"
//#include "UART1.h"
#include "KeyPad.h"
#include <REG51F.H>
#include "LCD_Driver_SPLC780D.h"
#include "VCNL4035X01.h"
#include <math.h>// to use power function
//#include "Receiver_Position_Data.h"

#define FOSC 18432000L 	 
#define T1MS (65536-FOSC/1000)

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
void WriteData(unsigned char dat);
void LCD_Init(void);
void DisplayLCD(unsigned char BCD);
void Read_time(unsigned char *months,unsigned char *days,unsigned char *hours,unsigned char *mins,unsigned char *seconds);
void LCD_return_home(void);
void Key_Process(void);
void Display_Line(int line);
void Command(unsigned char dat);//LCD command
unsigned int Read_VCNL4035(unsigned int command_code);
void I2C_Init();
void Display_Prox(unsigned int prox_data);
void Step_move(unsigned int step, bit dir);
void Update_position(unsigned char mnths,unsigned char dys,unsigned char hurs,unsigned char mns,unsigned char sconds,float *currnt_pos);
void Display_Pos(float sign_dat);

bit busy;
unsigned char Rec_data_hour[]="hh",Rec_data_min[]="mm",hour_count,min_count;
int RX_Data_Uart_Cnt=0;
int st_time=0;
static int KeyCount=0;
static unsigned char KeyNum_Old,KeyNum,PressedKey[4]="hhmm";

void tm0_isr() interrupt 1 using 1
{
	//Display_time(&months,&days,&hours,&mins,&seconds);
}

void main(void)
{
	unsigned char seconds,mins, hours,days,months,mins1, hours1,mins2, hours2;
	unsigned int prox_data;
	static int KeyCount=0;
	static unsigned char KeyNum_Old,KeyNum,PressedKey[4]="hhmm";	
	char prox_flag=1;
//	unsigned char KeyNum;
	int count=0;
	char numStr[5];
	float current_position=0;
	direction=1;
	move=0;
	small_move=0;
	auto_mode=0;
	//=======================================
	/*float a=-7.0014e-5;
	float b=1.1071e-2;
	float c=2.1989;
	float d=-2.0858e2;
	float dd=138;*/
//	float rx_pos_12h=a*pow(dd,3)+b*pow(dd,2)+c*dd+d;// pow (base, power)
	//=======================================
	LCD_Init();
	SPI_Init();
	KeyPad_IO_Init();
	//initUART1();
	I2C_Init();
	//Timer0===================================
	AUXR|=0x80;
	TL0=T1MS;
	TH0=T1MS>>8;
	TMOD=0x00;
	TR0=1;
	ET0=1;
	//========================================
	EA=1; 			// each interrupt source will be enable or disable by setting its interrupt bit	   
	//SPI_WriteTime(0x12,Hours);		// data , register address
	Delay_ms(500);
	//SPI_WriteTime(0x12,Minutes);
	Delay_ms(500);
	//==============================================================
	// LCD DISPLAY time format hhmm# to set time on the 1st LCD line
	//==============================================================
	Display_Line(1);	
	WriteData(0x68);//display "h"
	WriteData(0x68);//display "h"
	WriteData(0x6D);//display "m"
	WriteData(0x6D);//display "m"
	WriteData(0x23);//display "#" SETTIME_KEY*/
	//WriteData((int) rx_pos_12h);
	//Step_move(200, 1);// 1.8* step angle, 200 steps ~ 1 round
	while(1)                                      
	{
		Key_Process();
		//count++;
		//if (count==20)
		{
			//move cursor to line 1, pos 6
			
			Command(0x08);
			Command(0x05);
			
			WriteData(0x50);//display "P"
			WriteData(0x4F);//display "O"
			WriteData(0x53);//display "S"	
			WriteData(0x3A);//display ":"	
			//LCD_clear();
			Display_Pos(current_position);
			//==============================================================
			Display_Line(2);
			DisplayLCD(hours);
			WriteData(0x3A);//display ":"
			DisplayLCD(mins);
			WriteData(0x3A);//display ":"
			DisplayLCD(seconds&0x7f);
			WriteData(0x3B);//display ";"
			DisplayLCD(months);
			//WriteData(0x2D);//display "-"
			DisplayLCD(days);	
			WriteData(0x3B);//display ";"
			if(auto_mode)
			{
				WriteData(0x41);//display "A"
			}
			else if (!auto_mode)
			{
				WriteData(0x4D);//display "M"
			}			
			//count=0;
			//prox_data=Read_VCNL4035(PS1_Data_L);
			//Display_Prox(prox_data);// this is RAW data from the Prox Sensor
			// 	y = 12.051x2 - 546.97x + 7153.8;   	 	x in [1015:2880] 	=> 	distance: 	10-20mm
			// 	y = 2.4242x2 - 174.89x + 3545.5;			x in [473:941] 		=> 	distance:		20-30mm
			// 	y = 0.5038x2 - 54.417x + 1651.1; 		  x in [277:455] 		=> 	distance:		30-40mm
			//	y = 0.303x2 - 37.642x + 1302.4; 		  x in [177:269]	 	=> 	distance:		40-50mm
		
		
			//Delay_ms(1);
			//WriteData(Read_VCNL4035(PS3_Data_L));
			if (move && !auto_mode)// prox_data<2880 <=> distance to the sensor >10mm, please view "Test The accuracy and resolution of VCNl4035X01_ILED_20mA.xlxs" file
			{
				Step_move(11, direction);// 1.8* step angle, 200 steps ~ 1 round, 107 steps ~ 1mm movement, l(mm)=step*pi/337.5, L=R1*R3/R2*pi*n/(100*27), R1 is the pulley attached to the motor, R2 is the pulley attached to the long shaft with timing belt, R3 is the long pulley 
				if (direction==1)
				{
					current_position=current_position+0.1;
				}
				else
				{
					current_position=current_position-0.1;
				}
				prox_flag=0;
			}
			if (small_move && !auto_mode)
			{
				
				//auto_mode=0;
				Step_move(11, direction);// 1.8* step angle, 200 steps ~ 1 round, 107 steps ~ 1mm movement, l(mm)=step*pi/337.5, L=R1*R3/R2*pi*n/(100*27), R1 is the pulley attached to the motor, R2 is the pulley attached to the long shaft with timing belt, R3 is the long pulley 
				if (direction==1)
				{
					current_position=current_position+0.1;
				}
				else
				{
					current_position=current_position-0.1;
				}
				prox_flag=0;		
				small_move=0;				
			}

			/*if (prox_data<=300 && prox_flag==0)// prox_data<2880 <=> distance to the sensor >10mm, please view "Test The accuracy and resolution of VCNl4035X01_ILED_20mA.xlxs" file
			{
				current_position=0;
				prox_flag=1;
				move=0;
			}*/
			
		if (P33 && prox_flag==0 && current_position<=0)// prox_data<2880 <=> distance to the sensor >10mm, please view "Test The accuracy and resolution of VCNl4035X01_ILED_20mA.xlxs" file
			{
				current_position=0;
				prox_flag=1;
				move=0;
				small_move=0;
				direction=1;
			}			
			LCD_return_home();
			
		}
		hours2=hours1;
		hours1=hours;
		mins2=mins1;
		mins1=mins;
		Read_time(&months,&days,&hours,&mins,&seconds);

		//Read_time(&months,&days,&hours,&mins,&seconds);
		//Read_time(&months,&days,&hours,&mins,&seconds);
		if(auto_mode)
		{
			//if (mins1==mins2 && mins2==mins && hours1==hours && hours2==hours1)
				//Update_position(months,days,hours,mins,seconds,&current_position);
				Update_position(0x10,0x05,0x12,0x30,0x00,&current_position);
		}
		//==================================================		
		// This is for UART to set the time		
		//==================================================				
	/*	if(st_time)
		{
			SPI_WriteTime(hour_count,Hours);		// data , register address
			SPI_WriteTime(min_count,Minutes);
			st_time=0;
			SendUART1(hour_count);
			SendUART1(min_count);
		}	
		else
		{
			//UART
			//Delay_ms(1200);
		}*/
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


void Display_Prox(unsigned int dat)
{
	unsigned char unit, ten, hundred,thousand;
	unit =dat%10;// remainder after division
	thousand=dat/1000;
	hundred=(dat-thousand*1000)/100;
	ten=(dat-thousand*1000-hundred*100)/10;
	WriteData(thousand|0x30);
	WriteData(hundred|0x30);
	WriteData(ten|0x30);
	WriteData(unit|0x30);
	return;
}


void Display_Pos(float sign_dat)
{
	unsigned char unit, ten, hundred,thousand;
	unsigned char after_dot;
	int dat;
	dat=abs(sign_dat);
	if (sign_dat>0)
		after_dot=(unsigned char)((sign_dat-dat)*10);
	else
		after_dot=(unsigned char)((-sign_dat-dat)*10);
	
	unit =dat%10;// remainder after division
	thousand=dat/1000;
	hundred=(dat-thousand*1000)/100;
	ten=(dat-thousand*1000-hundred*100)/10;
	if (sign_dat>=0)
	{
		//WriteData(thousand|0x30);
		WriteData(hundred|0x30);
		WriteData(ten|0x30);
		WriteData(unit|0x30);
		WriteData(0x2E);//.
		WriteData((after_dot)|0x30);
		WriteData(0x6D);//m
		WriteData(0x6D);//m
		WriteData(0x20);// "blank"
	}
	else
	{
		
		WriteData(0x2D);// "-"
		WriteData(hundred|0x30);
		WriteData(ten|0x30);
		WriteData(unit|0x30);
		WriteData(0x2E);//.
		WriteData(after_dot|0x30);
		WriteData(0x6D);//m
		WriteData(0x6D);//m

	}
	return;
}