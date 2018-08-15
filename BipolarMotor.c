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
	if(date<=79 && date<=171)
	{
		for (i=0;i<=8;i++)
		{
			if (date>= date_stamp_79_171[i] && date<= date_stamp_79_171[i+1])
			{
				for (yy=0;yy<21;yy++)
				{
					pos_interpolate_12_17h[yy][0]=linear_interpolate(date_stamp_79_171[i],date_stamp_79_171[i+1]
					,exact_rx_pos_simulation[yy][i],exact_rx_pos_simulation[yy][i+1],date);
				}
			}
		}
		
		for(i=0;i<num_of_time_stamp;i++)// 
		{
			if (BCDtoDec1(hurs)==Time_stamp_PM[i][0] && BCDtoDec1(mns)== Time_stamp_PM[i][1] && BCDtoDec1(sconds&0x7f)==Time_stamp_PM[i][2])// check if current time match the time stamp in the table
			{			
				desired_distance=pos_interpolate_12_17h[i][0];//18July2018 is stored in the 1st column
			}
				
				//desired_distance=receiver_pos[0][4];;
				//distance=11;
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
				
			
			if ((i+1)<num_of_time_stamp && ((BCDtoDec1(hurs)*60+BCDtoDec1(mns))<=(Time_stamp_PM[i+1][0]*60+Time_stamp_PM[i+1][1])) && ((BCDtoDec1(hurs)*60+BCDtoDec1(mns))>=(Time_stamp_PM[i][0]*60+Time_stamp_PM[i][1])) && BCDtoDec1(sconds&0x7f)%30==0)// update every 30 sec
			{

				desired_distance=linear_interpolate(Time_stamp_PM[i][0]+Time_stamp_PM[i][1]/60,
																						Time_stamp_PM[i+1][0]+Time_stamp_PM[i+1][1]/60,pos_interpolate_12_17h[i][0],
																						pos_interpolate_12_17h[i+1][0],(float)BCDtoDec1(hurs)+(float)BCDtoDec1(mns)/60+(float)BCDtoDec1(sconds&0x7f)/3600);
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
				
				
			}
			
		}
	}
	if(date<=263 && date>171)
	{
		for (i=1;i<=8;i++)
		{
			if (date>= date_stamp_171_263[i] && date<= date_stamp_171_263[i+1])
			{
				for (yy=0;yy<21;yy++)
				{
					pos_interpolate_12_17h[yy][0]=linear_interpolate(date_stamp_171_263[i],date_stamp_171_263[i+1]
					,exact_rx_pos_simulation[yy][8-i],exact_rx_pos_simulation[yy][8-i-1],date);
				}
			}
		}
		
		for(i=0;i<num_of_time_stamp;i++)// 
		{
			if (BCDtoDec1(hurs)==Time_stamp_PM[i][0] && BCDtoDec1(mns)== Time_stamp_PM[i][1] && BCDtoDec1(sconds&0x7f)==Time_stamp_PM[i][2])// check if current time match the time stamp in the table
			{			
				desired_distance=pos_interpolate_12_17h[i][0];//18July2018 is stored in the 1st column
			}
				
				//desired_distance=receiver_pos[0][4];;
				//distance=11;
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
				
			
			if ((i+1)<num_of_time_stamp && ((BCDtoDec1(hurs)*60+BCDtoDec1(mns))<=(Time_stamp_PM[i+1][0]*60+Time_stamp_PM[i+1][1])) && ((BCDtoDec1(hurs)*60+BCDtoDec1(mns))>=(Time_stamp_PM[i][0]*60+Time_stamp_PM[i][1])) && BCDtoDec1(sconds&0x7f)%30==0)// update every 30 sec
			{

				desired_distance=linear_interpolate(Time_stamp_PM[i][0]+Time_stamp_PM[i][1]/60,
																						Time_stamp_PM[i+1][0]+Time_stamp_PM[i+1][1]/60,pos_interpolate_12_17h[i][0],
																						pos_interpolate_12_17h[i+1][0],(float)BCDtoDec1(hurs)+(float)BCDtoDec1(mns)/60+(float)BCDtoDec1(sconds&0x7f)/3600);
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
				
				
			}
			
		}
	}
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