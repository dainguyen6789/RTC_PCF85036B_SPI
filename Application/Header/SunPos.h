// This file is available in electronic form at http://www.psa.es/sdg/sunpos.htm

#ifndef __SUNPOS_H
#define __SUNPOS_H


// Declaration of some constants 
#define pi    3.14159//265358979323846
#define twopi (2*pi)
#define rad   (pi/180)
#define dEarthMeanRadius     6371.01	// In km
#define dAstronomicalUnit    149597890	// In km

struct cTime
{
	int iYear;
	int iMonth;
	int iDay;
	double dHours;
	double dMinutes;
	double dSeconds;
};

struct cLocation
{
	double dLongitude;
	double dLatitude;
};

struct cSunCoordinates
{
	double dZenithAngle;
	double dAzimuth;
};


double sunpos(struct cTime udtTime,struct cLocation udtLocation, struct cSunCoordinates *udtSunCoordinates);
struct point
{
	float x;
	float y;
};
#endif

//declaration of location to use in both files: main.c and BipolarMotor.c
extern struct cLocation location;

int FindClosestSamedayCalibTime(int *input_calibration_bool_array,int current_time_stamp_position);
