#include "stc15f2k60s2.h"
#include "LCD_Driver_SPLC780D.h"
#include "Receiver_Position_Data.h"
#include "math.h"
void Delay_ms(unsigned int ms);
int Day_Of_Year(unsigned char months,unsigned char days);
float degree;







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
		step= distance*675/(4*3.14159*3.14159);
		Step_move(step,direction);
}




void Update_position(unsigned char months,unsigned char days,unsigned char hours,unsigned char mins,unsigned char seconds,int *current_pos)
{
	int date,i;
	int desired_distance,distance;
	date=Day_Of_Year(months,days);
	
	for(i=0;i<47;i++)// time stamp from 12PM to 15PM, 47 diff values
	{
		if (hours==Time_stamp_PM[0][i] && mins== Time_stamp_PM[1][i]&& seconds==Time_stamp_PM[2][i])// check if current time match the time stamp in the table
		{			
			switch(date)
			{
				case 79:
					desired_distance=receiver_pos[i][0];
					break;
				case 80:
					desired_distance=receiver_pos[i][1];
					break;
				case 81:
					desired_distance=receiver_pos[i][2];
					break;
				case 82:
					desired_distance=receiver_pos[i][3];
					break;
				case 83:
					desired_distance=receiver_pos[i][4];
			}
			
			distance=desired_distance-*current_pos;
			
			if(distance>0)
			{
				Move(abs(distance),1);
			}
			else if(distance<0)
			{
				Move(abs(distance),0);
			}
			
			break;
		}
	}
}

//int Day_Of_Year(unsigned char months,unsigned char days)*/