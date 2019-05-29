
#include "stc15f2k60s2.h"
#include "a25lc040.h"
#include <stdio.h>
#include <stdlib.h>
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

#define FOSC 18432000L 	 
#define T1MS (65536-FOSC/1000) //1ms=1000us T0 overflow = (SYSclk)/(65536-[RL_TH0, RL_TL0])
//#define T (55000) //1ms=1000us T0 overflow = (SYSclk)/(65536-[RL_TH0, RL_TL0])

//#define T1MS (65536-FOSC/10) //10uS T0 overflow = (SYSclk)/(65536-[RL_TH0, RL_TL0])

////#define PointThree_mm_steps 10
//#define PointOne_mm_steps 10

//#define PointTwo_mm_steps 21 //
#define PointFour_mm_steps 28  //
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
static int KeyCount=0;
static unsigned char KeyNum_Old,KeyNum,PressedKey[4]="hhmm";
float calib_value[21],calib_time[21];// 600/calib_stamp+1
int calib_bool[21];
unsigned char seconds,mins, hours,days,months;//,mins1, hours1,mins2, hours2;
float current_position=0;
//int lcd=0;
//calib_value=malloc(24);
//calib_time=malloc(24);
unsigned char calib_stamp =30;// calib every 30 mins 
unsigned long int SPI_NOR_INTERNAL_FLASH_ADDR=0;
unsigned int timer0_count=0;
unsigned char start_timer0_count=0;
// definition of pwm time 
float pwm_time=0,pwm_time_min=0,pwm_time_max=0;
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
			pwm_time=(pwm_time*pow(2,8)+TL0-T1MS)/27;
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
	P3=0x4C;
	LCD_Init();
	SPI_Init();
	KeyPad_IO_Init();
	initUART1();
	//I2C_Init();
	ADC_Init();
	//=========================================
	// code to initialize the Light Sensor
	SI1120_CONFIG_MODE_VIRH();
	Enable_EXTINT0();
	//=========================================
	vSetLocation();// set location longtitude an
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
	SPI_WriteTime(0x12,Hours);		// data , register address
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
	}
	Delay_ms(5000);

	// Connect to the TCP Server (IP,Port)
	SendString("AT+CIPSTART=\"TCP\",\"192.168.11.203\",8080\r\n");
	Delay_ms(2000);

	//SendString("123\r\n");
	while(1)                                      
	{
		Key_Process();
		//uart1_SendToTCPServer("123");
		SendString("AT+CIPSEND=3\r\n");
		Delay_ms(1000);
		SendString("456\r\n");
		Delay_ms(1000);

		//SendString("123\r\n");
		//SendUART1(0x41);
		//count++;
		//if (count==20)
		{
			//Write_PI4IOE5V96248(&dat);
			//Display_Pos(current_position);
			//move cursor to line 1, pos 6
			Command(0x08);
			Command(0x06);
			
			WriteData(0x50);//display "P"
			//WriteData(0x4F);//display ""
			WriteData(0x53);//display "S"	
			WriteData(0x3A);//display ":"	
			//LCD_clear();
			Display_Pos(current_position);
			
			//Display_Pos(pwm_time);
			WriteData(0x6D);//m
			WriteData(0x6D);//m
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
				Step_move(PointFour_mm_steps, direction);// 1.8* step angle, 200 steps ~ 1 round, 107 steps ~ 1mm movement, l(mm)=step*pi/337.5, L=R1*R3/R2*pi*n/(100*27), R1 is the pulley attached to the motor, R2 is the pulley attached to the long shaft with timing belt, R3 is the long pulley 
				if (direction==1)
				{
					current_position=current_position+0.4;
				}
				else
				{
					current_position=current_position-0.4;
				}
				prox_flag=0;
			}
			if (small_move && !auto_mode)
			{
				
				//auto_mode=0;
				Step_move(PointFour_mm_steps, direction);// 1.8* step angle, 200 steps ~ 1 round, 107 steps ~ 1mm movement, l(mm)=step*pi/337.5, L=R1*R3/R2*pi*n/(100*27), R1 is the pulley attached to the motor, R2 is the pulley attached to the long shaft with timing belt, R3 is the long pulley 
				if (direction==1)
				{
					current_position=current_position+0.4;
				}
				else
				{
					current_position=current_position-0.4;
				}
				prox_flag=0;		
				small_move=0;				
			}
			
		if (P23 && prox_flag==0 && current_position<=0)// prox_data<2880 <=> distance to the sensor >10mm, please view "Test The accuracy and resolution of VCNl4035X01_ILED_20mA.xlxs" file
			{
				current_position=0;
				prox_flag=1;
				move=0;
				small_move=0;
				direction=1;
			}			
			LCD_return_home();
			
		}
		//hours2=hours1;
		//hours1=hours;
		//mins2=mins1;
		//mins1=mins;
		Read_time(&months,&days,&hours,&mins,&seconds);

		//Read_time(&months,&days,&hours,&mins,&seconds);
		//Read_time(&months,&days,&hours,&mins,&seconds);
