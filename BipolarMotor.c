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
		step= (unsigned int)(distance*145.95/(3.14159));// 1.8* step angle, 200 steps ~ 1 round, 107 steps ~ 1mm movement, 
		//l(mm)=step*pi/337.5, L=R1*R3/R2*pi*n/(100*27), R1 is the pulley attached to the motor, R2 is the pulley attached to the long shaft with timing belt, 
		//R3 is the long pulley 
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
	float  desired_distance,distance=0,JP_pos=0;
	float  pos_interpolate_azimuth[num_of_azimuth_stamp],current_local_sun_time,azimuth, elevation,time_offset,UTC_time=-5;
	float declination;
	struct point p1,p2;
	struct cTime time;
	struct cLocation location;
	struct cSunCoordinates *sunCoord;
	//hurs=hurs-1;// change to sun time
	//dys=dys+4;
	location.dLongitude=-73.6495;
	location.dLatitude=45.478889;
	time.iYear=2018;
	time.iMonth=BCDtoDec1(mnths);
	time.iDay=BCDtoDec1(dys);
	time.dHours=BCDtoDec1(hurs);
	time.dMinutes=BCDtoDec1(mns);
	time.dSeconds=BCDtoDec1(sconds&0x7f);

	
	desired_distance=*currnt_pos;
	
	//date=Day_Of_Year(mnths,dys)+4;
	//date=237;
	declination=sunpos(time,location,&sunCoord)*180/pi;//+declination_offset;
	time_offset=1/60*(4*(location.dLongitude-15*UTC_time)+9.87*sin(2*(360*(time.iDay-81)/365)*pi/180)    -    7.53*cos((360*(time.iDay-81)/365)*pi/180)    -   1.5*sin((360*(time.iDay-81)/365)*pi/180));
	current_local_sun_time=(float) (BCDtoDec1(hurs))+(float)BCDtoDec1(mns)/60;//+time_offset-1;//current time=sun time= clock time -1
	//=B10-1/60*(4*($B$7-15*$B$4)+9.87*SIN(2*(360*($B$8-81)/365)*3.1416/180)    -    7.53*COS((360*($B$8-81)/365)*3.1416/180)    -   1.5*SIN((360*($B$8-81)/365)*3.1416/180))
	elevation=(180/pi)*asin(             sin(location.dLatitude*pi/180)*sin(declination*pi/180)+
						cos(location.dLatitude*pi/180)*cos(declination*pi/180)*cos((15*(current_local_sun_time-12))*pi/180)           );
	azimuth=180+(180/pi)*asin(       sin((15*(current_local_sun_time-12))*pi/180)*cos(declination*pi/180)/sin((90-elevation)*pi/180)          );// JP calculation
	//azimuth=(180/pi)*acos(       sin((15*(current_local_sun_time-12))*pi/180)*cos(declination*pi/180)/sin((90-elevation)*pi/180)          );// JP calculation

	
	//if (current_local_sun_time>12)
	//	azimuth=360-azimuth;
	
	if(BCDtoDec1(sconds&0x7f)%3==0)
	{
		// interpolate for azimuth
		for (i=0;i<num_of_azimuth_stamp;i++)
		{
			if ((azimuth<=date_azimuth_mapping[i+1]) && (azimuth>=date_azimuth_mapping[i]))
			{
				for (yy=0;yy<num_of_elevation_stamp;yy++)
				{
					p1.x=date_azimuth_mapping[i];
					p2.x=date_azimuth_mapping[i+1];
					
					p1.y=RX_pos[yy][i];
					p2.y=RX_pos[yy][i+1];
					
					pos_interpolate_azimuth[yy]=linear_interpolate(p1,p2,azimuth);
				}
				//break;
			}
		}

		// interpolate for elevation
		for(i=0;i<num_of_elevation_stamp;i++)
		{
			if((elevation>=elevation_stamp[i])&&(elevation<=elevation_stamp[i+1]))
			{
				p1.x=elevation_stamp[i];
				p2.x=elevation_stamp[i+1];
				
				p1.y=pos_interpolate_azimuth[i];
				p2.y=pos_interpolate_azimuth[i+1];
				
				JP_pos=linear_interpolate(p1,p2,elevation);
				//break;
				
			}
			
		}
		
		desired_distance=11+2*JP_pos+20;// offset manual 20
		
		distance=desired_distance-*currnt_pos;
	
		if(distance>0)
			Move(distance,1);
		else if (distance<0)
			Move(-distance,0);
	}
	*currnt_pos=desired_distance;
	return;

}
