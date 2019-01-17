
#include "stc15f2k60s2.h"
//The PCF85063BTL contains 18 8-bit registers with an auto-incrementing register address, 
 sbit 			PCF85063BTL_CS	=	P2^0;	   
 sbit 			PCF85063BTL_SCK	=	P2^2;	   
 sbit 			PCF85063BTL_SDI	=	P2^1;	   
 sbit 			PCF85063BTL_SDO	=	P2^1;	   
 
 #define		PCF85063BTL_CS_Clr()			{PCF85063BTL_CS=0;} // Clear CS signal
 #define		PCF85063BTL_CS_Set()			{PCF85063BTL_CS=1;} // Set CS signal

 #define		PCF85063BTL_SCK_Clr()		{PCF85063BTL_SCK=0;} 
 #define		PCF85063BTL_SCK_Set()		{PCF85063BTL_SCK=1;}

 #define		PCF85063BTL_SDI_Clr()		{PCF85063BTL_SDI=0;} 
 #define		PCF85063BTL_SDI_Set()		{PCF85063BTL_SDI=1;}

 #define		PCF85063BTL_SDO_Clr()		{PCF85063BTL_SDO=0;} 
 #define		PCF85063BTL_SDO_Set()		{PCF85063BTL_SDO=1;}
 
//Page 5/58 in the RTC's datasheet https://www.mouser.com/datasheet/2/302/PCF85063BTL-1127645.pdf


#define		Control_1	0x00
#define		Control_2	0x01
#define		Offset		0x02
#define		RAM_Byte	0x03
#define		Seconds		0x04
#define		Minutes		0x05
#define		Hours			0x06
#define		Days			0x07
#define		Weekdays	0x08
#define		Months		0x09
#define		Years			0x0A
#define		Second_Alarms			0x0B
#define		Minute_Alarms			0x0C
#define		Hour_Alarms				0x0D
#define		Day_Alarms				0x0E
#define		Week_Alarms				0x0F
#define		Timer_value				0x10
#define		Timer_mode				0x11

#define		Read_Mode					0xA0
#define		Write_Mode				0x20

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
// FOR SPI INTERFACE
///////////////////////////////////////////////////////////

// Data for SPI control SPCTL register
#define SSIG 		0x80
#define SPIEN		0x40
#define DORD 		0x20
#define MSTR		0x10
#define CPOL		0x08
#define CPHA		0x04
#define SPR1 		0x02
#define SPR2 		0x01

// SPI register
/*sfr SPSTAT=0xcd;
sfr SPCTL=0xce;
sfr SPDAT=0xcf;*/
sbit SPISS=P1^2;

// SPI Status register possible value

#define SPIF 0x80 // SPI transfer completion flag.
#define WCOL 0x40 // SPI write collision flag.



//char BCDtoDec(char bcd);






























