#include "cam_init.h"

/* Chip pins definition */
#define BOARD_OV7725_PCLK_PORT		HW_GPIOC
#define BOARD_OV7725_PCLK_PIN		(16)
#define BOARD_OV7725_VSYNC_PORT		HW_GPIOA
#define BOARD_OV7725_VSYNC_PIN		(17)
#define BOARD_OV7725_HREF_PORT		HW_GPIOA
#define BOARD_OV7725_HREF_PIN		(16)
#define BOARD_OV7725_DATA_OFFSET	(8)

uint8_t bufferPool1[CAM_H * (CAM_W / 8)];
uint8_t bufferPool2[CAM_H * (CAM_W / 8)];
uint8_t bufferSelect = 1;
uint8_t *camBufferPool = bufferPool1;	// Camera image storage address
uint8_t img[CAM_H][CAM_W];				//Image storage array
uint8_t *camBuffer;						// Image data pointer

/* Image acquisition status flag */
volatile CamStatus camImgFlag;

/****************************************
Function: int SCCB_Init(void)
Description: Set camera OV7725 image size
Parameters: None
Retrun:	0 successful
				1 failed
****************************************/
int SCCB_Init(void)
{
	int r;
	r = ov7725_probe();
	
	if(r)
	{
		return 1;
	}
	
	r = ov7725_set_image_size((ov7725_size)IMAGE_SIZE);
	
	if(r)
	{
		printf("OV7725 Set Image Error!\r\n");
		return 1;
	}
	
	return 0;
}

/****************************************
Function: void OV_ISR(uint32_t index)
Description: Image field interruption
Parameters: None
Retrun:	None
****************************************/
void OV_ISR(uint32_t index)
{
	if(index & (1 << BOARD_OV7725_VSYNC_PIN))
	{
		/* Judging whether the image field ends or begins */
		if(camImgFlag == IMG_START)
		{
			camImgFlag = IMG_GATHER;	// Label image acquisition
			// disable_irq(PORTA_IRQn);
			PORTC -> ISFR = 1 << BOARD_OV7725_PCLK_PIN;					// reset PCLK flag
			DMA_EnableRequest(HW_DMA_CH2);								// Enable channel CH2 hardware request
			PORTC -> ISFR = 1 << BOARD_OV7725_PCLK_PIN;					// reset PCLK flag
			DMA_SetDestAddress(HW_DMA_CH2, (uint32_t)camBufferPool);	// reset transmission address
		}
		else if(camImgFlag == IMG_FINISH)
		{
			return;
		}
		else
		{
			disable_irq(PORTA_IRQn);	// disable PTA interruption
			camImgFlag = IMG_FAIL;		// Label image acquisition failure

		}
	}
}

/****************************************
Function: void Cam_DMA(void)
Description: DMA interruption service function
Parameters: None
Retrun:	None
****************************************/
void Cam_DMA(void)
{
	camImgFlag = IMG_FINISH;
//	disable_irq(PORTA_IRQn);
	DMA_DisableRequest(HW_DMA_CH2);
	DMA_IRQ_CLEAN(CAMERA_DMA_CH);	// Clean up DMA interruption flag
}

/****************************************
Function: void Cam_Init(void)
Description: Initialize Camera
Parameters: None
Retrun:	None
****************************************/
void Cam_Init(void)
{
	camBuffer = (uint8_t*)&camBufferPool;
	if(SCCB_Init())	// Detect camera device
	{
		printf("No Device Found!");
	}
	Cam_PortInit();
	DelayMs(10);
}

