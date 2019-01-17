#include "PI4IOE5V96248.h"

//unsigned int Read_PI4IOE5V96248(unsigned int command_code);

void Write_PI4IOE5V96248(struct DATA_FOR_IO_6PORTS *dat);

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



void I2C_SendByte(unsigned char dat)// Send dat to SPI device/Slave
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
void Write_PI4IOE5V96248(struct DATA_FOR_IO_6PORTS *xdat)
{
	I2C_Start();
	Wait_ms_i2c(1);
	I2C_SendByte(PI4IOE5V96248_Write_Addr);
	if(I2C_ACK()==0)
	{
		I2C_SendByte(xdat->port0);
	}
	
	if(I2C_ACK()==0)
	{
		I2C_SendByte(xdat->port1);
	}
	if(I2C_ACK()==0)
	{
		I2C_SendByte(xdat->port2);
	}
	if(I2C_ACK()==0)
	{
		I2C_SendByte(xdat->port3);
	}
	
	if(I2C_ACK()==0)
	{
		I2C_SendByte(xdat->port4);
	}
	if(I2C_ACK()==0)
	{
		I2C_SendByte(xdat->port5);
	}	
	
	if(I2C_ACK()==0)	
		I2C_Stop();
}