//		if(auto_mode)
//		{
//			
//			//sunlight_ADC=ADC_GetResult(2);
//			//if (mins1==mins2 && mins2==mins && hours1==hours && hours2==hours1)// prevent the noise of I2C on the demo board
//			{
//						if(BCDtoDec1(mins)%calib_stamp==0 &&  BCDtoDec1(seconds&0x7f)==0 )
//						{
//								if(BCDtoDec1(hours)<=16  && BCDtoDec1(hours)>=7 )
//								{
//									//calculate elevation to decide whether we will calibrate or not
//									elevation=elevation_calculation(months,days,hours,mins,seconds);
//									//10log10(photoR)=-0.4424*10log10(lux)+41.311
//									//if(sunlight_ADC>=sunlight_ADC_Threshold*sin(elevation))
//									//============IF SUN LIGHT IS GOOD=======================
//									if(pwm_time>=(563.91*cos(elevation)+33.99+10.5)) // 10.5 is the sensor offset value //experiment linear function: pwm_time(us)=0.623*light(w/m2)+55.581 05may2019 @Fullum
//									//=======================================================
//									//if(pwm_time>=5) //experiment linear function: pwm_time(us)=0.623*light(w/m2)+55.581 05may2019 @Fullum

//									//light GHI(W/m2) = 1.5648xPWM_time - 53.194 
//									//DNI=0.85*GHI/cos(elevation)>750W/m2 then calibrate
//									//GHI>=para
//									//GHI=pwm_time
//									{
//										count=((float)BCDtoDec1(hours)+(float)BCDtoDec1(mins)/60-7)*60/calib_stamp;
//										calib_bool[count]=1;
//										calib_value[count]=calibration(months,days,hours,mins,seconds,&current_position,&max_ADC_Val,&theorical_JP_max_pos,&max_ADC_Val_JP,&SPI_NOR_INTERNAL_FLASH_ADDR);// find the real max value within JP max +/- 10mm
//										// if light is not stable
//										if(pwm_time_max>1.2*pwm_time_min)
//										{
//											// go back to JP pos if the sun light is unstable
//											Update_position(months,days,hours,mins,seconds,&current_position,0);
//											calib_value[count]=0;
//											calib_bool[count]=0;


//										}
//									}
//									else // Store the data even in low light condition
//									{
//											// store  120 bytes of "0" value when calibration does not work  in order to synchronize the pattern.
//											for(count=0;count<=323;count++)
//											{
//												AT25SF041_WriteEnable();
//												AT25SF041_Write(Byte_Page_Program, SPI_NOR_INTERNAL_FLASH_ADDR,DATA_WITHOUT_RUNNING_CALIBRATION);	
//												Wait_ms_SPINOR(50);	
//												SPI_NOR_INTERNAL_FLASH_ADDR++;
//											}		
//											//use count variable to identify the position of calib_value[count]
//											count=((float)BCDtoDec1(hours)+(float)BCDtoDec1(mins)/60-7)*60/calib_stamp;
//											calib_bool[count]=0;

//											
//											Update_position(months,days,hours,mins,seconds,&current_position,calib_value[count]);
//											theorical_JP_max_pos=current_position-calib_value[count];								
//											max_ADC_Val = ADC_GetResult(0);// read from channel 0
//											max_ADC_Val_JP = max_ADC_Val;										
//									}

