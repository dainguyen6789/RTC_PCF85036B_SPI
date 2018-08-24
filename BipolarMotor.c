#include "stc15f2k60s2.h"
#include "LCD_Driver_SPLC780D.h"
#include "Receiver_Position_Data.h"
#include "math.h"
#include "SunPos.h"
//#include "PCF85963BTL.h"
void Delay_ms(unsigned int ms);
int Day_Of_Year(unsigned char months,unsigned char days);
unsigned char BCDtoDec1(char bcd);
float  degree;
float  findDet4x4 (float  a11, float  a12, float  a13, float  a14, 
            float  a21, float  a22, float  a23, float  a24,
            float  a31, float  a32, float  a33, float  a34,
            float  a41, float  a42, float  a43, float  a44 );
 float  findDet3x3( 
            float  a11, float  a12, float  a13, 
            float  a21, float  a22, float  a23,
            float  a31, float  a32, float  a33 );						
float
cubic_interpolate (float *p1, float *p2, float *p3, float *p4, float x);

unsigned char BCDtoDec1(unsigned char bcd)
{
	unsigned char hi,lo;
	hi=bcd>>4;
	lo=bcd&0x0F;
	return hi*10+lo;
}





void Step_move(unsigned int step, bit dir)
{
			unsigned int i=0;
			if(dir)
				P4 |=(1<<2);// set bit P4.2
			else
				P4 &=~(1<<2);// clear bit P4.2
			
			for( i=0;i<step;i++)
			{
					P4 |=1<<1;// P41=1 // moving distance (mm)=pi^2*step*4/675
					Wait_ms(2);
					P4 &= ~(1<<1);
					Wait_ms(2);
			}
}

void Move(float  distance, bit direction)
{
		unsigned int step;
		step= (unsigned int)(distance*337.5/(3.14159));
		Step_move(step,direction);
}
//=====================================================
//We need 4 points for cubic interpolation, p1[x],p1[y],..,p4[x],p4[y]
 float  findDet3x3( 
            float  a11, float  a12, float  a13, 
            float  a21, float  a22, float  a23,
            float  a31, float  a32, float  a33 )
{
		return( a11*a22*a33 + a12*a23*a31 + a13*a21*a32 -
						a13*a22*a31 - a12*a21*a33 - a11*a23*a32 );
}
float  findDet4x4 (float  a11, float  a12, float  a13, float  a14, 
            float  a21, float  a22, float  a23, float  a24,
            float  a31, float  a32, float  a33, float  a34,
            float  a41, float  a42, float  a43, float  a44 )
{
	
		//return a11*(float)findDet3x3(a22, a23, a24, a32, a33, a34, a42, a43, a44);
		//return (float)findDet3x3(a21, a23, a24, a31, a33, a34, a41, a43, a44);
		//return (float)findDet3x3(a21, a22, a24, a31, a32, a34, a41, a42, a44);
		//return (float)findDet3x3(a21, a22, a23, a31, a32, a33, a41, a42, a43);
	//	return a21;
		return( a11*findDet3x3(a22, a23, a24, a32, a33, a34, a42, a43, a44) -
						a12*findDet3x3(a21, a23, a24, a31, a33, a34, a41, a43, a44) +
						a13*findDet3x3(a21, a22, a24, a31, a32, a34, a41, a42, a44) -
						a14*findDet3x3(a21, a22, a23, a31, a32, a33, a41, a42, a43));
}
		
//=====================================================	

 
//===================================================== 
float  linear_interpolate(struct point p1,struct point p2, float  x)
{
	float  fx;
	
	fx=-(p1.x-x)/(p1.x-p2.x)*(p1.y-p2.y)+p1.y;
	
	return fx;
}


void Update_position(unsigned char mnths,unsigned char dys,
										 unsigned char hurs,unsigned char mns,unsigned char sconds,
										 float  *currnt_pos)
{
	unsigned int date,i=0,yy=0;
	float  desired_distance,distance=0,JP_pos;
	float  pos_interpolate_12_17h[num_of_time_stamp],current_time;
	float declination;
	struct point p1,p2;
	struct cTime time;
	struct cLocation location;
	struct cSunCoordinates *sunCoord;
	hurs=hurs-1;// change to sun time
	dys=dys+4;
	time.iYear=2018;
	time.iMonth=BCDtoDec1(mnths);
	time.iDay=BCDtoDec1(dys);
	time.dHours=BCDtoDec1(hurs);
	time.dMinutes=BCDtoDec1(mns);
	time.dSeconds=BCDtoDec1(sconds&0x7f);

	
	desired_distance=*currnt_pos;
	date=Day_Of_Year(mnths,dys);
	//date=237;
	declination=sunpos(time,location,&sunCoord)*180/pi;

	current_time=(float) BCDtoDec1(hurs)+(float)BCDtoDec1(mns)/60;//+(float)BCDtoDec1(sconds&0x7f)/3600;
	if(BCDtoDec1(sconds&0x7f)%3==0)
	{
	// interpolate for day
		for (i=0;i<num_of_day_stamp;i++)
		{
			if ((declination<=date_declination_mapping[i]) && (declination>=date_declination_mapping[i+1]))
			{
				for (yy=0;yy<num_of_time_stamp;yy++)
				{
					p1.x=date_declination_mapping[i];
					p2.x=date_declination_mapping[i+1];
					
					p1.y=RX_pos[yy][i];
					p2.y=RX_pos[yy][i+1];
					
					pos_interpolate_12_17h[yy]=linear_interpolate(p1,p2,declination);
				}
				//break;
			}
		}

		// interpolate for hour
		for(i=0;i<num_of_time_stamp;i++)
		{
			if((current_time>=Time_stamp_PM[i])&&(current_time<=Time_stamp_PM[i+1]))
			{
				p1.x=Time_stamp_PM[i];
				p2.x=Time_stamp_PM[i+1];
				
				p1.y=pos_interpolate_12_17h[i];
				p2.y=pos_interpolate_12_17h[i+1];
				
				JP_pos=linear_interpolate(p1,p2,current_time);
				//break;
				
			}
			
		}
		
		desired_distance=179+2*JP_pos;
		
		distance=desired_distance-*currnt_pos;
	
		if(distance>0)
			Move(distance,1);
		else if (distance<0)
			Move(-distance,0);
	}
	*currnt_pos=desired_distance;
	return;

}
