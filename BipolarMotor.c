#include "stc15f2k60s2.h"
#include "LCD_Driver_SPLC780D.h"
#include "Receiver_Position_Data.h"
#include "math.h"
//#include "PCF85963BTL.h"
void Delay_ms(unsigned int ms);
int Day_Of_Year(unsigned char months,unsigned char days);
unsigned char BCDtoDec1(char bcd);
float degree;
float findDet4x4 (float a11, float a12, float a13, float a14, 
            float a21, float a22, float a23, float a24,
            float a31, float a32, float a33, float a34,
            float a41, float a42, float a43, float a44 );
 float findDet3x3( 
            float a11, float a12, float a13, 
            float a21, float a22, float a23,
            float a31, float a32, float a33 );						
						
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

void Move(float distance, bit direction)
{
		unsigned int step;
		step= (unsigned int)(distance*337.5/(3.14159));
		Step_move(step,direction);
}
//=====================================================
//We need 4 points for cubic interpolation, p1[x],p1[y],..,p4[x],p4[y]
float
cubic_interpolate (float *p1, float *p2, float *p3, float *p4, float x)
{
  float a, b, c, d, det;	// the parameters of the cubic function
  float a11, a12, a13, a14, a21, a22, a23, a24,
    a31, a32, a33, a34, a41, a42, a43, a44, k1, k2, k3, k4;

  a11 = pow (p1[0], 3);
  a21 = pow (p2[0], 3);
  a31 = pow (p3[0], 3);
  a41 = pow (p4[0], 3);

  a12 = pow (p1[0], 2);
  a22 = pow (p2[0], 2);
  a32 = pow (p3[0], 2);
  a42 = pow (p4[0], 2);

  a13 = pow (p1[0], 1);
  a23 = pow (p2[0], 1);
  a33 = pow (p3[0], 1);
  a43 = pow (p4[0], 1);

  a14 = 1;
  a24 = 1;
  a34 = 1;
  a44 = 1;

  k1 = p1[1];
  k2 = p2[1];
  k3 = p3[1];
  k4 = p4[1];


  det = findDet4x4 (a11, a12, a13, a14, a21, a22, a23, a24,
		    a31, a32, a33, a34, a41, a42, a43, a44);
  a = findDet4x4 (k1, a12, a13, a14, k2, a22, a23, a24,
		  k3, a32, a33, a34, k4, a42, a43, a44) / det;

  b = findDet4x4 (a11, k1, a13, a14, a21, k2, a23, a24,
		  a31, k3, a33, a34, a41, k4, a43, a44) / det;

  c = findDet4x4 (a11, a12, k1, a14, a21, a22, k2, a24,
		  a31, a32, k3, a34, a41, a42, k4, a44) / det;
  d = findDet4x4 (a11, a12, a13, k1, a21, a22, a23, k2,
		  a31, a32, a33, k3, a41, a42, a43, k4) / det;

    //return a21;
  return a *pow(x,3)+b*pow(x,2)+c*x+d;
}
//=====================================================
float findDet4x4 (float a11, float a12, float a13, float a14, 
            float a21, float a22, float a23, float a24,
            float a31, float a32, float a33, float a34,
            float a41, float a42, float a43, float a44 )
{
		return( a11*findDet3x3(a22, a23, a24, a32, a33, a34, a42, a43, a44) -
						a12*findDet3x3(a21, a23, a24, a31, a33, a34, a41, a43, a44) +
						a13*findDet3x3(a21, a22, a24, a31, a32, a34, a41, a42, a44) -
						a14*findDet3x3(a21, a22, a23, a31, a32, a33, a41, a42, a43));
}
		
//=====================================================	
 float findDet3x3( 
            float a11, float a12, float a13, 
            float a21, float a22, float a23,
            float a31, float a32, float a33 )
{
		return( a11*a22*a33 + a12*a23*a31 + a13*a21*a32 -
						a13*a22*a31 - a12*a21*a33 - a11*a23*a32 );
}
//=====================================================
float linear_interpolate(float x1,float x2,float y1,float y2, float x)
{
	float a,b;
	a=(y1-y2)/(x1-x2);
	b=y1-a*x1;
	
	return a*x+b;
}


