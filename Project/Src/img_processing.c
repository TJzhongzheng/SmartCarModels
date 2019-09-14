#include "img_processing.h"

/* Track width in the camera image */
const uint8_t road_width[CAM_H] =
{
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	13,	15,	15,	17,	17,	19,	19,	21,	21,	23,
	24,	24,	26,	26,	28,	28,	30,	30,	31,	32,
	33,	34,	36,	36,	38,	38,	40,	40,	42,	42,
	44,	44,	45,	46,	47,	48,	49,	50,	51,	52,
	53,	54,	55,	55,	57,	57,	58,	59,	60,	61
};

/* Track type variable */
volatile Road_Type road_type = DEFAULT;	
volatile Road_Type road_type_old = DEFAULT;

/* Track sidelines and Track center variable and search range */
volatile uint8_t center[CAM_H];				
volatile uint8_t left[CAM_H];				
volatile uint8_t right[CAM_H];				
volatile uint8_t start_search = CAM_H - 7 ;	
volatile uint8_t end_search = CAM_H - 50;	
volatile uint8_t left_corner = 0;			
volatile uint8_t right_corner = 0;		

/* Flags */
volatile uint8_t search_flag = 0;			
volatile uint8_t cross_center_flag = 0;		
volatile uint8_t corner_flag = 0;			
volatile uint8_t in_circle_flag = 0;		
volatile uint8_t out_circle_flag = 0;	

/* Counters */
volatile uint8_t park_cnt = 0;				// Park
volatile uint8_t in_circle_cnt = 0;			// Enter circle
volatile uint8_t out_circle_cnt = 0;		// Leave circle

/****************************************
Function：void Track_Search(void)
Description：Search track sidelines and center
Parameters: None
Retrun:	None
****************************************/
void Track_Search(void)
{
	
	/* Initialization */

	/* Search track sidelines and center */
	
	/* Judge Track type */

}
