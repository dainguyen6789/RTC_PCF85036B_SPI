#include "a25lc040.h"

//包含该头文件可以使用_nop_()函数
#include <intrins.h>

/***********************************************
函数名称：A25LC040_Delay
功    能：STC 1T单片机1us延时程序
入口参数：us:延时的微秒数
返 回 值：无	
备    注：外部时钟11.0592MHz
************************************************/
void A25LC040_Delay(unsigned int us)
{
  while( us--)
  {
    _nop_(); 
  }             
}
/***********************************************
函数名称：SPI_SendByte
功    能：SPI总线发送一个字节的数据
入口参数：dat：写入的数据
返 回 值：无	
备    注：无
************************************************/
void SPI_SendByte(unsigned char dat)
{
  unsigned char i;
  for (i=0; i<8; i++)            //8位计数器
  {
    A25LC040_SCK_Clr();          //时钟线拉低		
		if( (dat&0x80)==0x80 )	  	 // check every bit of the data , if it is 1 then set MOSI to 1, otherwise set to Zero
	 	{
	   		A25LC040_SDI_Set();
	 	}
	 	else
	 	{
	   		A25LC040_SDI_Clr();
	 	}
		A25LC040_Delay(1);           //延时等待
    A25LC040_SCK_Set();          //时钟线拉高
		A25LC040_Delay(1);           //延时等待
		dat <<= 1;                   //移出数据
  }
}
/***********************************************
函数名称：SPI_ReceiveByte
功    能：从SPI总线接收一个字节数据
入口参数：无
返 回 值：读出的数据	
备    注：无
************************************************/
unsigned char SPI_ReceiveByte(void)
{
	unsigned char i;
  unsigned char dat = 0;

	A25LC040_SDO_Set();					//做输入先置高
  for (i=0; i<8; i++)         //8位计数器
  {
		dat <<= 1;	              //数据左移一位
		A25LC040_SCK_Clr();       //时钟线拉低
		A25LC040_Delay(1);        //延时等待
        
		if (A25LC040_SDO) 				//先接收高位
		{
			dat |= 0x01;            //读取数据
		}
		A25LC040_SCK_Set();       //时钟线拉高
		A25LC040_Delay(1);        //延时等待		
	}
  return dat;
}
/***********************************************
函数名称：A25LC040_WriteEnable
功    能：写使能。
入口参数：无
返 回 值：无	
备    注：无	
************************************************/
void A25LC040_WriteEnable(void)
{
  //拉低使能端 
  A25LC040_CS_Clr();

  //写入写使能命令
  SPI_SendByte(WREN);

  //拉高使能端 
  A25LC040_CS_Set();
}
/***********************************************
函数名称：A25LC040_WriteDisable
功    能：写禁止。
入口参数：无
返 回 值：无	
备    注：无
************************************************/
void A25LC040_WriteDisable(void)
{
  //拉低使能端 
  A25LC040_CS_Clr();

  //写入写禁止命令
  SPI_SendByte(WRDI);

  //拉高使能端 
  A25LC040_CS_Set();
}
/***********************************************
函数名称：A25LC040_WaitForWriteEnd
功    能：等待写结束
入口参数：无
返 回 值：无	
备    注：无
************************************************/
bit A25LC040_WaitForWriteEnd(void)
{
  unsigned char status = 0;

  //拉低使能端 
  A25LC040_CS_Clr();

  //发送读取寄存器指令
  SPI_SendByte(RDSR);
	//读取寄存器
  status = SPI_ReceiveByte();
  	
	//判断忙标记
  if( (status & 0x01) == 0x01)
	{
		return 1;	   //忙返回1
	}
	else
	{
		return 0;
	}

  //芯片禁止 
  A25LC040_CS_Set();
}
/***********************************************
函数名称：A25LC040_WriteByte
功    能：向指定地址写入一个字节数据
入口参数：
					add：欲写入的地址。
					dat：写入的数据
返 回 值：无	
备    注：详见数据手册中时序说明
************************************************/
void A25LC040_WriteByte(unsigned int add,unsigned char dat)
{
	//写数据之前必须执行写使能
	A25LC040_WriteEnable();	

	//拉低使能端
  A25LC040_CS_Clr();
	//判断地址第9位，写命令包含地址第9位
	if( (add&0x100)==0x100)
	{
		//写入写命令和地址第9位 
  	SPI_SendByte(WRITEH);
	}
	else
	{
		SPI_SendByte(WRITEL);	
	}

	//写入地址低八位
	SPI_SendByte(add&0xff);

	//写入数据
	SPI_SendByte(dat);

	//拉高使能端 
  A25LC040_CS_Set();

	A25LC040_Delay(5);    //延时等待	

	//等待写结束
	while(A25LC040_WaitForWriteEnd());		

  //写结束后关闭写使能，保护数据
	A25LC040_WriteDisable();
}
/***********************************************
函数名称：A25LC040_ReadByte
功    能：从指定地址读出一个字节数据
入口参数：
					add：欲读取的地址。
返 回 值：unsigned char 读出的数据	
备    注：详见数据手册中时序说明
************************************************/
unsigned char A25LC040_ReadByte(unsigned int add)
{
	unsigned char dat_temp;	

	//拉使能端 
  A25LC040_CS_Clr();

	//判断地址第9位，读命令包含地址第9位
	if( (add&0x100)==0x100)
	{
		//写入读命令和地址第八位 
  		SPI_SendByte(READH);
	}
	else
	{
		SPI_SendByte(READL);	
	}

	//写入地址低八位
	SPI_SendByte(add&0xff);

	//读出数据
	dat_temp=SPI_ReceiveByte();
	//拉低使能端 
  A25LC040_CS_Set();

	return dat_temp;
}
/***********************************************
函数名称：A25LC040_Init
功    能：25LC040初始化。
入口参数：无
返 回 值：无	
备    注：无
************************************************/
void A25LC040_Init(void)
{
	//初始化P20,P21,P22 P23口为准双向口
	P2M1 &=~( (1<<0) | (1<<1) | (1<<2) | (1<<3) );  
	P2M0 &=~( (1<<0) | (1<<1) | (1<<2) | (1<<3) );  
	A25LC040_CS_Set();
	A25LC040_SCK_Clr();
}