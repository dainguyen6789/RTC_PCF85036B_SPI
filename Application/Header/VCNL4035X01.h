#include "stc15f2k60s2.h"
sbit 			I2C_SDA	=	P3^1;	   
sbit 			I2C_SCK	=	P3^2;	   
 

#define		I2C_SCK_Clr()		{I2C_SCK=0;} 
#define		I2C_SCK_Set()		{I2C_SCK=1;}

#define		I2C_SDA_Clr()		{I2C_SDA=0;} 
#define		I2C_SDA_Set()		{I2C_SDA=1;}
 

#define VCNL4035_addr 0x60<<1

#define ALS_CONF1 0x00
#define ALS_CONF2 0x00

#define ALS_THDH_L 0x01
#define ALS_THDH_M 0x01

#define ALS_THDL_L 0x02
#define ALS_THDL_M 0x02

#define PS_CONF1  0x03
#define PS_CONF2  0x03

#define PS_CONF3  0x04
#define PS_MS	   0x04

#define PS_CANC_L 0x05
#define PS_CANC_M 0x05

#define PS_THDL_L 0x06
#define PS_THDL_M 0x06

#define PS_THDH_L 0x07
#define PS_THDH_M 0x07

#define PS1_Data_L 0x08
#define PS1_Data_M 0x08

#define PS2_Data_L 0x09
#define PS2_Data_M 0x09

#define PS3_Data_L 0x0A
#define PS3_Data_M 0x0A

#define ALS_Data_L 0x0B
#define ALS_Data_M 0x0B

#define White_Data_L 0x0C
#define White_Data_M 0x0C

#define Reserved 		0x0D
#define INT_Flag 0x0D

#define ID_L 				0x0E
#define ID_M				 	0x0D

#define I2C_READ			0x01
#define I2C_WRITE		0x00









