#include "AT25SF041.h"
//
void AT25SF041_WriteEnable(void)
{
	int i;
	AT25SF041_CS_Clr();
Wait_ms_i2c(2);
	
	for(i=0;i<8;i++)
	{
			AT25SF041_SCK_Clr();
			Wait_ms_i2c(2);
			AT25SF041_SDO=(Write_Enable<<i)&0x01;
			AT25SF041_SCK_Set();
			Wait_ms_i2c(2);

	}
	AT25SF041_CS_Set();
	Wait_ms_i2c();
	
}


void AT25SF041_Write(char opcode, unsigned long int addr,char dat)
{
	int i;
	AT25SF041_CS_Clr();
Wait_ms_i2c(2);

	//////////////////////////////////////////
	for( i=0;i<8;i++)
	{
			AT25SF041_SCK_Clr();
		Wait_ms_i2c(2);
			AT25SF041_SDO=(opcode<<i)&0x01;
			AT25SF041_SCK_Set();
		Wait_ms_i2c(2);
		
	}
	
		for( i=0;i<23;i++)
	{
			AT25SF041_SCK_Clr();
			Wait_ms_i2c(2);
			AT25SF041_SDO=(addr<<i)&0x01;
			AT25SF041_SCK_Set();
		Wait_ms_i2c(2);

	}
	
		for( i=0;i<8;i++)
	{
			AT25SF041_SCK_Clr();
			Wait_ms_i2c(2);
			AT25SF041_SDO=(dat<<i)&0x01;
			AT25SF041_SCK_Set();
			Wait_ms_i2c(2);

	}
	///////////////////////////////////////
	AT25SF041_CS_Set();
	Wait_ms_i2c(2);
	
}

char  AT25SF041_Read(char opcode,unsigned long int addr)
{
	char dat=0;
	int i;
	AT25SF041_CS_Clr();
	Wait_ms_i2c(2);

	//////////////////////////////////////////
	// Send the OPCODE
	for( i=0;i<8;i++)
	{
			AT25SF041_SCK_Clr();
			Wait_ms_i2c(2);
			AT25SF041_SDO=(opcode<<i)&0x01;
			AT25SF041_SCK_Set();
			Wait_ms_i2c(2);

	}
	// SEND THE ADDR (24-bit)
	for( i=0;i<23;i++)
	{
			AT25SF041_SCK_Clr();
			Wait_ms_i2c(2);
			AT25SF041_SDO=(addr<<i)&0x01;
			AT25SF041_SCK_Set();
			Wait_ms_i2c(2);

	}
	/////////////////////////////////////////
	// Store the output data from NOR FLash
	for( i=0;i<8;i++)
	{
			AT25SF041_SCK_Clr();
		Wait_ms_i2c(2);

			if(AT25SF041_SDI)
			{
				// INPUT BIT =1
				dat|=1<<i;
			}
			else
			{
				// INPUT BIT =0
				dat&=~(1<<i);
			}
			AT25SF041_SCK_Set();
			Wait_ms_i2c(2);

	}	
	
	///////////////////////////////////////
	AT25SF041_CS_Set();	
	Wait_ms_i2c(2);
	return dat;

}