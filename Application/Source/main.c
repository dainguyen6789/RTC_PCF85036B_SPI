
#include "stc15f2k60s2.h"
#include "a25lc040.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 //#include "hc595.h"
#include "PCF85063BTL.h"
//#include "UART1.h"
#include "KeyPad.h"
#include <REG51F.H>
#include "UART1.h"

#include "LCD_Driver_SPLC780D.h"
//#include "Receiver_Position_Data.h"
//#include "VCNL4035X01.h"
#include <math.h>// to use power function
#include "ADCh.h"
#include "SunPos.h"
#include "PI4IOE5V96248.h"
#include "AT25SF041.h"
#include "SI1120.h"
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
#define PointFour_mm_steps 28  //
//#define PointFour_mm_steps 42  //

#define DATA_WITHOUT_RUNNING_CALIBRATION 0

//void Delay_ms(unsigned int ms);
//void SendString(char *s);
//void SendUART1(unsigned char dat);
void initUART1(void);
//unsigned char BCDtoDec(unsigned char BCD);
unsigned char ten(unsigned char BCD);
unsigned char unit(unsigned char BCD);
//unsigned char SPI_ReadTime(unsigned char addr);
//void SPI_WriteTime(unsigned char val,unsigned char addr);
//unsigned char ASCIItoBCD(unsigned char ascii[2]); // time format hh:mm:ss
//void SendUART1(unsigned char dat);
//void SPI_Init(void);
//void WriteData(unsigned char dat);
//void LCD_Init(void);

//void Read_time(unsigned char *months,unsigned char *days,unsigned char *hours,unsigned char *mins,unsigned char *seconds);

//unsigned int Read_VCNL4035(unsigned int command_code);
void I2C_Init();
void Step_move(unsigned int step, bit dir);
void Update_position(unsigned char mnths,unsigned char dys,
										 unsigned char hurs,unsigned char mns,
										 unsigned char sconds,
										 float  *currnt_pos, float offset_calib);
float calibration(unsigned char mnths,unsigned char dys,
										 unsigned char hurs,unsigned char mns,unsigned char sconds,
										 float  *currnt_pos,unsigned int *calib_max_ADC_Val,
										 float *theorical_max_pos,unsigned int *max_ADC_JP_value,
										 unsigned long int *NOR_address_to_write);
unsigned char BCDtoDec1(unsigned char bcd);
float calib_interpolate(float hours, float mins);
float  linear_interpolate(struct point p1,struct point p2, float  x);
float elevation_calculation(unsigned char mnths,unsigned char dys,
										 unsigned char hurs,unsigned char mns,unsigned char sconds);
//void uart1_InitTCPConn();

//void uart1_SendToTCPServer(char *str);
//void SendString(char *s);
								 
bit busy;
//unsigned char Rec_data_hour[]="hh",Rec_data_min[]="mm",hour_count,min_count;
//int RX_Data_Uart_Cnt=0;
//int st_time=0;
int count=0;
int i,j,i_point1,i_point2,found_1st_point=0,ii,jj;// loop variables to interpolate the calibration values
static int KeyCount=0;
static unsigned char KeyNum_Old,KeyNum,PressedKey[4]="hhmm";
float calib_value[21],calib_time[21],diff_of_offset=0;// // diff_of_offset is the difference between the calibration value of this day and the day before
int calib_bool[21];
unsigned char old_mins,seconds,mins, hours,days,months;//,mins1, hours1,mins2, hours2;
float current_position=0;
char sTempString[9];
//int lcd=0;
//calib_value=malloc(24);
//calib_time=malloc(24);
unsigned char calib_stamp =30;// calib every 30 mins 
unsigned long int SPI_NOR_INTERNAL_FLASH_ADDR=0;
unsigned int timer0_count=0;
unsigned char start_timer0_count=0;
// definition of pwm time 
float pwm_time=0,pwm_time_min=0,pwm_time_max=0;
float val_to_store[21]={-8,	-8,	-8,	-8,	-8,	-8,	-8,	-8,	-8,	-8,	-8,	-9,	-9.5,	-10.5,	-10.5,	-10.5,	-10.5,	-10.5,	-10.5,	-10.5};

sbit INT0 = 0xB2;

