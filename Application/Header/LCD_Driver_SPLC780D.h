//reference page 7 http://www.newhavendisplay.com/app_notes/SPLC780D.pdf
#include "stc15f2k60s2.h"

#define Clear_Display 0x01s
#define Return_Home 0x02
#define Entry_Mode_Set 0x06
#define Function_Set_8bit 0x38
#define Function_Set_4bit	0x18
 sbit 			LCD_E	=	P3^3;	 
 sbit 			LCD_RS	=	P4^0;	   
 sbit 			LCD_RW	=	P3^2;	   
 sbit 			LCD_D4	=	P3^4;	   
 sbit 			LCD_D5	=	P3^5;	
 sbit 			LCD_D6	=	P3^6;	
 sbit 			LCD_D7	=	P3^7;	 
 void Wait_ms(int ms);
 void LCD_Init(void);
 void DisplayLCD(unsigned char BCD);
 void LCD_return_home(void);
 void WriteData(unsigned char dat);
 void Display_Line(int line);
 void Display_Pos(float sign_dat);
 void Command(unsigned char dat);//LCD command
int abs(int dat);