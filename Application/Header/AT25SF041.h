#include "stc15f2k60s2.h"
//
#define  	Byte_Page_Program 								0x02
#define  	Read_Array 												0x03
#define  	Write_Enable 											0x06
#define  	Chip_Erase 												0x60
#define  	Read_Stat_Register_Byte1 					0x05

//The PCF85063BTL contains 18 8-bit registers with an auto-incrementing register address, 
 sbit 			AT25SF041_CS	=	P4^1;	   
 sbit 			AT25SF041_SCK	=	P4^3;	   
 sbit 			AT25SF041_SDI	=	P4^2;	   
 sbit 			AT25SF041_SDO	=	P4^4;	   
 
 #define		AT25SF041_CS_Clr()			{AT25SF041_CS=0;} // Clear CS signal
 #define		AT25SF041_CS_Set()			{AT25SF041_CS=1;} // Set CS signal

 #define		AT25SF041_SCK_Clr()		{AT25SF041_SCK=0;} 
 #define		AT25SF041_SCK_Set()		{AT25SF041_SCK=1;}

 #define		AT25SF041_SDI_Clr()		{AT25SF041_SDI=0;} 
 #define		AT25SF041_SDI_Set()		{AT25SF041_SDI=1;}

 #define		AT25SF041_SDO_Clr()		{AT25SF041_SDO=0;} 
 #define		AT25SF041_SDO_Set()		{AT25SF041_SDO=1;}
 
struct data_to_store
{
		char month;
		char date;
		char hour;
	
		char min;
		char calib_max_voltage_ADC;
		char calib_max_pos_floor;
	
		char calib_max_pos_float;
		int light_ADC;
		char Voltage_at_LUT_pos;
	
		char LUT_max_pos_floor;
		char LUT_max_pos_float;
};
 
void AT25SF041_WriteEnable(void);
void AT25SF041_Write(char opcode, unsigned long int addr,char dat);
char AT25SF041_Read(char opcode,unsigned long int addr);
void Wait_ms_SPINOR(int ms);
void AT25SF041_ChipErase(void);
char Read_Status_Register_Byte1(void);
void SPI_NOR_Write_Data(struct data_to_store dat,unsigned long int *addr);



