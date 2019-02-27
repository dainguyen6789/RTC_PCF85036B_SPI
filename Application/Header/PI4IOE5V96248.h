#include "stc15f2k60s2.h"

#define PI4IOE5V96248_Read_Addr 0x41
#define PI4IOE5V96248_Write_Addr 0x40

sbit 			I2C_SDA	=	P4^7;
sbit 			I2C_SCK	=	P5^4;	   
//#define I2C_SDA P47
//#define I2C_SCL P54
#define		I2C_SCK_Clr()		{I2C_SCK=0;} 
#define		I2C_SCK_Set()		{I2C_SCK=1;}

#define		I2C_SDA_Clr()		{I2C_SDA=0;} 
#define		I2C_SDA_Set()		{I2C_SDA=1;}
struct DATA_FOR_IO_6PORTS
{
	unsigned char port0;
	unsigned char port1;
	unsigned char port2;
	unsigned char port3;
	unsigned char port4;
	unsigned char port5;
	

};
void Write_PI4IOE5V96248(struct DATA_FOR_IO_6PORTS *xdat);

