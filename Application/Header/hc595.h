#ifndef __HC595_H__
#define __HC595_H__

#include "stc15f2k60s2.h"

//HC595接口定义
sbit 			HC595_DAT		=	P3^7;	   //HC595串行数据线
sbit 			HC595_SCK		=	P4^2;	   //HC595移位时钟线
sbit 			HC595_RCK		=	P4^1;	   //HC595锁存时钟线
sbit 			HC595_OE		=	P4^5;	   //HC595输出允许线
																
//数码管位定义，从左往右依次为1~8位
#define		DIG1				0		
#define		DIG2				1	
#define		DIG3				2		
#define		DIG4				3	
#define		DIG5				4	
#define		DIG6				5	
#define		DIG7				6			
#define		DIG8				7																	

//端口操作定义
#define 	SET	   			1			   //高电平
#define 	CLR	   			0			   //低电平

//端口操作
#define		HC595_DAT_Clr()	{HC595_DAT=CLR;} 	//端口置低
#define		HC595_DAT_Set()	{HC595_DAT=SET;}	//端口置高

#define		HC595_SCK_Clr()	{HC595_SCK=CLR;} 
#define		HC595_SCK_Set()	{HC595_SCK=SET;}

#define		HC595_RCK_Clr()	{HC595_RCK=CLR;} 
#define		HC595_RCK_Set()	{HC595_RCK=SET;}

#define		HC595_OE_Clr()	{HC595_OE=CLR;} 
#define		HC595_OE_Set()	{HC595_OE=SET;}


 //外部函数
 void HC595_Init(void);
 void LED_Display(void);
 void ToDisplayCorrect(void);
 void ToDisplayError(void);

#endif