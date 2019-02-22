
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
//#include "Receiver_Position_Data.h"
//#include "VCNL4035X01.h"
#include <math.h>// to use power function
#include "ADCh.h"
#include "SunPos.h"
#include "PI4IOE5V96248.h"
#include "AT25SF041.h"

//#include "Receiver_Position_Data.h"

#define FOSC 18432000L 	 
#define T1MS (65536-FOSC/1000)
//#define PointThree_mm_steps 10
#define PointOne_mm_steps 12

#define PointTwo_mm_steps 24 //(11.5*2)
#define PointFour_mm_steps 48 //


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
void Update_position(unsigned char mnths,unsigned char dys,unsigned char hurs,unsigned char mns,unsigned char sconds,float *currnt_pos,float offset_calib);
void Display_Pos(float sign_dat);
float calibration(unsigned char mnths,unsigned char dys,
										 unsigned char hurs,unsigned char mns,unsigned char sconds,
										 float  *currnt_pos);
unsigned int Day_Of_Year(unsigned char months_bcd,unsigned char days_bcd); // this function is used to count the date in a year example: 22 March is the 80th day of the year
unsigned char BCDtoDec1(unsigned char bcd);
float calib_interpolate(float hours, float mins);
float  linear_interpolate(struct point p1,struct point p2, float  x);
void Write_PI4IOE5V96248(struct DATA_FOR_IO_6PORTS *xdat);
float  linear_interpolate(struct point p1,struct point p2, float  x);
unsigned int ADC_GetResult(unsigned char ch);

bit busy;
unsigned char Rec_data_hour[]="hh",Rec_data_min[]="mm",hour_count,min_count;
int RX_Data_Uart_Cnt=0;
int st_time=0;
int count=0;
static int KeyCount=0;
static unsigned char KeyNum_Old,KeyNum,PressedKey[4]="hhmm";
float calib_value[21],calib_time[21];// 600/calib_stamp+1
unsigned char seconds,mins, hours,days,months,mins1, hours1,mins2, hours2;
float current_position=0;
//int lcd=0;
//calib_value=malloc(24);
//calib_time=malloc(24);
unsigned char calib_stamp =30;// calib every 30 mins 




void tm0_isr() interrupt 1 using 1
{
}

void main(void)
{
//	unsigned char seconds,mins, hours,days,months,mins1, hours1,mins2, hours2;
	unsigned int sunlight_ADC;
	unsigned long int address=0;
	static int KeyCount=0;
	static unsigned char KeyNum_Old,KeyNum,PressedKey[4]="hhmm";	
	char prox_flag=1,SPI_NOR_DATA=0,error=0;
	int iUse_prevday_calib_value=0;
	struct point calib_point1,calib_point2;
//	unsigned char KeyNum;
	int calib_day=0, calib_count;

//	char numStr[5];
	
//	float current_position=0;
	calib_mode=1;
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
	P3M1=0x00;
	P3M0=0xFF;
	P3=0x4C;
	LCD_Init();
	SPI_Init();
	KeyPad_IO_Init();
	//initUART1();
	//I2C_Init();
	ADC_Init();
	//Timer0===================================
	AUXR|=0x80;
	TL0=T1MS;
	TH0=T1MS>>8;
	TMOD=0x00;
	TR0=1;
	ET0=1;
	//========================================
	EA=1; 			// each interrupt source will be enable or disable by setting its interrupt bit	   
	SPI_WriteTime(0x12,Hours);		// data , register address
	Delay_ms(500);
	SPI_WriteTime(0x00,Minutes);
	Delay_ms(500);

	address=0;
	//while(1)
	{
		Wait_ms_SPINOR(5);
		AT25SF041_CS_Set();
		Wait_ms_SPINOR(5);
		//AT25SF041_WriteEnable();

		SPI_NOR_DATA=AT25SF041_Read(Read_Array,6);
		Wait_ms_SPINOR(500);
		if(SPI_NOR_DATA==16||SPI_NOR_DATA==17||SPI_NOR_DATA==18||SPI_NOR_DATA==19)
		{
			WriteData(0x35);
		}
		WriteData(SPI_NOR_DATA);

	}
	

}

	
 




/*void Uart() interrupt 4 using 1
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
}*/


/*float calib_interpolate(float hours, float mins)
{
	struct point p_calib1,p_calib2;
	int i;
	float current_time;
	current_time=(float) (BCDtoDec1(hours))+(float)BCDtoDec1(mins)/60;
	
	for(i=0;i<=count;i++)
	{
		if(current_time >= *(calib_time+i) && current_time<= *(calib_time+i+1))
		{
			p_calib1.x= *(calib_time+i);
			p_calib1.y= *(calib_value+i);
			
			p_calib2.x= *(calib_time+i+1);
			p_calib2.y= *(calib_value+i+1);
			
			break;
		}
	}
	return 0;// for debugging only
	//return linear_interpolate(p_calib1,p_calib2,current_time);	
}*/


/*void SendUART1(unsigned char dat)
{
	while(busy);
	busy=1;
	ACC=dat;
	SBUF=ACC;
}*/


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
		//WriteData(0x6D);//m
		//WriteData(0x6D);//m
		//WriteData(0x20);// "blank"
	}
	else
	{
		
		WriteData(0x2D);// "-"
		WriteData(hundred|0x30);
		WriteData(ten|0x30);
		WriteData(unit|0x30);
		WriteData(0x2E);//.
		WriteData(after_dot|0x30);
		//WriteData(0x6D);//m
		//WriteData(0x6D);//m

	}
	return;
}