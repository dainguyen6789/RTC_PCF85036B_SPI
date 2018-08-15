#include "stc15f2k60s2.h"
#include "LCD_Driver_SPLC780D.h"
#include "Receiver_Position_Data.h"
#include "math.h"
//#include "PCF85963BTL.h"
void Delay_ms(unsigned int ms);
int Day_Of_Year(unsigned char months,unsigned char days);
unsigned char BCDtoDec1(char bcd);
float degree;

unsigned char BCDtoDec1(unsigned char bcd)
{
	unsigned char hi,lo;
	hi=bcd>>4;
	lo=bcd&0x0F;
	return hi*10+lo;
}





void Step_move(unsigned int step, bit dir)
{
			unsigned int i=0;
			if(dir)
				P4 |=(1<<2);// set bit P4.2
			else
				P4 &=~(1<<2);// clear bit P4.2
			
			for( i=0;i<step;i++)
			{
					P4 |=1<<1;// P41=1 // moving distance (mm)=pi^2*step*4/675
					Wait_ms(2);
					P4 &= ~(1<<1);
					Wait_ms(2);
			}
}

void Move(float distance, bit direction)
{
		unsigned int step;
		step= (unsigned int)(distance*337.5/(3.14159));
		Step_move(step,direction);
}

float linear_interpolate(float x1,float x2,float y1,float y2, float x)
{
	float a,b;
	a=(y1-y2)/(x1-x2);
	b=y1-a*x1;
	
	return a*x+b;
}


void Update_position(unsigned char mnths,unsigned char dys,unsigned char hurs,unsigned char mns,unsigned char sconds,float *currnt_pos)
{
	unsigned int date,i,yy;
	float desired_distance,distance=0;
	float pos_interpolate_12_17h[21][1];
	/*hurs=0x12;
	mns=0x30;*/
	//sconds=0x15;
	desired_distance=*currnt_pos;
	date=Day_Of_Year(mnths,dys);
	// interpolate for day
	if (date>=79 && date <=171)
	{
		for (i=0;i<=8;i++)
		{
			if( (date >=date_stamp_79_171[i]) && (date <= date_stamp_79_171[i+1])
				
		}
	}
	// interpolate for hour

}

float pos_day_171(float time){
	return 0.3224*pow(time,4) - 16.883*pow(time,3) + 336.15*pow(time,2) - 3004.8*time + 10281;
}
float pos_day_152_190(float time){
	return 0.3957*pow(time,4) - 20.914*pow(time,3) + 418.74*pow(time,2) - 3752.6*time + 12796;
}

float pos_day_139_203(float time){
 return 0.4751*pow(time,4) - 25.31*pow(time,3) + 509.39*pow(time,2) - 4578.4*time + 15589;
}

float pos_day_121_221(float time){
	return 0.6567*pow(time,4) - 35.589*pow(time,3) + 725.58*pow(time,2)  - 6585.1*time + 22505;
}	

float pos_day_113_229(float time){
	return 0.6698*pow(time,4)- 36.416*pow(time,3) + 744.21*pow(time,2)  - 6765.5*time + 23132;
}
float pos_day_106_236(float time){
	return 0.6886*pow(time,4) - 37.644*pow(time,3) + 772.66*pow(time,2)  - 7048.9*time + 24153;
}
float pos_day_94_248(float time){
	return 0.4628*pow(time,4) - 25.404*pow(time,3) + 523.11*pow(time,2)  - 4784.2*time + 16416;
}
float pos_day_87_255(float time){
	return 0.288*pow(time,4) - 15.904*pow(time,3) + 329.15*pow(time,2)  - 3023.8*time+ 10412;
}
//
/*
	for(i=0;i<13;i++)
	{
		if (BCDtoDec1(hurs)<=Time_stamp_PM[i+1][0] && BCDtoDec1(hurs)>=Time_stamp_PM[i][0] && BCDtoDec1(mns)>= Time_stamp_PM[i][1] && BCDtoDec1(mns)<= Time_stamp_PM[i+1][1] && BCDtoDec1(mns)%5==0)
		{
			desired_distance=(receiver_pos[i+1][0]-receiver_pos[i][0])/(Time_stamp_PM[i+1][0]*60+Time_stamp_PM[i+1][1]-Time_stamp_PM[i][0]*60-Time_stamp_PM[i][1])*(BCDtoDec1(hurs)*60+BCDtoDec1(mns)-Time_stamp_PM[i][0]*60-Time_stamp_PM[i][1])+receiver_pos[i][0];
		}
	}
*/

//int Day_Of_Year(unsigned char months,unsigned char days)*/