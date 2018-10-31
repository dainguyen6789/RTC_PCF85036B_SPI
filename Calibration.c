//#include "ADCh.h"
#include "stc15f2k60s2.h"
#include <stdio.h>
#include <stdlib.h>
#include "ADCh.h"
void Wait_ms(int ms);
unsigned int ADC_GetResult(unsigned char ch);
void Move(float  distance, bit direction);
unsigned int Max_Value(unsigned int *input);
void Update_position(unsigned char mnths,unsigned char dys,
										 unsigned char hurs,unsigned char mns,unsigned char sconds,
										 float  *currnt_pos, float offset_calib,float  *currnt_angle);
unsigned int Day_Of_Year(unsigned char months_bcd,unsigned char days_bcd); // this function is used to count the date in a year example: 22 March is the 80th day of the year

// read adc ch to sense the voltage of the solar panel.			

int voltage_is_stable(void)
{
	unsigned int sample_voltage[10];
	int i,ch=0;
	for (i=0; i<10;i++)
	{
				sample_voltage[i]=ADC_GetResult(ch);
	}
	
	for(i=1;i<=9;i++)
	{
				if(abs(sample_voltage[i]-sample_voltage[0])>ADC_Stable_Threshold)
					return 0;
	}
	return 1;
}
void  Find_Real_Max(float  *current_position)
{
	unsigned char ch=0;
	float calib_step_move=0.5;
	unsigned int voltage_at_scanned_pos[40],max_location;
	int i;
	// move/scan +direction
	for(i=1;i<=20;i++)
	{
		Move(calib_step_move,1);
		*current_position=*current_position+0.5;
		voltage_at_scanned_pos[20+i]=ADC_GetResult(ch);
		Wait_ms(500);
	}
	// go back to JP max theorical position
		Move(10,0);
		*current_position=*current_position-10;	
		Wait_ms(500);
	// move/scan -direction
	for (i=1;i<=20;i++)
	{

			Move(calib_step_move,0);
			*current_position=*current_position-0.5;
			voltage_at_scanned_pos[i]=ADC_GetResult(ch);
			Wait_ms(500);
	}
	
	max_location=Max_Value(voltage_at_scanned_pos);//max_location in an array [0,...,39]
	// move to the optimal position in the area of +/-10mm from JP max theorical pos
	Move(calib_step_move*max_location,1);
	*current_position=*current_position+calib_step_move*max_location;
	Wait_ms(500);

}


unsigned int Max_Value(unsigned int *input)
{
	unsigned int max,max_location=0,i;
	//int i;
	for(i=0;i<=39;i++)
	{
		max=max>*(input+i)? max:*(input+i);
		if (max==*(input+i))
			max_location=i;
	}
	return max_location;// pos in the array
	
}
//input currnt_pos is the JP max theorical position
float calibration(unsigned char mnths,unsigned char dys,
										 unsigned char hurs,unsigned char mns,unsigned char sconds,
										 float  *currnt_pos)
{
	float calib_value=0;
	float JP_max_pos=*currnt_pos;
	// if voltage is stable 
	if(voltage_is_stable())
	{
		// 	move to JP theorical max position
		Update_position(mnths,dys,hurs,mns,sconds,&JP_max_pos,0,0);// off set is Zero means we go to the JP max theorical position
		// 	find the real max value in the area of JP +/- 10mm
		Find_Real_Max(&JP_max_pos); //find real max and move to real max position
		calib_value=*currnt_pos-JP_max_pos;
		*currnt_pos=JP_max_pos;
		return calib_value;
	}
	else
		return 0;
}