
#include "ADCh.h"
#include "stc15f2k60s2.h"
#include <intrins.h>
void Wait_ms(int ms);

void ADC_Init(void)
{
	// Config P1.2, P1.3, P1.4,P1.5 as inputs

	P1M1 |=(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7);  
	P1M0 &=~( (1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7)); 
	

	CLK_DIV=0x00;//set ADRJ=0
//	P1xASF
//	0 : = Keep P1.x as general-purpose I/O function.
//	1 : = Set P1.x as ADC input channel-x
	//P1ASF |=(1<<2)|(1<<3)|(1<<4)|(1<<5);//|P1_2_ADC_FUNCTION;  

	ADC_RES=0;
	ADC_RESL=0;
	ADC_CONTR=ADC_PWR | ADC_SPEEDLL;				
	//ADC_CONTR=ADC_PWR;	//enable ADC power	
	//Wait_ms(2);
	
}
unsigned int ADC_GetResult(unsigned char ch)
{
	unsigned int ADC_Value_H=0,ADC_Value_L=0,ADC_Value=0;// 16-bit
	//ADC_RES=0;
	//ADC_RESL=0;
	P1ASF =(1<<ch);
	ADC_CONTR&=~(1<<7);
  _nop_();                        
  _nop_();
	ADC_CONTR =ADC_PWR | ADC_SPEEDLL | ch | ADC_START;	
  _nop_();                        
  _nop_();
  //_nop_();
  //_nop_();
  while (!(ADC_CONTR & ADC_FLAG));
  ADC_CONTR &= ~ADC_FLAG;         
	ADC_Value_H=ADC_RES;
	ADC_Value_L=ADC_RESL;

	ADC_Value=(ADC_Value_H<<2)|(ADC_Value_L&0x03);//10-bit ADC; RES[7:0], RESL[1:0]
  return ADC_Value;               	  
}




















