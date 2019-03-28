#define ADC_Stable_Threshold 30
#define ADC_START  	1<<3
#define ADC_PWR  		0x80
#define ADC_Flag 		0x10

#define P1_0_ADC_FUNCTION 	0x01
#define P1_1_ADC_FUNCTION 	0x02
#define P1_2_ADC_FUNCTION 	0x04

#define ADC_FLAG     	0x10
#define ADC_SPEEDLL 0x00
//#define P1_0_INPUT_P1M0 0x00
//#define P1_0_INPUT_P1M0 0x01
#define sunlight_ADC_Threshold 250 //820

extern int adc_data;

//extern int  calib_mode=1;
unsigned int ADC_GetResult(unsigned char ch);
void ADC_Init(void);