void Update_position(unsigned char mnths,unsigned char dys,
										 unsigned char hurs,unsigned char mns,unsigned char sconds,
										 float *currnt_pos)
{
	unsigned int date,i,yy;
	float desired_distance,distance=0;
	float pos_interpolate_12_17h[21][1],p1[2],p2[2],p3[2],p4[2],current_time;
	/*hurs=0x12;
	mns=0x30;*/
	//sconds=0x15;
	desired_distance=*currnt_pos;
	date=Day_Of_Year(mnths,dys);
	current_time=(float)BCDtoDec1(hurs)+(float)BCDtoDec1(mns)/60+(float)BCDtoDec1(sconds&0x7f)/3600;
	if (date>=79 && date <=171)
	{
		// interpolate for day
		for (i=0;i<8;i++)
		{
			if( (date >=date_stamp_79_171[i]) && (date <= date_stamp_79_171[i+1]))
			{
				if (i==0)// 79 to 87
				{
					p1[0]=date_stamp_79_171[0];
					p2[0]=date_stamp_79_171[1];
					p3[0]=date_stamp_79_171[2];
					p4[0]=date_stamp_79_171[3];
					for (yy=0;yy<21;yy++)
					{
						p1[1]=exact_rx_pos_simulation[yy][0];
						p2[1]=exact_rx_pos_simulation[yy][1];
						p3[1]=exact_rx_pos_simulation[yy][2];
						p4[1]=exact_rx_pos_simulation[yy][3];
						// this will give the interpolated position of "date" at time stamp defined in Time_stamp_PM[21][2]
						pos_interpolate_12_17h[yy][1]=cubic_interpolate(p1,p2,p3,p4,date);
					}
				}
				break;
			}
			else if (i==7)// 152 to 171
			{
					p1[0]=date_stamp_79_171[i+1];
					p2[0]=date_stamp_79_171[i];
					p3[0]=date_stamp_79_171[i-1];
					p4[0]=date_stamp_79_171[i-2];
					for (yy=0;yy<21;yy++)
					{
						p1[1]=exact_rx_pos_simulation[yy][i+1];
						p2[1]=exact_rx_pos_simulation[yy][i];
						p3[1]=exact_rx_pos_simulation[yy][i-1];
						p4[1]=exact_rx_pos_simulation[yy][i-2];
						// this will give the interpolated position of "date" at time stamp defined in Time_stamp_PM[21][2]
						pos_interpolate_12_17h[yy][1]=cubic_interpolate(p1,p2,p3,p4,date);
					}	
					break;
			}
			else
			{
					p1[0]=date_stamp_79_171[i+2];
					p2[0]=date_stamp_79_171[i+1];
					p3[0]=date_stamp_79_171[i];
					p4[0]=date_stamp_79_171[i-1];
					for (yy=0;yy<21;yy++)
					{
						p1[1]=exact_rx_pos_simulation[yy][i+2];
						p2[1]=exact_rx_pos_simulation[yy][i+1];
						p3[1]=exact_rx_pos_simulation[yy][i];
						p4[1]=exact_rx_pos_simulation[yy][i-1];
						// this will give the interpolated position of "date" at time stamp defined in Time_stamp_PM[21][2]
						pos_interpolate_12_17h[yy][1]=cubic_interpolate(p1,p2,p3,p4,date);
					}
					break;
			}
					
		}
		// interpolate for hour
		for(i=0;i<21;i++)
		{
			if (current_time>=Time_stamp_PM[i] && current_time<=Time_stamp_PM[i+1])
			{
				if(i==0)//7 to 7h30
				{
					p1[0]=Time_stamp_PM[i];
					p2[0]=Time_stamp_PM[i+1];
					p3[0]=Time_stamp_PM[i+2];
					p4[0]=Time_stamp_PM[i+3];

					p1[1]=pos_interpolate_12_17h[i][1];
					p2[1]=pos_interpolate_12_17h[i+1][1];
					p3[1]=pos_interpolate_12_17h[i+2][1];
					p4[1]=pos_interpolate_12_17h[i+3][1];
						// this will give the interpolated position of "date" at time stamp defined in Time_stamp_PM[21][2]
					desired_distance=cubic_interpolate(p1,p2,p3,p4,current_time);
				}
				else if (i==20)// 16.5h to 17h
				{
					p1[0]=Time_stamp_PM[i];
					p2[0]=Time_stamp_PM[i+1];
					p3[0]=Time_stamp_PM[i-1];
					p4[0]=Time_stamp_PM[i-2];

					p1[1]=pos_interpolate_12_17h[i][1];
					p2[1]=pos_interpolate_12_17h[i+1][1];
					p3[1]=pos_interpolate_12_17h[i-1][1];
					p4[1]=pos_interpolate_12_17h[i-2][1];
						// this will give the interpolated position of "date" at time stamp defined in Time_stamp_PM[21][2]
					desired_distance=cubic_interpolate(p1,p2,p3,p4,current_time);
					
				}
				else // from 7h30 to 16h30
				{
					p1[0]=Time_stamp_PM[i];
					p2[0]=Time_stamp_PM[i+1];
					p3[0]=Time_stamp_PM[i+2];
					p4[0]=Time_stamp_PM[i-1];

					p1[1]=pos_interpolate_12_17h[i][1];
					p2[1]=pos_interpolate_12_17h[i+1][1];
					p3[1]=pos_interpolate_12_17h[i+2][1];
					p4[1]=pos_interpolate_12_17h[i-1][1];
						// this will give the interpolated position of "date" at time stamp defined in Time_stamp_PM[21][2]
					desired_distance=cubic_interpolate(p1,p2,p3,p4,current_time);
				}
				break;
			}
		}
	}
	distance=desired_distance-*currnt_pos;
	
	if(distance>0)
		Move(distance,1);
	else
		Move(distance,0);
	
	*currnt_pos=desired_distance;
	return;

}
// return 4 closest points of "point"
/*void closest_points(float input[],float *closest_pts, float point)
{
	int i;
	for(i=0;i<21;i++)
	{
		if (point>=input[i] && point<=input[i+1] )
		{
			closest_pts[0]=input[i];
			closest_pts[1]=input[i+1];
		}
		
	}
	return;
}*/

