#ifndef __IMG_PROCESSING_H_
#define __IMG_PROCESSING_H_

#include "cam_init.h"

#define P_BLACK		1				// BlACK
#define P_WHITE		0				// WHITE
#define IMG_CENTER	CAM_W / 2 - 3	// The midline of the image


typedef enum{
	STRAIGHT, 
	DEFAULT
}Road_Type;

extern volatile Road_Type road_type;

extern volatile uint8_t center[CAM_H];
extern volatile uint8_t left[CAM_H];				
extern volatile uint8_t right[CAM_H];				

extern volatile uint8_t start_search;	
extern volatile uint8_t end_search;	


void Track_Search(void);


#endif
