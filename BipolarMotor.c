#include "stc15f2k60s2.h"
#include "LCD_Driver_SPLC780D.h"
#include "Receiver_Position_Data.h"
#include "math.h"
void Delay_ms(unsigned int ms);
int Day_Of_Year(unsigned char months,unsigned char days);
void Step_move(unsigned int step, int direction)
{
			unsigned int i=0;
			for( i=0;i<step;i++)
			{
					P4 |=0x06;// P41=1 // moving distance (mm)=pi^2*step*4/675
					Wait_ms(10);
					P4 &= 0xFC;
					Wait_ms(10);
			}
}

void Move(float distance, int direction)
{
		unsigned int step;
		step= distance*675/(4*3.14159*3.14159);
		Step_move(step,direction);
}




void Update_position(unsigned char months,unsigned char days,unsigned char hours,unsigned char mins,unsigned char seconds,int direction)
{
	int date,i;
	float distance;
	date=Day_Of_Year(months,days);
	
	for(i=0;i<192;i++)
	{
		if (hours==Time_stamp[0][i] && mins== Time_stamp[1][i]&& seconds==Time_stamp[2][i])
		{
			distance=degree3_function[0][i]*pow(date,3)+degree3_function[1][i]*pow(date,2)+degree3_function[2][i]*date+degree3_function[3][i];
			Move(distance,direction);
			break;
		}
	}
}

//int Day_Of_Year(unsigned char months,unsigned char days)