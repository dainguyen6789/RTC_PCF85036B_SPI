#include "AT25SF041.h"
//
void AT25SF041_WriteEnable(void)
{
	int i;
	char write_en;
	write_en=Write_Enable;
//	AT25SF041_CS_Clr();
//Wait_ms_i2c(2);
	
	for( i=0;i<8;i++)
	{
				AT25SF041_SCK_Clr();
				Wait_ms_i2c(2);
				if( (write_en&0x80)==0x80 )	  	 //
				{
						AT25SF041_SDI_Set();
				}	
					else
				{
						AT25SF041_SDI_Clr();

				}
				Wait_ms_i2c(2); 		
				AT25SF041_SCK_Set();
				Wait_ms_i2c(2);
				write_en <<= 1;                   //


	}
	//AT25SF041_CS_Set();
	//Wait_ms_i2c();
	
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
				if( (opcode&0x80)==0x80 )	  	 //
				{
						AT25SF041_SDI_Set();
				}	
					else
				{
						AT25SF041_SDI_Clr();

				}
				Wait_ms_i2c(2); 		
				AT25SF041_SCK_Set();
				Wait_ms_i2c(2);
				opcode <<= 1;                   //


		}
	// SEND THE ADDR (24-bit)
	for( i=0;i<24;i++)
	{
			AT25SF041_SCK_Clr();
			Wait_ms_i2c(2);
			if( (addr&0x80)==0x80 )	  	 //
			{
					AT25SF041_SDI_Set();
			}	
				else
			{
					AT25SF041_SDI_Clr();

			}
			Wait_ms_i2c(2); 		
			AT25SF041_SCK_Set();
			Wait_ms_i2c(2);
			addr <<= 1;

	}
	
		for( i=0;i<8;i++)
	{
			AT25SF041_SCK_Clr();
			Wait_ms_i2c(2);
			if( (dat&0x80)==0x80 )	  	 //
			{
					AT25SF041_SDI_Set();
			}	
				else
			{
					AT25SF041_SDI_Clr();

			}
			Wait_ms_i2c(2); 		
			AT25SF041_SCK_Set();
			Wait_ms_i2c(2);
			dat <<= 1;

	}
	///////////////////////////////////////
	//AT25SF041_CS_Set();
	//Wait_ms_i2c(2);
	
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
			if( (opcode&0x80)==0x80 )	  	 //
			{
					AT25SF041_SDI_Set();
			}	
				else
			{
					AT25SF041_SDI_Clr();

			}
			Wait_ms_i2c(2); 		
			AT25SF041_SCK_Set();
			Wait_ms_i2c(2);
			opcode <<= 1;                   //


	}
	// SEND THE ADDR (24-bit)
	for( i=0;i<24;i++)
	{
			AT25SF041_SCK_Clr();
			Wait_ms_i2c(2);
			if( (addr&0x80)==0x80 )	  	 //
			{
					AT25SF041_SDI_Set();
			}	
				else
			{
					AT25SF041_SDI_Clr();

			}
			Wait_ms_i2c(2); 		
			AT25SF041_SCK_Set();
			Wait_ms_i2c(2);
			addr <<= 1;

	}
	/////////////////////////////////////////
	// Store the output data from NOR FLash
	AT25SF041_SDO_Set();
	for( i=0;i<8;i++)
	{
			dat <<= 1;	              //
			AT25SF041_SCK_Set();
			Wait_ms_i2c(2);
			AT25SF041_SCK_Clr();
			Wait_ms_i2c(2);

			if(AT25SF041_SDO)
			{
				// INPUT BIT =1
				dat |= 0x01;            //
			}

			//Wait_ms_i2c(2); 
			//AT25SF041_SCK_Set();
			//Wait_ms_i2c(2);

	}	
	
	///////////////////////////////////////
	AT25SF041_CS_Set();	
	Wait_ms_i2c(2);
	return dat;

}