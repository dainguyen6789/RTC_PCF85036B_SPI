
//
#define num_of_time_stamp 11
#define num_of_day_stamp 	10
#define Dai_to_JP_Zero 179;
float date_declination_mapping[num_of_day_stamp]={23.4400,21.0960,18.7520,14.0640,11.7200,9.3760,7.0320,4.6880,2.3440,0.0000};

float RX_pos[num_of_time_stamp][num_of_day_stamp]=
{
	{-70.88,-64.14,-57.32,-43.59,-36.61,-29.59,-22.37,-15.12,-7.63,0.00},
	{-71.38,-64.61,-57.76,-43.91,-36.95,-29.82,-22.54,-15.23,-7.70,0.00},
	{-72.94,-66.07,-59.09,-44.98,-37.80,-30.56,-23.12,-15.62,-7.92,0.00},
	{-75.69,-68.60,-61.41,-46.82,-39.40,-31.83,-24.11,-16.31,-8.29,0.00},
	{-79.80,-72.39,-64.90,-49.61,-41.81,-33.77,-25.61,-17.33,-8.85,0.00},
	{-85.48,-77.82,-69.89,-53.61,-45.20,-36.65,-27.84,-18.76,-9.54,0.00},
	{-93.49,-85.30,-76.91,-59.25,-50.12,-40.64,-30.97,-20.94,-10.68,0.00},
	{-104.22,-95.65,-86.67,-67.37,-57.21,-46.55,-35.57,-24.10,-12.21,0.00},
	{-118.82,-109.96,-100.47,-79.29,-67.62,-55.49,-42.47,-28.94,-14.62,0.00},
	{-138.85,-130.17,-120.40,-97.58,-84.24,-69.67,-53.95,-37.07,-18.80,0.00},
	{-165.88,-158.21,-149.23,-126.45,-111.78,-95.01,-75.17,-52.58,-27.24,0.00}
};
// Corrected sun time
float  Time_stamp_PM[num_of_time_stamp]={
11.94,
12.44,
12.94,
13.44,
13.94,
14.44,
14.94,
15.44,
15.94,
16.44,
16.94,
};
struct point
{
	float x;
	float y;
};