
 #include "stc15f2k60s2.h"
 #include "a25lc040.h"
 #include <stdio.h>
#include <stdlib.h>
 //#include "hc595.h"
 //#include "PCF85063BTL.h"

 	 

void Delay_ms(unsigned int ms);
void SendString(char *s);
void SendUART1(unsigned char dat);
void initUART1(void);
unsigned char BCDtoDec(unsigned char BCD);
unsigned char ten(unsigned char BCD);
unsigned char unit(unsigned char BCD);


void main(void)
{
	unsigned char seconds;
	int sec_decimal;
	char numStr[5];
	SPI_Init();
	initUART1();
	EA=1; 			// each interrupt source will be enable or disable by setting its interrupt bit	   

	while(1)
	{
		seconds=SPI_ReadTime();
		//sprintf(numStr, "%d", BCDtoDec(seconds&0x7f));
		//puts(numStr);		
		//SendString(numStr);		
		P0=seconds;
		SendUART1(ten(seconds&0x7f)+48);
		SendUART1(unit(seconds&0x7f)+48);
		/*sec_decimal=BCDtoDec(seconds&0x7f);
		sprintf(numStr, "%d", sec_decimal);
		puts(numStr);		
		SendString(numStr);*/
		//SendUART1(BCDtoDec(seconds&0x7f));
		Delay_ms(1000);
	}
	
} 

void Delay_ms(unsigned int ms)
{
  unsigned int De_Cnt;
  while( (ms--) != 0)
  {
    for(De_Cnt = 0; De_Cnt < 600; De_Cnt++); 
  }             
}
