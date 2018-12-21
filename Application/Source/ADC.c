
#include "ADCh.h"
#include "stc15f2k60s2.h"
 #include <intrins.h>
 void Wait_ms(int ms);

void ADC_Init(void)
{
	// P1.0 is input
	P1M1 |=( 1<<0);  
	P1M0 &=~( 1<<0); 
	CLK_DIV=0x00;
	P1ASF |=P1_0_ADC_FUNCTION;  
		
	ADC_RES=0;	  		
	ADC_CONTR=ADC_PWR | ADC_SPEEDLL;				
	//ADC_CONTR=ADC_PWR;	//enable ADC power	
	Wait_ms(2);
	
}


unsigned int ADC_GetResult(unsigned char ch)
{
	unsigned int ADC_Value;
	ADC_CONTR =ADC_PWR | ADC_SPEEDLL | ch | ADC_START;	
  _nop_();                        
  _nop_();
  _nop_();
  _nop_();
  while (!(ADC_CONTR & ADC_FLAG));
  ADC_CONTR &= ~ADC_FLAG;         
	ADC_Value=ADC_RES;							
	ADC_Value=(ADC_Value<<2)|ADC_RESL;
  return ADC_Value;               	  
}