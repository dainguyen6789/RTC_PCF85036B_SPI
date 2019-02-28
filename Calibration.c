//#include "ADCh.h"
#include "stc15f2k60s2.h"
#include <stdio.h>
#include <stdlib.h>
#include "ADCh.h"
#include "AT25SF041.h"
void Wait_ms(int ms);
unsigned int ADC_GetResult(unsigned char ch);
void Move(float  distance, bit direction);
unsigned int Max_Value(unsigned int *input);
void Update_position(unsigned char mnths,unsigned char dys,
										 unsigned char hurs,unsigned char mns,unsigned char sconds,
										 float  *currnt_pos, float offset_calib);
unsigned int Day_Of_Year(unsigned char months_bcd,unsigned char days_bcd); // this function is used to count the date in a year example: 22 March is the 80th day of the year
void WriteData(unsigned char dat);
void Command(unsigned char dat);//dat=0x0X
void Display_Pos(float sign_dat);
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
void  Find_Real_Max(float  *current_position, unsigned int *calib_max_ADC_Value,unsigned int *max_ADC_JP_value, unsigned long int *address_to_write)
{
		unsigned char ch=0;
		float calib_step_move=0.5;
		unsigned int voltage_at_scanned_pos[60],max_location;
		int i;
		// move/scan +`
		for(i=0;i<60;i++)
		{
			voltage_at_scanned_pos[i]=0;
		}
		for(i=0;i<60;i++)
		{
				//if(ADC_GetResult(2)>=sunlight_ADC_Threshold)
				{
					Move(calib_step_move,1);
					*current_position=*current_position+0.5;
					voltage_at_scanned_pos[i]=ADC_GetResult(ch);
					// 	Because the uC Flash Memory is almost full, 
					//	so I did not change/increase the size of "struct data_to_store"
					// 	STORE THE CALIB VOLTAGE IN THE SPI NOR FLASH after every 1mm, total 4*30=120 byte of data for each calib time
					if(i%2==0)
					{
						AT25SF041_WriteEnable();
						//Wait_ms_SPINOR(50);	
						AT25SF041_Write(Byte_Page_Program, *address_to_write,ADC_GetResult(2)/4);	// SUNLIGHT
						Wait_ms_SPINOR(50);	
						++*address_to_write;
						
						AT25SF041_WriteEnable();
						//Wait_ms_SPINOR(50);	
						AT25SF041_Write(Byte_Page_Program, *address_to_write,voltage_at_scanned_pos[i]/4);// SOLAR CELL ADC VOLTAGE	
						Wait_ms_SPINOR(50);	
						++*address_to_write;
						
						AT25SF041_WriteEnable();
						//Wait_ms_SPINOR(50);	
						AT25SF041_Write(Byte_Page_Program, *address_to_write,*current_position);	// SOLAR CELL Instant Position when calib
						Wait_ms_SPINOR(50);	
						++*address_to_write;	
						
						AT25SF041_WriteEnable();
						//Wait_ms_SPINOR(50);	
						AT25SF041_Write(Byte_Page_Program, *address_to_write,(*current_position-(char)*current_position)*100);	// SOLAR CELL Instant Position when calib
						Wait_ms_SPINOR(50);	
						++*address_to_write;
	
					}
						
				}
				/*else
				{
					// go back to JP max position
					if(i<=29)
						Move(calib_step_move*(29-(i-1)),1);//
						//*current_position=*current_position+calib_step_move*(29-i);
					else
						Move(calib_step_move*(i-1-29),0);// 
					
					*current_position=*current_position+calib_step_move*(29-(i-1));
					break;// break FOR LOOP
				}*/
				
				//move cursor to line 1, pos 6
				//Write_PI4IOE5V96248(&dat);
				Command(0x08);
				Command(0x05);
				
				WriteData(0x41);//display "A"
				WriteData(0x44);//display "D"
				WriteData(0x43);//display "C"	
				WriteData(0x3A);//display ":"	
				//LCD_clear();
				//real_voltage=(float)voltage_at_scanned_pos[i]/1024*5;
				Display_Pos((float)voltage_at_scanned_pos[i]/1024*5);
				WriteData(0x56);//display "V"	
				WriteData(0x10);//display " "	
				Wait_ms(1500);
		}

		if(i==60)// make sure that all of the calibration value are scanned with GOOD SUN
		{
			max_location=Max_Value(voltage_at_scanned_pos);//max_location in an array [0,...,39]
			*calib_max_ADC_Value=voltage_at_scanned_pos[max_location];

			*max_ADC_JP_value=voltage_at_scanned_pos[29];
		
			// move to the optimal position in the area of +/-10mm from JP max theorical pos
			Move(calib_step_move*(59-max_location),0);
			*current_position=*current_position-calib_step_move*(59-max_location);
			Wait_ms(500);
		}

}


unsigned int Max_Value(unsigned int *input)
{
	unsigned int max=0,max_location=29,i;
	//int i;
	for(i=0;i<=59;i++)
	{
		max=max>*(input+i)? max:*(input+i);
		if (max==*(input+i)&& max!=0)
			max_location=i;
	}
	return max_location;// pos in the array
	
}
//input currnt_pos is the JP max theorical position
float calibration(unsigned char mnths,unsigned char dys,
										 unsigned char hurs,unsigned char mns,unsigned char sconds,
										 float  *currnt_pos,unsigned int *calib_max_ADC_Val,float *theorical_max_pos,unsigned int *max_ADC_JP_value,unsigned long int *NOR_address_to_write)
{
	float calib_value=0;
	unsigned int temporary_calib_max_ADC=0,temp_max_ADC_JP_value=0;//
	unsigned long int temp_NOR_address_to_write=*NOR_address_to_write;
	float JP_max_pos=*currnt_pos;
	*calib_max_ADC_Val=0;
	*max_ADC_JP_value=0;
	*theorical_max_pos=0;
	// if voltage is stable 
	if(voltage_is_stable())
	{
		// 	move to JP theorical max position-15
		Update_position(mnths,dys,hurs,mns,sconds,&JP_max_pos,-15);// off set is Zero means we go to  (first, go to JP max theorical position -5)
		*theorical_max_pos=JP_max_pos+15;// compensate +15 because of previous line.
		// 	find the real max value in the area of JP +/- 15mm
		Find_Real_Max(&JP_max_pos,&temporary_calib_max_ADC,&temp_max_ADC_JP_value,&temp_NOR_address_to_write); //find real max and move to real max position
		
		*NOR_address_to_write=temp_NOR_address_to_write;
		
		*calib_max_ADC_Val=temporary_calib_max_ADC;
		*max_ADC_JP_value=temp_max_ADC_JP_value;
		calib_value=JP_max_pos-*theorical_max_pos;
		*currnt_pos=JP_max_pos;
		return calib_value;
	}
	else
		return 0;
}