#include "stc15f2k60s2.h"
#include "MCP4725.h"
#include <stdlib.h>
#include <string.h>


//unsigned int Read_PI4IOE5V96248(unsigned int command_code);

//void Write_PI4IOE5V96248(struct DATA_FOR_IO_6PORTS *dat);

void Wait_ms_i2c(int ms)
{
  unsigned int De_Cnt;
  while( (ms--) != 0)
  {
    for(De_Cnt = 0; De_Cnt < 9; De_Cnt++); 
  }  	
}

void nop()
{
	return;
}
void I2C_Start()
{

	I2C_SCK=1;
	I2C_SDA=1;
	Wait_ms_i2c(2);
	I2C_SDA=0;
	Wait_ms_i2c(2);
	I2C_SCK=0;
	//Wait_ms_i2c(1);
	//Wait_ms_i2c(1);
}

void I2C_Stop()
{
	I2C_SCK=0;
	I2C_SDA=0;
	Wait_ms_i2c(2);
	I2C_SCK=1;
	I2C_SDA=0;
	Wait_ms_i2c(4);
	I2C_SDA=1;
	//Wait_ms_i2c(2);

}

unsigned char I2C_ACK(void)
{
	unsigned char i;
  unsigned char dat = 0;

	//I2C_SDA_Set();					//
  //for (i=0; i<1; i++)         //
  {
		dat <<= 1;	              //
		I2C_SCK_Clr(); 
		Wait_ms_i2c(2);		//
		//nop();        
		if (I2C_SDA) 				//If  PCF85063BTL_SDO is HIGH, dat will be 1(one) 
		{
			dat |= 0x01;            //
		}
		

		I2C_SCK_Set();       //
		Wait_ms_i2c(2);           //	
	}
  return dat;
}
void uC_ACK()
{

  unsigned char i;
	
  //for (i=0; i<1; i++)            //
  {
    //I2C_SCK_Clr(); 
		//Wait_ms_i2c(1);//		
		//if( (dat&0x80)==0x80 )	  	 //
	 	{
	   		//I2C_SDA_Clr();
	 	}

		Wait_ms_i2c(2);           //
    I2C_SCK_Set();          //
		        //
		//dat <<= 1;                   //
		Wait_ms_i2c(2); 
		I2C_SCK_Clr();		
  }


  
}

void uC_NACK()
{

  unsigned char i;
	
  //for (i=0; i<1; i++)            //
  {
    I2C_SCK_Clr(); 
		//Wait_ms_i2c(1);//		
		//if( (dat&0x80)==0x80 )	  	 //
	 	{
	   		I2C_SDA_Set();
	 	}

		Wait_ms_i2c(2);           //
    I2C_SCK_Set();          //
		        
		//dat <<= 1;                   //
		Wait_ms_i2c(2); 
		I2C_SCK_Clr();
		Wait_ms_i2c(2);
		I2C_SDA_Clr();		
  }


  
}



void I2C_SendByte(unsigned char dat)// Send dat to I2C device/Slave
{
  unsigned char i;
	
  for (i=0; i<8; i++)            //
  {
    I2C_SCK_Clr(); 
		//Wait_ms_i2c(1);//		
		if( (dat&0x80)==0x80 )	  	 //
	 	{
	   		I2C_SDA_Set();
	 	}
	 	else
	 	{
	   		I2C_SDA_Clr();
	 	}
		Wait_ms_i2c(2);           //
    I2C_SCK_Set();          //
		        //
		dat <<= 1;                   //
		Wait_ms_i2c(2);      
  }
}
void Write_MCP4725(int dat)
{
	unsigned char msb,lsb;
	int temp;
	
	temp=dat>>8;//msb
	
	msb=temp;
	
	lsb=dat&0x00ff;
	
	I2C_Start();
	Wait_ms_i2c(1);
	// 1st byte (Device Addressing)
	I2C_SendByte(MCP4725_ADDR);
	if(I2C_ACK()==0)
	{
		//2nd byte
		I2C_SendByte(Write_DAC_Register);
	}
	
	if(I2C_ACK()==0)
	{
		// DAC D11 D10 D9 D8 D7 D6 D5 D4
		I2C_SendByte(msb);
	}
	if(I2C_ACK()==0)
	{
		// D3 D3 D1 D0 X X X X 
		I2C_SendByte(lsb);
	}
	
	if(I2C_ACK()==0)	
		I2C_Stop();
}




//V_OUT=V_REF × Dn/4096
//------------------------------
//Where:
//VREF = VDD
//D
//n = Input code
void WriteVoltage(float voltage)
{
	float vref=5;
	int Dn=voltage*4096/vref;
	Write_MCP4725(Dn);
}