void tm0_isr() interrupt 1 using 1
{
	//SI1120_STX=!SI1120_STX;
	//re-init timer 0
	TL0=T1MS;
	TH0=T1MS>>8;
	timer0_count++;
}
void tm1_isr() interrupt 3 using 1
{
	SI1120_STX=!SI1120_STX;
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
		
			pwm_time=(TH0);//micro second 10^6 divide FOSC
			pwm_time=(pwm_time*pow(2,8)+TL0-T1MS)/18.432;
			pwm_time=pwm_time+timer0_count*1000;//microsesond unit
			timer0_count=0;
			//TL0=T1MS;
			//TH0=T1MS>>8;
			//start_timer0_count=1;
			//time_at_falling_edge=timer0_count;
	}	
	
}

void main(void)
{
//	unsigned char seconds,mins, hours,days,months,mins1, hours1,mins2, hours2;
	//unsigned int sunlight_ADC;
	static int KeyCount=0;
	static unsigned char KeyNum_Old,KeyNum,PressedKey[4]="hhmm";	
	char prox_flag=1;
	int iUse_prevday_calib_value=0;
	//char s="1234";
	unsigned int max_ADC_Val_JP=0,max_ADC_Val=0;
	struct point calib_point1,calib_point2;
	struct data_to_store dat_to_store;
	//	unsigned char KeyNum;
	int calib_day=0, calib_count;
	float theorical_JP_max_pos=0,elevation;
	//	char numStr[5];
	
	//	float current_position=0;
	//calib_mode=1;
	//auto_mode=0;
	
	// select ADC0 port 0
	//	P46       =   0;
	//	P45        =   0;
	//	P27        =   0;	
	direction=1;
	move=0;
	small_move=0;
	
	SPI_NOR_ClearEnable=0;
	//=======================================
	/*float a=-7.0014e-5;
	float b=1.1071e-2;
	float c=2.1989;
	float d=-2.0858e2;
	float dd=138;*/
	//	float rx_pos_12h=a*pow(dd,3)+b*pow(dd,2)+c*dd+d;// pow (base, power)
	//=======================================
	//P3M1&=~(0xF8);
	//P3M0|=0xF8;
	//P3=0x4C;
	LCD_Init();
	SPI_Init();
	KeyPad_IO_Init();
	//I2C_Init();
	ADC_Init();
	//=========================================
	// code to initialize the Light Sensor
	SI1120_CONFIG_MODE_VIRH();
	Enable_EXTINT0();
	//=========================================
	vSetLocation();// set location longtitude and lattitude
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
	SPI_WriteTime(0x09,Hours);		// data , register address
	Delay_ms(100);
	SPI_WriteTime(0x00,Minutes);
	Delay_ms(100);
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
	for(calib_count=0;calib_count<=20;calib_count++)
	{
		calib_value[calib_count]=0;
		calib_time[calib_count]=7+(float)calib_count/2;
		calib_bool[calib_count]=0;
	}
	initUART1();
	//IP|=0x10;
	//IP&=~(0x09);// UART1 lowest priority
	Delay_ms(8000);

	// Connect to the TCP Server (IP,Port)
	uart1_InitTCPConn();
	//SendString("AT+CIPSTART=\"TCP\",\"192.168.11.203\",8080\r\n");
	Delay_ms(500);
	uart1_SendToTCPServer("123");

	//SendString("123\r\n");

		//hours2=hours1;
		//hours1=hours;
		//mins2=mins1;
		//mins1=mins;
		old_mins=mins;
		Read_time(&months,&days,&hours,&mins,&seconds);
		if(BCDtoDec1(mins)%50==0 &&  BCDtoDec1(seconds&0x7f)==0 )
		{		
			//uart1_inittcpconn();
			SendString("AT+CIPSTART=\"TCP\",\"192.168.11.203\",8080\r\n");	
			Delay_ms(300);
			uart1_SendToTCPServer("123");
			Delay_ms(300);

		}
		AT25SF041_WriteEnable();
		//Wait_ms_SPINOR(50);
		AT25SF041_ChipErase();
		Wait_ms_SPINOR(5);
		
		for(j=0;j<=20;j++)
		{
			Wait_ms_SPINOR(50);	
			AT25SF041_WriteEnable();
			//Wait_ms_SPINOR(50);	
			if(val_to_store[j]>0)
				AT25SF041_Write(Byte_Page_Program, 3*j,1);	
			else
				AT25SF041_Write(Byte_Page_Program, 3*j,0);	
				
			Wait_ms_SPINOR(50);						
			Wait_ms_SPINOR(50);	
			AT25SF041_WriteEnable();
			//Wait_ms_SPINOR(50);	
			AT25SF041_Write(Byte_Page_Program, 3*j+1,abs(val_to_store[j]));	
			Wait_ms_SPINOR(50);	
				
			Wait_ms_SPINOR(50);	
			AT25SF041_WriteEnable();
			//Wait_ms_SPINOR(50);	
			AT25SF041_Write(Byte_Page_Program, 3*j+2,(fabs(val_to_store[j])-abs(val_to_store[j]   ))   *100);	
			Wait_ms_SPINOR(50);	
		}
			Wait_ms_SPINOR(50);	
			AT25SF041_WriteEnable();
			//Wait_ms_SPINOR(50);	
			AT25SF041_Write(Byte_Page_Program, 98,0x08);	
			Wait_ms_SPINOR(50);	
		
			Wait_ms_SPINOR(50);	
			AT25SF041_WriteEnable();
			//Wait_ms_SPINOR(50);	
			AT25SF041_Write(Byte_Page_Program, 99,0x25);	
			Wait_ms_SPINOR(50);	
				
		// Code for the PUMP, enable PUMP from 7AM to 5PM
		// Update the Calibation value by interpolation if there are uncalibrated value (decide by calib_bool variable)

				P55=0;
			//Connect_Electronics_Load=0;
			if(iUse_prevday_calib_value==1)// 2nd day
			{
				// clear SPI NOR FLASH
				AT25SF041_WriteEnable();
				//Wait_ms_SPINOR(50);
				AT25SF041_ChipErase();
				Wait_ms_SPINOR(5);				
				// Write calib value of this/2nd day to Memory Block 1 (addr from 0 to 100)
				for(ii=0;ii<=20;ii++)
				{
					if(calib_bool[ii]==1)
					{
						Wait_ms_SPINOR(50);	
						AT25SF041_WriteEnable();
						//	Wait_ms_SPINOR(50);	
						// 	calib bool array is stored from addr 63 
						AT25SF041_Write(Byte_Page_Program, ii+63,calib_bool[ii]);
						Wait_ms_SPINOR(50);	
						AT25SF041_WriteEnable();
						//Wait_ms_SPINOR(50);	
						if(calib_value[ii]>0)
							AT25SF041_Write(Byte_Page_Program, 3*ii,1);	
						else
							AT25SF041_Write(Byte_Page_Program, 3*ii,0);	

						Wait_ms_SPINOR(50);						
						Wait_ms_SPINOR(50);	
						AT25SF041_WriteEnable();
						//	Wait_ms_SPINOR(50);	
						//	AT25SF041_Write(Byte_Page_Program, 3*count+1,(unsigned char)fabs(calib_value[count]));	
						AT25SF041_Write(Byte_Page_Program, 3*ii+1,abs(calib_value[ii]));	

						Wait_ms_SPINOR(50);	

						Wait_ms_SPINOR(50);	
						AT25SF041_WriteEnable();
						//Wait_ms_SPINOR(50);	
						//AT25SF041_Write(Byte_Page_Program, 3*count+2,(fabs(calib_value[count])-(unsigned char)fabs(calib_value[count]))*100);	
						AT25SF041_Write(Byte_Page_Program, 3*ii+2,(fabs(calib_value[ii])-abs(calib_value[ii]))*100);	

						Wait_ms_SPINOR(50);	
					}						
				}				
				
			}

			//===========================================
			// interpolate and justify calibration value

			// // reset this variable so that we can interpolate calib value before the first calibration
			// update iUse_prevday_calib_value variable to identify "not First day" 
			iUse_prevday_calib_value=1;
			found_1st_point=0;

			for(i=0;i<=20;i++)
			{
				calib_value[i]=0;
				calib_bool[i]=0;
				
			}

		}

		// CLEAR SPI NOR by 99990 command from KEyPAd
		//if(SPI_NOR_ClearEnable==1)
	
	


		



		


void Uart() interrupt 4 using 1
{
	if(RI) 
	{
		RI=0;
		
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








