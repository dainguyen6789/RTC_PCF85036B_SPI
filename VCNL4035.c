#include "VCNL4035X01.h"

void Read_VCNL4035();
void Write_VCNl4035();

void I2C_SendByte(unsigned char dat)// Send dat to SPI device/Slave
{
  unsigned char i;
	
  for (i=0; i<8; i++)            //
  {
    I2C_SCK_Clr();          //		
		if( (dat&0x80)==0x80 )	  	 //
	 	{
	   		I2C_SDA_Set();
	 	}
	 	else
	 	{
	   		I2C_SDA_Clr();
	 	}
		//I2C_Delay(1);           //
    I2C_SCK_Set();          //
		I2C_Delay(1);           //
		dat <<= 1;                   //
  }
}



unsigned char I2C_ReceiveByte(void)
{
	unsigned char i;
  unsigned char dat = 0;
	

	I2C_SDO_Set();					//
  for (i=0; i<8; i++)         //
  {
		dat <<= 1;	              //
		I2C_SCK_Clr();       //
		I2C_Delay(1);        //
        
		if (I2C_SDA) 				//If  I2C_SDO is HIGH, dat will be 1(one) , I2C_SDO is defined in .h file
		{
			dat |= 0x01;            //
		}
		I2C_SCK_Set();       //
		I2C_Delay(1);        //	
	}
  return dat;
}
void Write_VCNL4035(unsigned char  command_mode,unsigned char dat_h,unsigned char  dat_l)
{
	I2C_Start();
	I2C_SendByte(VCNL4035_addr|I2C_WRITE);
	if(I2C_ACK()==0)
	{
		I2C_SendByte(command_mode);
	}
	if(I2C_ACK()==0)
		I2C_SendByte(dat_l);

	if(I2C_ACK()==0)
		I2C_SendByte(dat_h);
	
	if(I2C_ACK()==0)	
		I2C_Stop();
}

unsigned int Read_VCNL4035(unsigned char  command_code)
{
	unsigned int temp_dat=0;
	I2C_Start();
	I2C_SendByte(VCNL4035_addr|I2C_WRITE);
	if(I2C_ACK()==0)
	{
		I2C_SendByte(command_mode);
	}	
	if(I2C_ACK()==0)
	{
		I2C_SendByte(VCNL4035_addr|I2C_READ);
	}		
	if(I2C_ACK()==0)
	{
		temp_dat_l|=I2C_ReceiveByte();
	}
	
	
	uC_ACK();
	temp_dat_h|=I2C_ReceiveByte();
	
	I2C_Stop();	
}

void I2C_Start()
{
	I2C_SCL_Pin=1;
	I2C_SDA_Pin=1;
	Wait_ms(30);
	I2C_SDA_Pin=0;
}

void I2C_Stop()
{
	I2C_SCL_Pin=1;
	I2C_SDA_Pin=0;
	Wait_ms(30);
	I2C_SDA_Pin=1;
}
bit I2C_ACK()
{
	I2C_SCK_Clr(); 
	I2C_SCK_Set();
  return 	I2C_SDA;
	
}
void uC_ACK()
{
	I2C_SCK_Clr(); 
	I2C_SCK_Set();
  I2C_SDA=0;
}