//									dat_to_store.month=months;
//									dat_to_store.date=days;
//									dat_to_store.hour=hours;
//									dat_to_store.min=mins;
//									
//									dat_to_store.calib_max_voltage_ADC=max_ADC_Val/4;
//									dat_to_store.calib_max_pos_floor=(unsigned char)current_position;
//									dat_to_store.calib_max_pos_float=(current_position-dat_to_store.calib_max_pos_floor)*100;// consider only 2 digit after .
//									dat_to_store.light_ADC=(unsigned char)(pwm_time/4);//
//									
//									dat_to_store.Voltage_at_LUT_pos=max_ADC_Val_JP/4;// Scale the ADC value into the range [0:255]
//									dat_to_store.LUT_max_pos_floor=(unsigned char)theorical_JP_max_pos;
//									dat_to_store.LUT_max_pos_float=(theorical_JP_max_pos-dat_to_store.LUT_max_pos_floor)*100;								
//									Wait_ms_SPINOR(50);
//									//TOTAL: 120bytes for calib + 11 Bytes for  dat_to_store= 131 BYTES
//									SPI_NOR_Write_Data(dat_to_store,&SPI_NOR_INTERNAL_FLASH_ADDR);//0 is the starting address of SPI NOR
//					

//								}
//								
//								else if (BCDtoDec1(hours)>=17)// do not calib after 17pm
//								{
//									iUse_prevday_calib_value=1;
//									//count=0;
//								}

//											
//					}
//					if(iUse_prevday_calib_value==0)// FIRST day of calibration, update position not at the calibration time stamp
//					{
//								if(BCDtoDec1(hours)<=16  && BCDtoDec1(hours)>=7)
//								{
//										count=((float)BCDtoDec1(hours)+(float)BCDtoDec1(mins)/60-7)*60/calib_stamp;
//										Update_position(months,days,hours,mins,seconds,&current_position,calib_value[count]);
//								}
//							
//					}
//					
//					// how to update for next day and use the calib value from the previous day???
//					else
//					{
//						
//							// calib every 30mins, from 7AM to 17PM
//								if(BCDtoDec1(hours)<=16  && BCDtoDec1(hours)>=7)								
//								{
//										count=((float)BCDtoDec1(hours)+(float)BCDtoDec1(mins)/60-7)*60/calib_stamp;

//										calib_point1.x=calib_time[count];
//										calib_point1.y=calib_value[count];
//										calib_point2.x=calib_time[count+1];// this is from previous day.
//										if(calib_bool[count]==1 && calib_bool[count+1]==1)
//										{
//											calib_point2.y=calib_value[count+1];// this is from previous day.
//											Update_position(months,days,hours,mins,seconds,&current_position,linear_interpolate(calib_point1,calib_point2,(float)BCDtoDec1(hours)+(float)BCDtoDec1(mins)/60));
//										}
//										else // was not calib prev day at this time stamp=> use the latest calib value of the same day
//										{
//											Update_position(months,days,hours,mins,seconds,&current_position,calib_value[FindClosestSamedayCalibTime(calib_bool,count)]);
//										}
//										// in the UPDATE function, we only update the motor position when the distance >0.5mm


//								}
//							
//					}
//				}

//		}	
				
		// Code for the PUMP, enable PUMP from 7AM tp 5PM
		if(BCDtoDec1(hours)<=16  && BCDtoDec1(hours)>=7)
				P55=1;
		else
				P55=0;
		
		// CLEAR SPI NOR by 99990 command from KEyPAd
		if(SPI_NOR_ClearEnable==1)
		{
			LCD_clear();
			Display_Line(1);
			WriteData(0x43);//display "C"
			WriteData(0x4C);//display "L"
			WriteData(0x45);//display "E"	
			WriteData(0x41);//display "A"	
			WriteData(0x52);//display "R"	
		//	WriteData(0x2E);//display "."	
		//	WriteData(0x2E);//display "."	
		//	WriteData(0x46);//display "F"	
		//	WriteData(0x4C);//display "L"	
		//	WriteData(0x41);//display "A"	
		//	WriteData(0x53);//display "S"	
		//	WriteData(0x48);//display "H"	

			//==================================
			AT25SF041_WriteEnable();
			//Wait_ms_SPINOR(50);
			AT25SF041_ChipErase();
			Wait_ms_SPINOR(5);
			SPI_NOR_INTERNAL_FLASH_ADDR=0;
			SPI_NOR_ClearEnable=0;
			//===================================
			LCD_clear();
			//Command(0x08);
			//Command(0x00);			
			//WriteData(0x68);//display "h"
			//WriteData(0x68);//display "h"
			//WriteData(0x6D);//display "m"
			//WriteData(0x6D);//display "m"
			//WriteData(0x23);//display "#" SETTIME_KEY		
			
		}
	}

		



		
}
	
 




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







