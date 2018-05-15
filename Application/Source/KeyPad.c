#include "stc15f2k60s2.h"
#include "KeyPad.h"
#include "PCF85063BTL.h"
int KeyCount=0;
void SPI_WriteTime(unsigned char val,unsigned char addr);

void Delay_ms(unsigned int ms)
{
  unsigned int De_Cnt;
  while( (ms--) != 0)
  {
    for(De_Cnt = 0; De_Cnt < 600; De_Cnt++); 
  }             
}
unsigned char Key_Scan(void)
{
	unsigned char KeyTemp1,KeyTemp2;
	unsigned char KeyValue;

	//
	KEYPORT &=(~((1<<Column1)|(1<<Column2)|(1<<Column3))); // ~(00000010 | 00000100 | 00001000)=1111*000*1: set three columns equal to Zero
	//
	//0010 0000
	KEYPORT |= (1<<Line1) | (1<<Line2) | (1<<Line3) | (1<<Line4) ;  //10000000 | 01000000 | 00100000=*111*00000: Set three columns equal to 1
	//		
	KeyTemp1=KEYPORT | (~((1<<Line1)|(1<<Line2)|(1<<Line3)|(1<<Line4)));	//~(10000000 | 01000000 | 00100000)=00011111
	if(KeyTemp1!=0xff)	 		//
	{	
		Delay_ms(10);					//
		KeyTemp1=KEYPORT | ( ~((1<<Line1)|(1<<Line2)|(1<<Line3)|(1<<Line4)) );	//~(10000000 | 01000000 | 00100000)=00011111
		if(KeyTemp1!=0xff)	 		//
		{
			//COL1 COL2 COL3
			//0 		1 		1
			KEYPORT &=(~(1<<Column1)); //0 1 1
			KEYPORT |= (1<<Column2)|(1<<Column3);//
			KeyTemp1=KEYPORT | (~((1<<Line1)|(1<<Line2)|(1<<Line3)|(1<<Line4)));//	KEYPORT | 00011111
			if(KeyTemp1!=0xff)	 		//
			{
				while(KeyTemp1!=0xff)	// if pressed any key on COL1
				{	
					KeyTemp2=KeyTemp1;	
					KeyTemp1=KEYPORT | (~((1<<Line1)|(1<<Line2)|(1<<Line3)|(1<<Line4)));	// read the data, KEYPORT is the instantaneous value of the PORT.
				}
				// run when unpress
				switch(KeyTemp2)
				{
					case ~(1<<Line1):			//S6°
					{
						KeyValue=KEY1;
					
					}break;
					case ~(1<<Line2):			//S9°
					{
						KeyValue=KEY4;
					
					}break;
					case ~(1<<Line3):			//S9°
					{
						KeyValue=KEY7;
					
					}break;	
					case ~(1<<Line4):			//
					{
						KeyValue=KEY0;
					
					}break;					
				}
			}		
			//COL1 COL2 COL3
			//1 		0 		1
			KEYPORT &=(~(1<<Column2)); 
			KEYPORT |= (1<<Column1)|(1<<Column3);
			KeyTemp1=KEYPORT | (~((1<<Line1)|(1<<Line2)|(1<<Line3)|(1<<Line4)));			//
			if(KeyTemp1!=0xff)	 		//
			{
				while(KeyTemp1!=0xff)	//
				{	
					KeyTemp2=KeyTemp1;	
					KeyTemp1=KEYPORT | (~((1<<Line1)|(1<<Line2)|(1<<Line3)|(1<<Line4))); //
				}
				switch(KeyTemp2)
				{
					case ~(1<<Line1):			//
					{
						KeyValue=KEY2;
					
					}break;
					case ~(1<<Line2):			//
					{
						KeyValue=KEY5;
					
					}break;
					case ~(1<<Line3):			//
					{
						KeyValue=KEY8;
					
					}break;		
					case ~(1<<Line4):			//
					{
						KeyValue=KEY0;
					
					}break;						
				}
			}
			//COL1 COL2 COL3
			//1 		1 		0
			KEYPORT &=(~(1<<Column3)); //1 1 0
			KEYPORT |= (1<<Column1)|(1<<Column2);
			KeyTemp1=KEYPORT | (~((1<<Line1)|(1<<Line2)|(1<<Line3)|(1<<Line4)));		 //
			if(KeyTemp1!=0xff)	 		//
			{
				while(KeyTemp1!=0xff)	// if pressed any key of COL3
				{	
					KeyTemp2=KeyTemp1;	
					KeyTemp1=KEYPORT | (~((1<<Line1)|(1<<Line2)|(1<<Line3)|(1<<Line4)));//
				}
				switch(KeyTemp2)
				{
					case ~(1<<Line1):			//S8°
					{
						KeyValue=KEY3;
					
					}break;
					case ~(1<<Line2):			//S11°
					{
						KeyValue=KEY6;
					
					}break;
					case ~(1<<Line3):			//S11°
					{
						KeyValue=KEY9;
					
					}break;		
					case ~(1<<Line4):			//
					{
						KeyValue=KEY0;
					
					}break;					
				}
			}	
			return 	KeyValue;	
		}							
		else
		{
			return Unpress;				
		}
	}
	else
	{
		return Unpress;	
	}			
}
void Key_Process(void)
{
	unsigned char KeyNum_Old,KeyNum,PressedKey[4];
	KeyNum_Old=KeyNum;
	KeyNum=Key_Scan();
	//if( (KeyNum=Key_Scan())!=0 )  	//¼ì²âÊÇ·ñÓÐ¼ü°´ÏÂ
	if(KeyNum_Old==Unpress && KeyNum!=Unpress)
	{
		PressedKey[KeyCount]=KeyNum;
		KeyCount++;
		if(KeyCount==4)
		{
			KeyCount=0;
			SPI_WriteTime(PressedKey[0]<<4|PressedKey[1],Hours);
			SPI_WriteTime(PressedKey[2]<<4|PressedKey[3],Minutes);
		}
		
	}
	
}
/***********************************************
************************************************/
void KeyPad_O_Init(void)
{	
	P0M1 &=~( (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7) );  
	P0M0 &=~( (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7) );    
   
}







