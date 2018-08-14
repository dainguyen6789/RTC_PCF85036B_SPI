#include "LCD_Driver_SPLC780D.h"
#include "stc15f2k60s2.h"


void Wait_ms(int ms);
void Command(unsigned char dat);
void WriteData(unsigned char dat);

void LCD_Init(void)
{
	P1M1 =0x00;  
	P1M0 =0x00; 

	Wait_ms(150);
	Command(0x03);
	
	Wait_ms(30);
	Command(0x03);
	
	Wait_ms(10);
	Command(0x03);
	
	Wait_ms(10);
	Command(0x02);
	//Command(0x08);	
	Wait_ms(1);
	
	//Wait_ms(50);
	Command(0x02);	
	Command(0x08);
	
	Command(0x00);
	Command(0x0E);
	
	Command(0x00);	
	Command(0x06);
	Wait_ms(20);
	
}
void WriteData(unsigned char dat)
{	

	/*LCD_E=1;
	Wait_ms(10);
	LCD_E=0;*/
	LCD_RS=1;
	LCD_RW=0;
	// 4 high bits
	LCD_D7=(dat&(1<<7)) >>7;
	LCD_D6=(dat&(1<<6)) >>6;
	LCD_D5=(dat&(1<<5)) >>5;
	LCD_D4=(dat&(1<<4)) >>4;
	LCD_E=1;
	Wait_ms(1);
	LCD_E=0;
	// 4 low bits
	LCD_D7=(dat&(1<<3)) >>3;
	LCD_D6=(dat&(1<<2)) >>2;
	LCD_D5=(dat&(1<<1)) >>1;
	LCD_D4=dat&1;	
	//LCD_RS=0;
	LCD_E=1;
	Wait_ms(10);
	LCD_E=0;	
	LCD_RS=0;
		
}

void Command(unsigned char dat)//dat=0x0X
{	
	LCD_RS=0;
	LCD_RW=0;
	//we only consider 4 low bits
	if((dat&(1<<3)) >>3)
		LCD_D7=1;
	else
		LCD_D7=0;
	if((dat&(1<<2)) >>2)
		LCD_D6=1;
	else
		LCD_D6=0;	
	if((dat&(1<<1)) >>1)
		LCD_D5=1;
	else
		LCD_D5=0;	
	if(dat&0x01)
		LCD_D4=1;
	else
		LCD_D4=0;	
	LCD_E=1;
	Wait_ms(10);
	LCD_E=0;	
}

void Wait_ms(int ms)
{
  unsigned int De_Cnt;
  while( (ms--) != 0)
  {
    for(De_Cnt = 0; De_Cnt < 600; De_Cnt++); 
  }  	
}
 void DisplayLCD(unsigned char BCD)
{
	unsigned char first_digit, second_digit;
	first_digit=BCD>>4 | 0x30;
	second_digit=BCD&0x0F | 0x30;
	//write 1st letter 
	WriteData(first_digit);
	// write second letter
	WriteData(second_digit);

}

void LCD_return_home(void)
{
	Command(0x00);
	Command(0x02);
	
}
void LCD_clear(void)
{
	Command(0x00);
	Command(0x01);
	
}

void Display_Line(int line)
{
	if (line==1)
	{	
		Command(0x08);
		Command(0x00);
	}
	else
	{
		Command(0x0C);
		Command(0x00);
	}
}