/****************************************
Function: void Cam_PortInit(void)
Description: Initialize Camera device pins. Internal call
Parameters: None
Retrun:	None
****************************************/
void Cam_PortInit(void)
{
	/* Initialization of Camera Data Acquisition Port */
	for(uint8_t i = 0; i < 8; i++)
	{
		GPIO_QuickInit(HW_GPIOC, BOARD_OV7725_DATA_OFFSET + i, kGPIO_Mode_IFT);
	}
	
	/* Field Interrupt, Line Interrupt, Pixel Interrupt Settings */
	GPIO_QuickInit(BOARD_OV7725_PCLK_PORT, BOARD_OV7725_PCLK_PIN, kGPIO_Mode_IPU);		// PCLK Set to pull-up input
	GPIO_QuickInit(BOARD_OV7725_VSYNC_PORT, BOARD_OV7725_VSYNC_PIN, kGPIO_Mode_IPD);	// VSYNC Set to drop-down input

	/* Registering Callback */
	GPIO_CallbackInstall(BOARD_OV7725_VSYNC_PORT, OV_ISR);

	/* Rising edge triggers interruption */
	GPIO_ITDMAConfig(BOARD_OV7725_VSYNC_PORT, BOARD_OV7725_VSYNC_PIN, kGPIO_IT_RisingEdge, true);
	/* Drop Edge Triggers DMA Interruption */
	GPIO_ITDMAConfig(BOARD_OV7725_PCLK_PORT, BOARD_OV7725_PCLK_PIN, kGPIO_DMA_FallingEdge, true);

	/* DMA structure */
	DMA_InitTypeDef DMA_InitStruct1 = {0};

	/* Set DMA */
	DMA_InitStruct1.chl = CAMERA_DMA_CH;					// HW_DMA_CH2
	DMA_InitStruct1.chlTriggerSource = PORTC_DMAREQ;
	DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
	DMA_InitStruct1.minorLoopByteCnt = 1;
	DMA_InitStruct1.majorLoopCnt = (CAM_H * (CAM_W / 8));	// Number of main cycles

	DMA_InitStruct1.sAddr = (uint32_t)&PTC -> PDIR + BOARD_OV7725_DATA_OFFSET / 8;
	DMA_InitStruct1.sLastAddrAdj = 0;
	DMA_InitStruct1.sAddrOffset = 0;
	DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
	DMA_InitStruct1.sMod = kDMA_ModuloDisable;

	DMA_InitStruct1.dAddr = (uint32_t)camBuffer;
	DMA_InitStruct1.dLastAddrAdj = 0;
	DMA_InitStruct1.dAddrOffset = 1;
	DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
	DMA_InitStruct1.dMod = kDMA_ModuloDisable;

	/* Initialize the DMA module */
	DMA_Init(&DMA_InitStruct1);
	DMA_ITConfig(HW_DMA_CH2, kDMA_IT_Major, true);			// Setting up DMA interrupt
	DMA_CallbackInstall(HW_DMA_CH2, Cam_DMA);
	DMA_DisableRequest(HW_DMA_CH2);
}

/****************************************
Function: void Cam_GetImage(void)
Description: Image Acquisition Function. The collected data is stored in the initialized array.
Parameters: None
Retrun:	None
****************************************/
void Cam_GetImage(void)
{
	camImgFlag = IMG_START;
	PORTA -> ISFR = ~0;				// Clean up interruption flag
	enable_irq(PORTA_IRQn);			// enable PTA interruption
	while(camImgFlag != IMG_FINISH)	// Waitting for image data acquisition completed
	{
		/* if error, re collect image */
		if(camImgFlag == IMG_FAIL)
		{
			camImgFlag = IMG_START;
			PORTA -> ISFR = ~0;
			enable_irq(PORTA_IRQn);
		}
	}
}

/****************************************
Function：void Img_Extract(uint8_t *src)
Description: Extract camera image data to array img[]
Parameters: None
Retrun:	None
****************************************/
void Img_Extract(uint8_t *src)
{
	uint8_t color[2] = {0, 1};	// In the image, 0 shows white color，1 shows black color 
	uint8_t tmpsrc;
	uint8_t i, j;
	uint8_t line;

	for(i = 0; i < CAM_H; i++)
	{
		for(j = 0; j < CAM_W / 8; j++)
		{
			line = j * 8;
			tmpsrc = *src++;
			img[i][line + 0] = color[(tmpsrc >> 0) & 0x01];
			img[i][line + 1] = color[(tmpsrc >> 1) & 0x01];
			img[i][line + 2] = color[(tmpsrc >> 2) & 0x01];
			img[i][line + 3] = color[(tmpsrc >> 3) & 0x01];
			img[i][line + 4] = color[(tmpsrc >> 4) & 0x01];
			img[i][line + 5] = color[(tmpsrc >> 5) & 0x01];
			img[i][line + 6] = color[(tmpsrc >> 6) & 0x01];
			img[i][line + 7] = color[(tmpsrc >> 7) & 0x01];
		}
	}
}
