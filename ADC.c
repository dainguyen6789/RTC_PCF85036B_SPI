#include "ADCh.h"
#include "stc15f2k60s2.h"
#include "LCD_Driver_SPLC780D.h"

void adc_isr() interrupt 5 using 1
{
	
	ADC_CONTR&=!ADC_Flag;
	adc_data=ADC_RES;// 8 high bit
	adc_data=(adc_data<<2)|ADC_RESL;//ADC 10 bit
}

void initADC()
{
	P1ASF=P1ASF|(1<<0);// P1.0 is ADC port
	ADC_RES=0;
	ADC_RESL=0;
	ADC_CONTR=ADC_PWR | ADC_START;
	Wait_ms(2);
}