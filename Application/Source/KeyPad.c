#include "stc15f2k60s2.h"
#include "KeyPad.h"
#include "PCF85063BTL.h"
#include "LCD_Driver_SPLC780D.h"
//#include "Receiver_Position_Data.h"

void DisplayLCD(unsigned char BCD);
void WriteData(unsigned char dat);

void SPI_WriteTime(unsigned char val,unsigned char addr);
void Command(unsigned char dat);
void LCD_clear(void);
bit move=0;
bit small_move=0;
bit direction=0;
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
	KEYPORT &=(~((1<<Column1)|(1<<Column2)|(1<<Column3)|(1<<Column4))); // ~(00000010 | 00000100 | 00001000)=1111*000*1: set three columns equal to Zero
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
			KEYPORT &=(~(1<<Column1)); //0 1 1 1
			KEYPORT |= (1<<Column2)|(1<<Column3)|(1<<Column4);//
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
						KeyValue=KEY_Star;
					
					}break;					
				}
			}		
			//COL1 COL2 COL3
			//1 		0 		1
			KEYPORT &=(~(1<<Column2)); 
			KEYPORT |= (1<<Column1)|(1<<Column3)|(1<<Column4);
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
			KEYPORT |= (1<<Column1)|(1<<Column2)|(1<<Column4);
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
						KeyValue=KEY_SHARP;
					
					}break;					
				}
			}	

			//COL1 COL2 COL3 COL4
			//1 		1 		1		0
			KEYPORT &=(~(1<<Column4)); //1 1 1 0
			KEYPORT |= (1<<Column1)|(1<<Column2)|(1<<Column3);
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
						KeyValue=KEY_A;
					
					}break;
					case ~(1<<Line2):			//S11°
					{
						KeyValue=KEY_B;
					
					}break;
					case ~(1<<Line3):			//S11°
					{
						KeyValue=KEY_C;
					
					}break;		
					case ~(1<<Line4):			//
					{
						KeyValue=KEY_D;
					
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
void Key_Process(void)//
{
	static int KeyCount=0;
	static unsigned char KeyNum_Old,KeyNum,PressedKey[5]="hhmms";
	int d,hours,mins,months,days;
	KeyNum_Old=KeyNum;
	KeyNum=Key_Scan();
	//if( (KeyNum=Key_Scan())!=0 )  	//
	if (KeyNum_Old==Unpress && KeyNum==KEY_C)
	{
		if(direction)
			direction=0;
		else 
			direction=1;
		return;
	}
	if (KeyNum==KEY_D && !move)
	{
		move =1;
		return;
	}	
	if (KeyNum==KEY_D && move)
	{
		move =0;
		return;
	}
	if (KeyNum==KEY_A && auto_mode)
	{
		auto_mode=0;
		return;
	}	
	if (KeyNum==KEY_A && !auto_mode)
	{
		auto_mode=1;
		return;
	}		
	
	if (KeyNum==KEY_B)
	{
		small_move =1;
		return;
	}	
	
	if(KeyNum_Old==Unpress && KeyNum!=Unpress)
	{
		PressedKey[KeyCount]=KeyNum;
		KeyCount++;
		if(KeyCount==5)
		{
			//PressedKey[]="";
			KeyCount=0;
			//=========================================================================			
			// Set Hour,Minute
			//=========================================================================
			hours=PressedKey[0]*10+PressedKey[1];
			mins=PressedKey[2]*10+PressedKey[3];
			if(PressedKey[4]==KEY_SHARP && hours<=24 && mins<=59)// set hour, minute
			{	
				SPI_WriteTime((PressedKey[0]<<4)|PressedKey[1],Hours);//Write BCD value
				SPI_WriteTime((PressedKey[2]<<4)|PressedKey[3],Minutes);//Write BCD value
			}
			//=========================================================================			
			// Set Month,Day			
			//=========================================================================
			months=PressedKey[0]*10+PressedKey[1];
			days=PressedKey[2]*10+PressedKey[3];			
			if (PressedKey[4]==KEY_Star && months<=12 )//Set month and day
			{
				if(months<=7)//1..7
				{
					if(months%2)//month has 31 days,1 3 5 7 
					{
						if(days<=31)
						{
							SPI_WriteTime((PressedKey[0]<<4)|PressedKey[1],Months);//Write BCD value
							SPI_WriteTime((PressedKey[2]<<4)|PressedKey[3],Days);//Write BCD value							
						}
					}
					else //2 4 6 
					{
						if (months==2)//February
						{
							if(days<=28)
							{
								SPI_WriteTime((PressedKey[0]<<4)|PressedKey[1],Months);//Write BCD value
								SPI_WriteTime((PressedKey[2]<<4)|PressedKey[3],Days);//Write BCD value							
							}							
							
						}
						else //4 6 
						{
							if(days<=30)
							{
								SPI_WriteTime((PressedKey[0]<<4)|PressedKey[1],Months);//Write BCD value
								SPI_WriteTime((PressedKey[2]<<4)|PressedKey[3],Days);//Write BCD value							
							}									
							
						}
								
					}
				}
				else	//8..12
				{
					if(!(months%2))//month has 31 days,8 10 12
					{
						if(days<=31)
						{
							SPI_WriteTime((PressedKey[0]<<4)|PressedKey[1],Months);//Write BCD value
							SPI_WriteTime((PressedKey[2]<<4)|PressedKey[3],Days);//Write BCD value							
						}
					}
					else //9 11
					{
							if(days<=30)
							{
								SPI_WriteTime((PressedKey[0]<<4)|PressedKey[1],Months);//Write BCD value
								SPI_WriteTime((PressedKey[2]<<4)|PressedKey[3],Days);//Write BCD value							
							}									
					}
				}
			}		
			LCD_clear();
			Command(0x08);
			Command(0x00);			
			WriteData(0x68);//display "h"
			WriteData(0x68);//display "h"
			WriteData(0x6D);//display "m"
			WriteData(0x6D);//display "m"
			WriteData(0x23);//display "#" SETTIME_KEY			
		}
		//LCD Display the pressed buttons
	for(d=0;d<KeyCount;d++)
	{
		if(KeyCount<=4)
		{
			Command(0x08);
			Command(0x00+d);
			WriteData(PressedKey[d]|0x30);
		}
	}	

	}
	

	
}
/***********************************************
************************************************/
void KeyPad_IO_Init(void)
{	
	P0M1 &=~( (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7) );  
	P0M0 &=~( (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7) );    
}







