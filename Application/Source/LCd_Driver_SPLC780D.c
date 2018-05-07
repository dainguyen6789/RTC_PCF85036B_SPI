#include "LCD_Driver_SPLC780D.h"
#include "stc15f2k60s2.h"


void Wait_ms(int ms);
void Command(unsigned char dat);
void WriteData(unsigned char dat);

void LCD_Init(void)
{

	Wait_ms(100);
	Command(0x03);
	
	Wait_ms(10);
	Command(0x03);
	
	Wait_ms(150);
	Command(0x03);
	
	Wait_ms(20);
	Command(0x02);
	
	Wait_ms(20);
	Command(0x02);
	
	Wait_ms(20);
	Command(0x08);
	
	Wait_ms(20);	
	Command(0x00);
	
	Wait_ms(20);
	Command(0x08);
	
	Wait_ms(20);	
	Command(0x00);
	
	Wait_ms(20);
	Command(0x01);
	
	Wait_ms(20);	
// entry mode, the display has no shift
	Command(0x00);
	
	Wait_ms(20);	
	Command(0x06);
	Wait_ms(20);
	
}
void WriteData(unsigned char dat)
{	
	LCD_RS=1;
	LCD_RW=0;
	LCD_E=1;
	Wait_ms(1);
	LCD_E=0;
	// 4 high bits
	LCD_D7=dat&(1<<7) >>7;
	LCD_D6=dat&(1<<6) >>6;
	LCD_D5=dat&(1<<5) >>5;
	LCD_D4=dat&(1<<4) >>4;
	//Wai_ms(10);
	LCD_E=1;
	Wait_ms(1);
	LCD_E=0;
	// 4 low bits
	LCD_D7=dat&(1<<3) >>3;
	LCD_D6=dat&(1<<2) >>2;
	LCD_D5=dat&(1<<1) >>1;
	LCD_D4=dat&1;		
}

void Command(unsigned char dat)//dat=0x0X
{	
	LCD_RS=0;
	LCD_RW=0;
	LCD_E=1;
	Wait_ms(1);
	LCD_E=0;
	//we only consider 4 low bits
	LCD_D7=dat&(1<<3) >>3;
	LCD_D6=dat&(1<<2) >>2;
	LCD_D5=dat&(1<<1) >>1;
	LCD_D4=dat&1;		
}

void Wait_ms(int ms)
{
  unsigned int De_Cnt;
  while( (ms--) != 0)
  {
    for(De_Cnt = 0; De_Cnt < 600; De_Cnt++); 
  }  	
}


