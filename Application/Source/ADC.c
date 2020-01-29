
#include "ADCh.h"
#include "stc15f2k60s2.h"
#include <intrins.h>
void Wait_ms(int ms);

void ADC_Init(void)
{
	// P1.0 is input
	P24=0;
	P25=0;
	P26=0;
	P1M1 |=( 1<<0);  
	P1M0 &=~( 1<<0); 
	
		P1M1 |=( 1<<2);  
	P1M0 &=~( 1<<2); 
	CLK_DIV=0x00;//set ADRJ=0
	P1ASF |=P1_0_ADC_FUNCTION;//|P1_2_ADC_FUNCTION;  

	ADC_RES=0;
	ADC_RESL=0;
	ADC_CONTR=ADC_PWR | ADC_SPEEDLL;				
	//ADC_CONTR=ADC_PWR;	//enable ADC power	
	//Wait_ms(2);
	
}


unsigned int ADC_GetResult(unsigned char ch)
{
	unsigned int ADC_Value_H,ADC_Value_L,ADC_Value;// 16-bit
	ADC_CONTR =ADC_PWR | ADC_SPEEDLL | ch | ADC_START;	
  _nop_();                        
  _nop_();
  //_nop_();
  //_nop_();
  while (!(ADC_CONTR & ADC_FLAG));
  ADC_CONTR &= ~ADC_FLAG;         
	ADC_Value_H=ADC_RES;
	ADC_Value_L=ADC_RESL;

	ADC_Value=(ADC_Value_H<<2)|(ADC_Value_L&0x03);//RES[7:0], RESL[1:0]
  return ADC_Value;               	  
}


