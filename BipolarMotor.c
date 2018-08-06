#include "stc15f2k60s2.h"
#include "LCD_Driver_SPLC780D.h"
#include "Receiver_Position_Data.h"
#include "math.h"
//#include "PCF85963BTL.h"
void Delay_ms(unsigned int ms);
int Day_Of_Year(unsigned char months,unsigned char days);
unsigned char BCDtoDec1(char bcd);
float trend_line_30_31(float time);
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




void Update_position(unsigned char mnths,unsigned char dys,unsigned char hurs,unsigned char mns,unsigned char sconds,float *currnt_pos)
{
	unsigned int date,i;
	float desired_distance,distance=0,t;
	/*hurs=0x12;
	mns=0x30;*/
	//sconds=0x15;
	desired_distance=*currnt_pos;
	date=Day_Of_Year(mnths,dys);
	//date=80;
	for(i=0;i<num_of_time_stamp;i++)// 13 diff values of time stamp
	{
		if ((i+1)<num_of_time_stamp && ((BCDtoDec1(hurs)*60+BCDtoDec1(mns))<=(Time_stamp_PM[i+1][0]*60+Time_stamp_PM[i+1][1])) && ((BCDtoDec1(hurs)*60+BCDtoDec1(mns))>=(Time_stamp_PM[i][0]*60+Time_stamp_PM[i][1])) && BCDtoDec1(mns)%2==0)// update every 2 mins
		{
			//switch(date)
			{
				if (date==217)//05Aout2018
				{
					t=(float)(BCDtoDec1(hurs)*60+BCDtoDec1(mns));
					desired_distance=trend_line_30_31(t/60);
				}
			}
			distance=desired_distance-*currnt_pos;
			*currnt_pos=desired_distance;// change to sync with step movement
			if(distance>0)
			{
				Move(distance,1);// counter clock wise
			}
			else if(distance<0)
			{
				Move(-distance,0);// clock wise
			}
			else
			{
			} 
			
			break;
			
		}
		
	}
}

float trend_line_30_31(float time)
{
	return (-4.88*time*time+126.08*time-745.26+(1.1*5.5));//for 5Aout2018
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