
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
//When T1 in 1T mode(AUXR.6/T1x12=1), the output frequency = (SYSclk) / (256-TH1) / 2
//When T1 in 12T mode(AUXR.6/T1x12=0), the output frequency = (SYSclk) / 12 / (256-TH1) / 2
#define T1MS (65536-FOSC/12/1000) //1ms=1000us T0 overflow = (SYSclk)/(65536-[RL_TH0, RL_TL0])
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

//void Delay_ms(unsigned int ms);
//void SendString(char *s);
//void SendUART1(unsigned char dat);
void initUART1(void);
//unsigned char BCDtoDec(unsigned char BCD);
unsigned char ten(unsigned char BCD);
unsigned char unit(unsigned char BCD);

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
float azimuth_calculation(unsigned char mnths,unsigned char dys,
										 unsigned char hurs,unsigned char mns,unsigned char sconds)		;	
float TheoricalJP_Position(float azimuth,float elevation);

								 
bit busy;
//unsigned char Rec_data_hour[]="hh",Rec_data_min[]="mm",hour_count,min_count;

int count=0,day_offset=0,Enable_UpdateSPINOR=0;
int i,j,i_point1,i_point2,found_1st_point=0,ii,jj;// loop variables to interpolate the calibration values
static int KeyCount=0;
static unsigned char KeyNum_Old,KeyNum,PressedKey[4]="hhmm";
float calib_value[21],calib_time[21],diff_of_offset=0;// // diff_of_offset is the difference between the calibration value of this day and the day before
int calib_bool[21];
unsigned char old_mins,seconds,mins, hours,days,months;//,mins1, hours1,mins2, hours2;
float current_position=0;
char sTempString[9];

unsigned char calib_stamp =30;// calib every 30 mins 
unsigned long int SPI_NOR_INTERNAL_FLASH_ADDR=200;
unsigned int timer0_count=0;
unsigned char start_timer0_count=0;
unsigned int gas_sensor1_ADC[3],
						 gas_sensor2_ADC[3],
						 gas_sensor3_ADC[3],
						 gas_sensor4_ADC[3];
						 
float filted_gas_sensor1_ADC[3],
			filted_gas_sensor2_ADC[3],
			filted_gas_sensor3_ADC[3],
			filted_gas_sensor4_ADC[3];
			

float a[3]={1,-1.77,0.8};
float b[3]={0.006,0.011,0.006};
float gas_sensor_voltage=0;
int flag_500ms=0,n;
char cRead_adc=0;
// definition of pwm time 
float pwm_time=0,pwm_time_min=0,pwm_time_max=0;
sbit INT0 = 0xB2;

void tm0_isr() interrupt 1 using 1
{
	//SI1120_STX=!SI1120_STX;
	//re-init timer 0
	TL0=T1MS;
	TH0=T1MS>>8;
	//gas_sensor_ADC[0]=ADC_GetResult();


}

// When T1 in 1T mode(AUXR.6/T1x12=1), the output frequency = (SYSclk) / (256-TH1) / 2
// When T1 in 12T mode(AUXR.6/T1x12=0), the output frequency = (SYSclk) / 12 / (256-TH1) / 2
void tm1_isr() interrupt 3 using 1
{
	//SI1120_STX=!SI1120_STX;
	//re-init timer 0
	TL1=T1MS;
	TH1=T1MS>>8;
	cRead_adc=1;
	if(flag_500ms!=1000)
		flag_500ms++;
	//if(flag_500ms==500)
	//	flag_500ms=0;

}
void exint0() interrupt 0
{

}

void main(void)
{
	// P1.0 is an INPUT;
	ADC_Init();
	//=========================================

	//Enable_EXTINT0();
	//=========================================
//	vSetLocation();// set location longtitude and lattitude
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


	initUART1();
//	 for(n=0;n<3;n++)
//	 {
//			gas_sensor_ADC[n]=0;
//			filted_gas_sensor_ADC[n]=0;
//		 
//		 	seneca_sense_gas_sensor_ADC[n]=0;
//			seneca_sense_filted_gas_sensor_ADC[n]=0;
//	 }
	 
	while(1) 
	{
		// sample frequency is 1ms ~ 1000Hz
		// cut frequency :    "omega_c=2*fc/fs"
		// https://dsp.stackexchange.com/questions/46903/what-is-the-difference-between-the-sampling-frequency-of-signal-and-sampling-fre
		// fc=25Hz
			if(cRead_adc==1)
			{
//				P24=0;
//				P25=0;
//				P26=0;
				filted_gas_sensor1_ADC[2]=ADC_GetResult(2);// New sample
				//Wait_ms(1000);

//				// IIR Butterworth Low pass Filter
//				for(n=1;n<=2;n++)
//					filted_gas_sensor_ADC[0]+=a[n]*gas_sensor_ADC[n]-b[n]*filted_gas_sensor_ADC[n];
//				
//				
//				gas_sensor_ADC[1]=gas_sensor_ADC[0];
//				gas_sensor_ADC[2]=gas_sensor_ADC[1];

//				filted_gas_sensor_ADC[1]=filted_gas_sensor_ADC[0];
//				filted_gas_sensor_ADC[2]=filted_gas_sensor_ADC[1];
				// Send data to UART. filted_gas_sensor_voltage[0];
				// convert to string with 4 digit after dot.
				
				
								//mux to Y1
//				P24=1;
//				P25=0;
//				P26=0;
				//seneca_sense_filted_gas_sensor_ADC[0]=a[0]*ADC_GetResult(0);// New sample
				filted_gas_sensor2_ADC[2]=ADC_GetResult(3);// New sample
								//Wait_ms(1000);

				filted_gas_sensor3_ADC[2]=ADC_GetResult(4);// New sample
								//Wait_ms(1000);

				filted_gas_sensor4_ADC[2]=ADC_GetResult(5);// New sample
								//Wait_ms(1000);


//				// IIR Butterworth Low pass Filter
//				for(n=1;n<=2;n++)
//					seneca_sense_filted_gas_sensor_ADC[0]+=a[n]*seneca_sense_gas_sensor_ADC[n]-b[n]*seneca_sense_filted_gas_sensor_ADC[n];
//				
//				
//					seneca_sense_gas_sensor_ADC[1]=	seneca_sense_gas_sensor_ADC[0];
//					seneca_sense_gas_sensor_ADC[2]=	seneca_sense_gas_sensor_ADC[1];

//					seneca_sense_filted_gas_sensor_ADC[1]=	seneca_sense_filted_gas_sensor_ADC[0];
//					seneca_sense_filted_gas_sensor_ADC[2]=	seneca_sense_filted_gas_sensor_ADC[1];				
				
				cRead_adc=0;
			}
			if(flag_500ms==1000)
			{
				sprintf(sTempString, "%.4f",filted_gas_sensor1_ADC[2]/1024*5);
				SendString("1");
				SendString(sTempString);
				SendString("\r\n");
				Wait_ms(100);
				sprintf(sTempString, "%.4f",filted_gas_sensor2_ADC[2]/1024*5);
				SendString("2");
				SendString(sTempString);
				SendString("\r\n");
				Wait_ms(100);

				
				sprintf(sTempString, "%.4f",filted_gas_sensor3_ADC[2]/1024*5);
				SendString("3");
				SendString(sTempString);
				SendString("\r\n");
				Wait_ms(100);

				sprintf(sTempString, "%.4f",filted_gas_sensor4_ADC[2]/1024*5);
				SendString("4");
				SendString(sTempString);
				SendString("\r\n");
				Wait_ms(100);

				
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








