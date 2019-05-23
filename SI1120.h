	//#define  
	#include "stc15f2k60s2.h"

sbit 			SI1120_SC	=	P1^6;	
sbit 			SI1120_MD	=	P1^4;	
sbit 			SI1120_STX =	P1^5;	
sbit 			SI1120_PRX	=	P3^2;	//pin INT0, trigger on both rising and falling edge
// declaration pwm time as an external variable
extern float pwm_time,pwm_time_min,pwm_time_max;

void SI1120_CONFIG_MODE_VIRH(void);
void Enable_EXTINT0(void);


 
