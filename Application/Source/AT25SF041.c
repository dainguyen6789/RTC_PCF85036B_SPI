#include "AT25SF041.h"
//
void AT25SF041_WriteEnable(void)
{
	AT25SF041_CS_Clr();
	for(int i=0;i<8;i++)
	{
			AT25SF041_SCK_Clr();
			AT25SF041_SDO=(Write_Enable<<i)&0x01;
			AT25SF041_SCK_Set();
	}
	AT25SF041_CS_Clr();
}


void AT25SF041_Write(char opcode, unsigned long int addr,char dat)
{
	AT25SF041_CS_Clr();
	//////////////////////////////////////////
	for(int i=0;i<8;i++)
	{
			AT25SF041_SCK_Clr();
			AT25SF041_SDO=(opcode<<i)&0x01;
			AT25SF041_SCK_Set();
	}
	
		for(int i=0;i<23;i++)
	{
			AT25SF041_SCK_Clr();
			AT25SF041_SDO=(addr<<i)&0x01;
			AT25SF041_SCK_Set();
	}
	
		for(int i=0;i<8;i++)
	{
			AT25SF041_SCK_Clr();
			AT25SF041_SDO=(dat<<i)&0x01;
			AT25SF041_SCK_Set();
	}
	///////////////////////////////////////
	AT25SF041_CS_Set();

	
}

char  AT25SF041_Read((char opcode,unsigned long int addr)
{
	char dat=0;
	AT25SF041_CS_Clr();
	//////////////////////////////////////////
	// Send the OPCODE
	for(int i=0;i<8;i++)
	{
			AT25SF041_SCK_Clr();
			AT25SF041_SDO=(opcode<<i)&0x01;
			AT25SF041_SCK_Set();
	}
	// SEND THE ADDR (24-bit)
		for(int i=0;i<23;i++)
	{
			AT25SF041_SCK_Clr();
			AT25SF041_SDO=(addr<<i)&0x01;
			AT25SF041_SCK_Set();
	}
	/////////////////////////////////////////
	// Store the output data from NOR FLash
	for(int i=0;i<8;i++)
	{
			AT25SF041_SCK_Clr();
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
	}	
	
	///////////////////////////////////////
	AT25SF041_CS_Set();	

}