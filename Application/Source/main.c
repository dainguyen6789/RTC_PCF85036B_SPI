
#include "stc15f2k60s2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 //#include "hc595.h"
#include "MCP4725.h"
//#include "UART1.h"
#include <REG51F.H>
//#include "UART1.h"

#include "LCD_Driver_SPLC780D.h"
//#include "Receiver_Position_Data.h"
//#include "VCNL4035X01.h"
#include <math.h>// to use power function
//#include "Receiver_Position_Data.h"
#define Block2_MEM_ADDR 100
#define FOSC 18432000L 	 
#define T1MS (65536-FOSC/1000) //1ms=1000us T0 overflow = (SYSclk)/(65536-[RL_TH0, RL_TL0])
sbit Connect_Electronics_Load = P1^7; 
sbit Connect_IV_Load = P1^2; 

//#define T (55000) //1ms=1000us T0 overflow = (SYSclk)/(65536-[RL_TH0, RL_TL0])

//#define T1MS (65536-FOSC/10) //10uS T0 overflow = (SYSclk)/(65536-[RL_TH0, RL_TL0])

////#define PointThree_mm_steps 10
//#define PointOne_mm_steps 10

//#define PointTwo_mm_steps 21 //
//#define PointFour_mm_steps 28  //
#define PointFour_mm_steps 42  //

#define DATA_WITHOUT_RUNNING_CALIBRATION 0

void initUART1(void);
//unsigned char BCDtoDec(unsigned char BCD);
unsigned char ten(unsigned char BCD);
unsigned char unit(unsigned char BCD);

void LCD_Init(void);

void I2C_Init();

										 
//void uart1_InitTCPConn();

//void uart1_SendToTCPServer(char *str);
//void SendString(char *s);
								 
bit busy;
//unsigned char Rec_data_hour[]="hh",Rec_data_min[]="mm",hour_count,min_count;
//int RX_Data_Uart_Cnt=0;
//int st_time=0;
char sTempString[9];
float voltage;
float fosc=7.7,delta_f=0.05;
//int lcd=0;
//calib_value=malloc(24);
//calib_time=malloc(24);
// definition of pwm time 
int i;
sbit INT0 = 0xB2;
void WriteVoltage(float voltage);

void tm0_isr() interrupt 1 using 1
{
	//SI1120_STX=!SI1120_STX;
	//re-init timer 0
	TL0=T1MS;
	TH0=T1MS>>8;
}
void tm1_isr() interrupt 3 using 1
{
	//re-init timer 0
	TL1=T1MS;
	TH1=T1MS>>8;
}
void exint0() interrupt 0
{
	
	//	==== if falling edge  ====
	if(INT0==0)
	{
			TL0=T1MS;
			TH0=T1MS>>8;
			TR0=1;// run timer0
			ET0=1;
			//pwm_time=timer0_count;	// real pwm time=timer0_count*10us
			//timer0_count=0;
			//start_timer0_count=0;		// disable timer0 time. 
	}
	//	==== if rising edge ====
	else
	{
			TR0=0;// stop timer 0
		

			//TL0=T1MS;
			//TH0=T1MS>>8;
			//start_timer0_count=1;
			//time_at_falling_edge=timer0_count;
	}	
	
}

void main(void)
{
	LCD_Init();
	//I2C_Init();
//	ADC_Init();
	//=========================================
	// code to initialize the Light Sensor
	//SI1120_CONFIG_MODE_VIRH();
//	Enable_EXTINT0();
	//=========================================
	//Timer0===================================
	AUXR|=0x80;
	TL0=T1MS;
	TH0=T1MS>>8;
	TMOD=0x00;
	TL1=T1MS;
	TH1=T1MS>>8;
	TR1=1;// run timer1
	ET1=1;
	//========================================
	EA=1; 			// each interrupt source will be enable or disable by setting its interrupt bit	   

	//==============================================================
	// LCD DISPLAY time format hhmm# to set time on the 1st LCD line
	//==============================================================
	Display_Line(1);	
	WriteData(0x68);//display "h"
	WriteData(0x68);//display "h"
	WriteData(0x6D);//display "m"
	WriteData(0x6D);//display "m"
	WriteData(0x23);//display "#" SETTIME_KEY*/

	Write_MCP4725(0xffff);
	
//	fosc=10MHz/N*10k/Rset *(1-Vcontrol/1.13)
//	VCONTROL
//	0V TO 1.13V
	voltage=1.13-1.13*fosc/10;
//	WriteVoltage(3.3);
		Write_MCP4725(0xffff);	
	// sweep the frequency range 8.2+/-0.5MHz at interval of 0.05MHz;
	
//	for(i=0;i<200;i++)
//	{
//			WriteVoltage(voltage+i*delta_f);
//			Wait_ms_i2c(5);
//	}
}
