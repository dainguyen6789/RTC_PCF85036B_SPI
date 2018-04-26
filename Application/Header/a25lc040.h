#ifndef __A25LC040_H__
#define __A25LC040_H__

#include "stc15f2k60s2.h"

//A25LC040接口定义
 sbit 			A25LC040_CS		=	P2^0;	   //A25LC040片选端
 sbit 			A25LC040_SCK	=	P2^1;	   //A25LC040时钟端
 sbit 			A25LC040_SDI	=	P2^3;	   //A25LC040数据输入端
 sbit 			A25LC040_SDO	=	P2^2;	   //A25LC040数据输出端

//端口操作
 #define		A25LC040_CS_Clr()			{A25LC040_CS=0;} //置低
 #define		A25LC040_CS_Set()			{A25LC040_CS=1;} //置高

 #define		A25LC040_SCK_Clr()		{A25LC040_SCK=0;} 
 #define		A25LC040_SCK_Set()		{A25LC040_SCK=1;}

 #define		A25LC040_SDI_Clr()		{A25LC040_SDI=0;} 
 #define		A25LC040_SDI_Set()		{A25LC040_SDI=1;}

 #define		A25LC040_SDO_Clr()		{A25LC040_SDO=0;} 
 #define		A25LC040_SDO_Set()		{A25LC040_SDO=1;}
 
 //25LC040A寄存器定义
 #define		READH		0x0b		//读取高256字节单元
 #define		READL		0x03		//读取低256字节单元
 #define		WRITEH	0x0a		//写高256字节单元
 #define		WRITEL	0x02		//写低256字节单元
 #define		WREN		0x06		//写使能
 #define		WRDI		0x04		//写禁止
 #define		RDSR		0x05		//读状态寄存器
 #define		WRSR		0x01		//写状态寄存器

//外部函数声明
void A25LC040_Init(void);
unsigned char A25LC040_ReadByte(unsigned int addt);
void A25LC040_WriteByte(unsigned int add,unsigned char dat);

#endif