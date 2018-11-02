#include "stc15f2k60s2.h"
#include "LCD_Driver_SPLC780D.h"
#include "Receiver_Position_Data.h"
#include "math.h"
#include "SunPos.h"
#include "KeyPad.h"
//#include "PCF85963BTL.h"
//#include "PCF85063BTL.h"

void Delay_ms(unsigned int ms);
int Day_Of_Year(unsigned char months,unsigned char days);
unsigned char BCDtoDec1(char bcd);
void Display_Pos(float sign_dat);
void WriteData(unsigned char dat);
void Command(unsigned char dat);

int lcd=0;
float  degree;
unsigned char previous_move_time=0;
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
			
			//for( i=0;i<step;i++)
			{
					P4 |=1<<1;// P41=1 // moving distance (mm)=pi^2*step*4/675
					Wait_ms(2);
					P4 &= ~(1<<1);
					Wait_ms(2);
			}
}


// P4.5 direction; P4.3 pulse
void Step_move_2ndMotor(unsigned int step, bit dir)
{
			unsigned int i=0;
			if(dir)
				P4 |=(1<<5);// set bit P4.5
			else
				P4 &=~(1<<5);// clear bit P4.5
			
			//for( i=0;i<step;i++)
			{
					P3 |=1<<4;// P43=1 // moving distance (mm)=pi^2*step*4/675
					Wait_ms(2);
					P3 &= ~(1<<4);
					Wait_ms(2);
			}
}

void Move_2ndMotor(float  angle_distance, bit direction,float current_angle)
{
		unsigned int step,i;
		step= (unsigned int)(angle_distance/0.039);// use geared motor 
		//https://www.omc-stepperonline.com/geared-stepper-motor/nema-23-stepper-motor-bipolar-l76mm-w-gear-raio-471-planetary-gearbox-23hs30-2804s-pg47.html
		for( i=0;i<step;i++)
		{
			Step_move_2ndMotor(step,direction);
			if(direction)
				current_angle=current_angle+0.039;// for LCD display only
			else
				current_angle=current_angle-0.039;// for LCD display only
			if(i%50==0)
			{
				Command(0x08);
				Command(0x09);
				Display_Pos(current_angle);
			}
		}
}

