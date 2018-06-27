#include "VCNL4035X01.h"

unsigned int Read_VCNL4035X(unsigned int command_code);
void Write_VCNL4035X(unsigned char  command_code,unsigned char dat_h,unsigned char  dat_l);
void Wait_ms_iic(int ms)
{
  unsigned int De_Cnt;
  while( (ms--) != 0)
  {
    for(De_Cnt = 0; De_Cnt < 9; De_Cnt++); 
  }  	
}

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

	I2C_SCK=1;
	I2C_SDA=1;
	Wait_ms_iic(2);
	I2C_SDA=0;
	Wait_ms_i2c(2);
	I2C_SCK=0;
	//Wait_ms_i2c(1);
	//Wait_ms_iic(1);
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

void I2C_Init()
{
		//P41 SDA,P42 SCL quasi-bidirectional

	P3M1 &=~( (1<<1) | (1<<2) );  //~ bitwise NOT
	P3M0 &=~( (1<<1) | (1<<2) );  
	//P4M1 |=0x04;// SCK OPen Drain output
	//P4M0 |=0x04;
	//Write_VCNL4035X(PS_CONF1,0x20,0x00);
	
	Write_VCNL4035X(PS_CONF1,0x00,0x00);//  1 = typical sensitivity mode (two step mode)
	//Write_VCNL4035X(PS_CONF3,0x0F,0x80);
	Write_VCNL4035X(PS_CONF3,0x07,0x80);// 20mA
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



unsigned char I2C_ReceiveByte(void)
{
	unsigned char i;
  unsigned char dat = 0;
	

	//I2C_SDA_Set();					//
  for (i=0; i<8; i++)         //
  {
		dat <<= 1;	              //
		I2C_SCK_Clr();       //
		Wait_ms_i2c(2);        //
        
		if (I2C_SDA) 				//If  I2C_SDO is HIGH, dat will be 1(one) , I2C_SDO is defined in .h file
		{
			dat |= 0x01;            //
		}
		I2C_SCK_Set();       //
		Wait_ms_i2c(2);
		if (i==7)
		{
			I2C_SCK_Clr();
			I2C_SDA_Clr(); 			
		}
		//	
	}
  return dat;
}
void Write_VCNL4035X(unsigned char  command_code,unsigned char dat_h,unsigned char  dat_l)
{
	I2C_Start();
	Wait_ms_i2c(1);
	I2C_SendByte(VCNL4035_addr);
	if(I2C_ACK()==0)
	{
		I2C_SendByte(command_code);
	}
	if(I2C_ACK()==0)
		I2C_SendByte(dat_l);

	if(I2C_ACK()==0)
		I2C_SendByte(dat_h);
	
	if(I2C_ACK()==0)	
		I2C_Stop();
}

unsigned int Read_VCNL4035(unsigned int command_code)
{
	unsigned int temp_dat=0,temp_dat_l=0,temp_dat_h=0;
	
		I2C_Start();
		Wait_ms_i2c(2);
		I2C_SendByte((VCNL4035_addr)|I2C_READ);
		
	if(I2C_ACK()==0)
	{
		P3M1 |=0x02;  //~ bitwise NOT// KEy to work
		P3M0 &=~( (1<<1) | (1<<2) ); //	KEy to work
		temp_dat_l=I2C_ReceiveByte(); // data from proximity sensor
		P3M1 &=~( (1<<1) | (1<<2) );  //~ bitwise NOT//KEy to work
		P3M0 &=~( (1<<1) | (1<<2) ); //KEy to work
	}
	//Wait_ms_i2c(1); 
	
	uC_ACK();
	
	Wait_ms_i2c(4);
	P3M1 |=0x02;  //~ bitwise NOT// KEy to work
	P3M0 &=~( (1<<1) | (1<<2) ); 	//KEy to work	
	temp_dat_h=I2C_ReceiveByte();
	P3M1 &=~( (1<<1) | (1<<2) );  //~ bitwise NOT//KEy to work
	P3M0 &=~( (1<<1) | (1<<2) ); //KEy to work
	uC_NACK();
	Wait_ms_i2c(2);
	I2C_Stop();
	Wait_ms_i2c(2);
		I2C_Start();
	Wait_ms_i2c(2);
	I2C_SendByte((VCNL4035_addr)|I2C_WRITE);
	if(I2C_ACK()==0)
	{
		I2C_SendByte(command_code);
		
	}	
	if(I2C_ACK()==0)
	{
		//Wait_ms_iic(2);
		I2C_SCK=0;
		Wait_ms_iic(1);	
		I2C_Stop();	
		Wait_ms_iic(2);
	}	
	return ((temp_dat_h<<8)|temp_dat_l);
}

void DisplayLCD_VCNL4035(unsigned char dat)
{

}