float pos_day_171(float time){
	return 0.3224*pow(time,4) - 16.883*pow(time,3) + 336.15*pow(time,2) - 3004.8*time + 10281;
}
float pos_day_152_190(float time){
	return 0.3957*pow(time,4) - 20.914*pow(time,3) + 418.74*pow(time,2) - 3752.6*time + 12796;
}

float pos_day_139_203(float time){
 return 0.4751*pow(time,4) - 25.31*pow(time,3) + 509.39*pow(time,2) - 4578.4*time + 15589;
}

float pos_day_121_221(float time){
	return 0.6567*pow(time,4) - 35.589*pow(time,3) + 725.58*pow(time,2)  - 6585.1*time + 22505;
}	

float pos_day_113_229(float time){
	return 0.6698*pow(time,4)- 36.416*pow(time,3) + 744.21*pow(time,2)  - 6765.5*time + 23132;
}
float pos_day_106_236(float time){
	return 0.6886*pow(time,4) - 37.644*pow(time,3) + 772.66*pow(time,2)  - 7048.9*time + 24153;
}
float pos_day_94_248(float time){
	return 0.4628*pow(time,4) - 25.404*pow(time,3) + 523.11*pow(time,2)  - 4784.2*time + 16416;
}
float pos_day_87_255(float time){
	return 0.288*pow(time,4) - 15.904*pow(time,3) + 329.15*pow(time,2)  - 3023.8*time+ 10412;
}