void Move(float  distance, bit direction,float current_position)
{
		unsigned long int step,i;
		//int i;
		step= (unsigned long int)(distance*337.5/(3.14159));// need to update this for the system in Vancouver
		for( i=0;i<step;i++)
		{
			Step_move(step,direction);
			if(direction)
				current_position=current_position+0.00931;
			else
				current_position=current_position-0.00931;
			if(i%50==0)
			{			
				Command(0x08);
				Command(0x09);
				Display_Pos(current_position);
				WriteData(0x6D);//m
			}
		}
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
										 float  *currnt_pos, float offset_calib,float  *currnt_angle)
{
	unsigned int date,i=0,yy=0;
	
	float  desired_distance=0,distance=0,angle_distance=0,JP_pos=0,angle,JP_angle=0;
	float  pos_interpolate_azimuth[num_of_azimuth_stamp],angle_interpolate_azimuth[num_of_azimuth_stamp],current_local_sun_time,azimuth, elevation,time_offset,UTC_time=-5;
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
	angle=*currnt_angle;
	//date=Day_Of_Year(mnths,dys)+4;
	//date=237;
	declination=sunpos(time,location,&sunCoord)*180/pi;//+declination_offset;
	time_offset=1/60*(4*(location.dLongitude-15*UTC_time)+9.87*sin(2*(360*(time.iDay-81)/365)*pi/180)    -    7.53*cos((360*(time.iDay-81)/365)*pi/180)    -   1.5*sin((360*(time.iDay-81)/365)*pi/180));
	current_local_sun_time=(float) (BCDtoDec1(hurs))+(float)BCDtoDec1(mns)/60+time_offset-1;//current time=sun time= clock time -1
	//=B10-1/60*(4*($B$7-15*$B$4)+9.87*SIN(2*(360*($B$8-81)/365)*3.1416/180)    -    7.53*COS((360*($B$8-81)/365)*3.1416/180)    -   1.5*SIN((360*($B$8-81)/365)*3.1416/180))
	elevation=(180/pi)*asin(             sin(location.dLatitude*pi/180)*sin(declination*pi/180)+
						cos(location.dLatitude*pi/180)*cos(declination*pi/180)*cos((15*(current_local_sun_time-12))*pi/180)           );
	azimuth=180+(180/pi)*asin(       sin((15*(current_local_sun_time-12))*pi/180)*cos(declination*pi/180)/sin((90-elevation)*pi/180)          );// JP calculation
	//azimuth=(180/pi)*acos(       sin((15*(current_local_sun_time-12))*pi/180)*cos(declination*pi/180)/sin((90-elevation)*pi/180)          );// JP calculation

	
	if(BCDtoDec1(sconds&0x7f)%2==0)
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
			else if ((azimuth<=date_azimuth_mapping_symmetry_side[i]) && (azimuth>=date_azimuth_mapping_symmetry_side[i+1]))
			{
				for (yy=0;yy<num_of_elevation_stamp;yy++)
				{
					p1.x=date_azimuth_mapping_symmetry_side[i];
					p2.x=date_azimuth_mapping_symmetry_side[i+1];
					
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
		
		desired_distance=JP_pos;// +offset_calib;
		//desired_distance=elevation;// +offset_calib;
		//desired_distance=azimuth;// +offset_calib;
		//desired_distance=declination;// +offset_calib;

		distance=desired_distance-*currnt_pos;
		if(abs(distance)>0.5 | abs(previous_move_time-BCDtoDec1(sconds&0x7f))>30)// move if the change is more than 0.5mm OR >30s
		{
			if(distance>0)
				Move(distance,1,*currnt_pos);
			else if (distance<0)
				Move(-distance,0,*currnt_pos);
			previous_move_time=BCDtoDec1(sconds&0x7f);
			*currnt_pos=desired_distance;
		}
		
		//============================================================
		// MOVEMENT CONTROL FOR ANGLE ROTATION
		// Interpolate for azimuth
		//============================================================
		
		for (i=0;i<num_of_azimuth_stamp;i++)
		{
			if ((azimuth<=date_azimuth_mapping[i+1]) && (azimuth>=date_azimuth_mapping[i]))
			{
				for (yy=0;yy<num_of_elevation_stamp;yy++)
				{
					p1.x=date_azimuth_mapping[i];
					p2.x=date_azimuth_mapping[i+1];
					
					p1.y=RX_angle[yy][i];
					p2.y=RX_angle[yy][i+1];
					
					angle_interpolate_azimuth[yy]=linear_interpolate(p1,p2,azimuth);
				}
				//break;
			}
			else if ((azimuth<=date_azimuth_mapping_symmetry_side[i]) && (azimuth>=date_azimuth_mapping_symmetry_side[i+1]))
			{
				for (yy=0;yy<num_of_elevation_stamp;yy++)
				{
					p1.x=date_azimuth_mapping_symmetry_side[i];
					p2.x=date_azimuth_mapping_symmetry_side[i+1];
					
					p1.y=RX_angle[yy][i];
					p2.y=RX_angle[yy][i+1];
					
					angle_interpolate_azimuth[yy]=linear_interpolate(p1,p2,azimuth);
				}
				//break;
			}
			
		}
		// MOVEMENT CONTROL FOR ANGLE ROTATION
		// interpolate for elevation
		for(i=0;i<num_of_elevation_stamp;i++)
		{
			if((elevation>=elevation_stamp[i])&&(elevation<=elevation_stamp[i+1]))
			{
				p1.x=elevation_stamp[i];
				p2.x=elevation_stamp[i+1];
				
				p1.y=angle_interpolate_azimuth[i];
				p2.y=angle_interpolate_azimuth[i+1];
				
				JP_angle=linear_interpolate(p1,p2,elevation);
				//break;
				
			}
			
		}
				
		angle_distance=JP_angle-*currnt_angle;
		if(abs(angle_distance)>0.5 | abs(previous_move_time-BCDtoDec1(sconds&0x7f))>30)// move if the change is more than 0.5mm OR >30s
		{
			if(angle_distance>0)
				Move_2ndMotor(angle_distance,1,*currnt_angle);
			else if (angle_distance<0)
				Move_2ndMotor(-angle_distance,0,*currnt_angle);
			previous_move_time=BCDtoDec1(sconds&0x7f);
			*currnt_angle=JP_angle;
		}
		
	}
	return;

}

void Display_Pos(float sign_dat)
{
	unsigned char unit, ten, hundred,thousand;
	unsigned char after_dot;
	int dat;
	dat=abs(sign_dat);
	if (sign_dat>0)
		after_dot=(unsigned char)((sign_dat-dat)*10);
	else
		after_dot=(unsigned char)((-sign_dat-dat)*10);
	
	unit =dat%10;// remainder after division
	thousand=dat/1000;
	hundred=(dat-thousand*1000)/100;
	ten=(dat-thousand*1000-hundred*100)/10;
	if (sign_dat>=0)
	{
		WriteData(thousand|0x30);
		WriteData(hundred|0x30);
		WriteData(ten|0x30);
		WriteData(unit|0x30);
		WriteData(0x2E);//.
		WriteData((after_dot)|0x30);
		if(!selected_motor)
		{
			WriteData(0x6D);//m
			WriteData(0x6D);//m
			WriteData(0x20);// "blank"
			//WriteData(0x20);// "blank"
			//WriteData(0x20);// "blank"
		}
		else
		{
			WriteData(0xDF);// "blank"
			WriteData(0x20);// "blank"
			WriteData(0x20);// "blank"
		}
	}
	else
	{
		
		WriteData(0x2D);// "-"
		WriteData(thousand|0x30);
		WriteData(hundred|0x30);
		WriteData(ten|0x30);
		WriteData(unit|0x30);
		WriteData(0x2E);//"."
		WriteData(after_dot|0x30);
		if(!selected_motor)
		{
			WriteData(0x6D);//m
			WriteData(0x6D);//m
			WriteData(0x20);// "blank"
			//WriteData(0x20);// "blank"
			//WriteData(0x20);// "blank"
		}
		else
		{
			WriteData(0xDF);// "blank"
			WriteData(0x20);// "blank"
			WriteData(0x20);// "blank"
		}
		//WriteData(0x6D);//m
		//WriteData(0x6D);//m

	}
	return;
}


