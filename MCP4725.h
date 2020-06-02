#include "stc15f2k60s2.h"
//
#define  	MCP4725_ADDR 								0xC0
//============================================================
//(A) Write DAC Register: (C2, C1, C0) = (0,1,0) or
//(B) Write DAC Register and EEPROM: (C2, C1, C0) = (0,1,1)
//============================================================
// 2nd byte C2 C1 C0 X X PD1 PD0 X DS page 25/50. 
// TABLE 5-2: POWER-DOWN BITS
//PD1 PD0 Function
//0 0 Normal Mode
#define  	Write_DAC_Register 								0x40 


#define  	READ_Command											0x01


sbit 			I2C_SDA	=	P1^2;
sbit 			I2C_SCK	=	P1^3;	   
//#define I2C_SDA P47
//#define I2C_SCL P54
#define		I2C_SCK_Clr()		{I2C_SCK=0;} 
#define		I2C_SCK_Set()		{I2C_SCK=1;}

#define		I2C_SDA_Clr()		{I2C_SDA=0;} 
#define		I2C_SDA_Set()		{I2C_SDA=1;}




void Write_MCP4725(int dat);