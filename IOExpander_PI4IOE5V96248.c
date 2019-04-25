#include "PI4IOE5V96248.h"

//unsigned int Read_PI4IOE5V96248(unsigned int command_code);

void Write_PI4IOE5V96248(struct DATA_FOR_IO_6PORTS *dat);

void Wait_ms_i2c(int ms)
{
  unsigned int De_Cnt;
  while( (ms--) != 0)
  {
    for(De_Cnt = 0; De_Cnt < 1; De_Cnt++); 
  }  	
}

void nop()
{
	return;
}
void I2C_Start()
{
	// set p5.4 as OUTPUT
	P5M0|=1<<4;
	P5M1&=~(1<<4);
	//==================================
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
unsigned char I2C_ReceiveByte(void)
{
	unsigned char i;
  unsigned char dat = 0;

	I2C_SDA_Set();					//
  for (i=0; i<8; i++)         //
  {
		dat <<= 1;	              //
		I2C_SCK_Clr();       //
		Wait_ms_i2c(1);        //
        
		if (I2C_SDA) 				//If  PCF85063BTL_SDO is HIGH, dat will be 1(one) 
		{
			dat |= 0x01;            //
		}
		I2C_SCK_Set();       //
		Wait_ms_i2c(1);        //	
	}
  return dat;
}

unsigned int Read_VEML6030_ALS(unsigned char command_code)
{
		unsigned int temp_dat_l=0,temp_dat_h=0;
		//====================================
		I2C_Start();
		Wait_ms_i2c(2);
		I2C_SendByte(VEML6030_Write_Addr);
		if(I2C_ACK()==0)
		{
			I2C_SendByte(command_code);
			
		}	
		if(I2C_ACK()==0)
		{
			Wait_ms_i2c(2);
			I2C_SCK=0;
			Wait_ms_i2c(1);	
			I2C_Stop();	
			Wait_ms_i2c(2);
			I2C_Start();
		}
		Wait_ms_i2c(2);
		I2C_SendByte(VEML6030_Read_Addr);
		
		if(I2C_ACK()==0)
		{
			P4M1 |=1<<7;  //~ bitwise NOT// KEy to work
			P4M0 &=~ (1<<7); //	KEy to work
			temp_dat_l=I2C_ReceiveByte(); // data from proximity sensor
			P4M1 &=~( 1<<7 );  //~ bitwise NOT//KEy to work
			P4M0 &=~( (1<<7)); //KEy to work
		}
		//Wait_ms_i2c(1); 
		
		uC_ACK();
		
		Wait_ms_i2c(4);
		//Set P4.7 as input
		P4M1 |=1<<7;  //~ bitwise NOT// KEy to work
		P4M0 &=~( (1<<7) ); 	//KEy to work	
		temp_dat_h=I2C_ReceiveByte();
		//Set P4.7 as OUTPUT
		P4M1 &=~( 1<<7);  //~ bitwise NOT//KEy to work
		P4M0 &=~( 1<<7 ); //KEy to work
		uC_NACK();
		Wait_ms_i2c(2);
		I2C_Stop();
		Wait_ms_i2c(2);

	return ((temp_dat_h<<8)|temp_dat_l);
}


void Write_VEML6030_ALS(unsigned char command_code,unsigned char dat_h,unsigned char dat_l)// write 16-bit of data
{
	
	P4M1 &=~( 1<<7);  //~ bitwise NOT//KEy to work
	P4M0 &=~( 1<<7 ); //KEy to work
	I2C_Start();
	Wait_ms_i2c(1);
	I2C_SendByte(VEML6030_Write_Addr);
	if(I2C_ACK()==0)
	{
			P4M1 &=~( 1<<7);  //~ bitwise NOT//KEy to work
			P4M0 &=~( 1<<7 ); //KEy to work
		I2C_SendByte(command_code);
	}
	if(I2C_ACK()==0)
		I2C_SendByte(dat_l);

	if(I2C_ACK()==0)
		I2C_SendByte(dat_h);
	
	if(I2C_ACK()==0)	
		I2C_Stop();


	
}

