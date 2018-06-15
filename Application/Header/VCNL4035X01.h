 sbit 			I2C_SDA	=	P2^4;	   
 sbit 			I2C_SCK	=	P2^5;	   
 



#define VCNL4035_addr 0x60

#define ALS_CONF1 0x00
#define ALS_CONF2 0x00

#define ALS_THDH_L 0x01
#define ALS_THDH_M 0x01

#define ALS_THDL_L 0x02
#define ALS_THDL_M 0x02

#defined PS_CONF1  0x03
#defined PS_CONF2  0x03

#defined PS_CONF3  0x04
#defined PS_MS	   0x04

#defined PS_CANC_L 0x05
#defined PS_CANC_M 0x05

#defined PS_THDL_L 0x06
#defined PS_THDL_M 0x06

#defined PS_THDH_L 0x07
#defined PS_THDH_M 0x07

#defined PS1_Data_L 0x08
#defined PS1_Data_M 0x08

#defined PS2_Data_L 0x09
#defined PS2_Data_M 0x09

#defined PS3_Data_L 0x0A
#defined PS3_Data_M 0x0A

#defined ALS_Data_L 0x0B
#defined ALS_Data_M 0x0B

#defined White_Data_L 0x0C
#defined White_Data_M 0x0C

#defined Reserved 		0x0D
#defined White_Data_M 0x0D

#defined ID_L 				0x0E
#defined ID_M				 	0x0D

#defined I2C_READ			0x01
#defined I2C_WRITE		0x00









