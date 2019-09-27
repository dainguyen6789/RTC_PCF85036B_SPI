#include "SI1120.h"
#include "LCD_Driver_SPLC780D.h"

 void SI1120_CONFIG_MODE_VIRH(void)
 {
	 	SI1120_SC=1;
	 	SI1120_MD=0;
	 	SI1120_STX=0;
	 
		Wait_ms(20);
		SI1120_SC=1;
		//latch
	 	SI1120_MD=1;
	 	SI1120_STX=1;
	 
		Wait_ms(20);
	 
		SI1120_SC=0;

	 	Wait_ms(20);
	 //Static
	 
	 	SI1120_MD=0;

		SI1120_STX=0;
	 
		Wait_ms(20);
	 
		SI1120_STX=1;
	 
}

void Enable_EXTINT0(void)
{
	P3M0&=~(1<<2);//0
	P3M1|=(1<<2);//1
	IT0=0;	//Both rising and falling edge of INT0
	EX0=1; 	// enable EXT INT0
	EA=1; 	//each interrupt source would be individually enabled or disabled by setting or clearing its enable bit.
}


