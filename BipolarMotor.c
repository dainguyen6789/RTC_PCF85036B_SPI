#include "stc15f2k60s2.h"
#include "LCD_Driver_SPLC780D.h"
#include "Receiver_Position_Data.h"
#include "math.h"
void Delay_ms(unsigned int ms);
int Day_Of_Year(unsigned char months,unsigned char days);
float degree;
void Step_move(unsigned int step, int direction)
{
			unsigned int i=0;
			for( i=0;i<step;i++)
			{
					P4 |=1<<1;// P41=1 // moving distance (mm)=pi^2*step*4/675
					Wait_ms(2);
					P4 &= (~1<<1);
					Wait_ms(2);
			}
}

void Move(float distance, int direction)
{
		unsigned int step;
		step= distance*675/(4*3.14159*3.14159);
		Step_move(step,direction);
}




/*void Update_position(unsigned char months,unsigned char days,unsigned char hours,unsigned char mins,unsigned char seconds,int direction)
{
	int date,i;
	float distance;
	date=Day_Of_Year(months,days);
	
	for(i=0;i<192;i++)
	{
		if (hours==Time_stamp_PM[0][i] && mins== Time_stamp_PM[1][i]&& seconds==Time_stamp_PM[2][i])
		{
			distance=degree3_function[i][0]*pow(date,3)+degree3_function[i][1]*pow(date,2)+degree3_function[i][2]*date+degree3_function[i][3];
			Move(distance,direction);
			break;
		}
	}
}

//int Day_Of_Year(unsigned char months,unsigned char days)*/