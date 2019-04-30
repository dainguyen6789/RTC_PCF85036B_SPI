	//#define  
	#include "stc15f2k60s2.h"

sbit 			SI1120_SC	=	P1^3;	
sbit 			SI1120_MD	=	P1^4;	
sbit 			SI1120_STX =	P1^5;	
sbit 			SI1120_PRX	=	P3^2;	//INT0
	

void SI1120_CONFIG_MODE_VIRH(void);
void Enable_EXTINT0(void);


 
