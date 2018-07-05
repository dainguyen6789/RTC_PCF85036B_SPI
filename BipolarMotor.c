#include "stc15f2k60s2.h"
#include "LCD_Driver_SPLC780D.h"
void Delay_ms(unsigned int ms);
void Move(unsigned int step, int direction)
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
