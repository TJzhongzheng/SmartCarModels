#ifndef __CAM_INIT_H_
#define __CAM_INIT_H_

#include "dma.h"
#include "ov7725.h"

/* Image size captured by camera */
#define IMAGE_SIZE	1

/* Width 80, Height 60 */
#if (IMAGE_SIZE == 0)
#define CAM_W	(80)
#define CAM_H	(60)

/* Width 160, Height 120 */
#elif (IMAGE_SIZE == 1)
#define CAM_W	(160)
#define CAM_H	(120)

/* Width 240, Height 180 */
#elif (IMAGE_SIZE == 2)
#define CAM_W	(240)
#define CAM_H	(180)
	
#else
#error "Image Size Not Support!"
#endif

#define CAMERA_DMA_CH		HW_DMA_CH2
#define BITBAND(Reg, Bit)	((uint32_t volatile*)(0x22000000 + (32*((uint32_t) & (Reg) - (uint32_t)0x20000000)) + (4 * ((uint32_t)(Bit)))))

/* Define the state machine */
typedef enum
{
	IMG_NOTINIT = 0,
	IMG_FINISH,	// finish
	IMG_FAIL,	// fail
	IMG_GATHER,	// collecting
	IMG_START,	// start
	IMG_STOP,	// stop
}CamStatus;


extern uint8_t *camBufferPool;
extern uint8_t img[CAM_H][CAM_W];

int SCCB_Init(void);

void OV_ISR(uint32_t index);

void Cam_DMA(void);

void Cam_Init(void);

void Cam_PortInit(void);

void Cam_GetImage(void);

void Cam_WaitGetImage(void);

void Img_Extract(uint8_t *src);

#endif
