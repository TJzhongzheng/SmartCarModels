#include "main.h"

uint32_t timer_50ms = 0;

int main(void)
{
	/* Init Systick, Interrupt period is 50ms */
	SYSTICK_Init(50000);
	SYSTICK_ITConfig(ENABLE);
	SYSTICK_Cmd(ENABLE);
	
	/* Initialization of LEDs as Normal Indicator */
	GPIO_QuickInit(HW_GPIOA, 14, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOA, 15, kGPIO_Mode_OPP);
	
	/* PTA4 Set to input to prevent crash */
	GPIO_QuickInit(HW_GPIOA, 4, kGPIO_Mode_OPP);
	
	/* Delay init */
	DelayInit();
	
	/* Dial Switch Initialization */
	GPIO_QuickInit(HW_GPIOB, 8, kGPIO_Mode_IPU);
	GPIO_QuickInit(HW_GPIOB, 9, kGPIO_Mode_IPU);
	GPIO_QuickInit(HW_GPIOB, 10, kGPIO_Mode_IPU);
	GPIO_QuickInit(HW_GPIOB, 11, kGPIO_Mode_IPU);
	/* Read dial switch status */
	if(GPIO_ReadBit(HW_GPIOB, 8))
		sw1 = 0;
	if(GPIO_ReadBit(HW_GPIOB, 9))
		sw2 = 0;
	if(GPIO_ReadBit(HW_GPIOB, 10))
		sw3 = 0;
	if(GPIO_ReadBit(HW_GPIOB, 11))
		sw4 = 0;
	
	/* UART port initialization, baud rate 115200 */
	UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
	
	/* IO simulation of IIC. IIC Port Initialization */
	GPIO_QuickInit(IIC_SCL_PORT, IIC_SCL_PIN, kGPIO_Mode_OPP);
	GPIO_QuickInit(IIC_SDA_PORT, IIC_SDA_PIN, kGPIO_Mode_OPP);
	
	/* Initial orthogonal decoding, directional pulse type. Left speed-encoder: PB18 & PB19, Right speed-encoder：PA12 & PA13 */
	FTM_QD_QuickInit(FTM1_QD_PHA_PA12_PHB_PA13, kFTM_QD_NormalPolarity, kQD_CountDirectionEncoding);
	FTM_QD_QuickInit(FTM2_QD_PHA_PB18_PHB_PB19, kFTM_QD_NormalPolarity, kQD_CountDirectionEncoding);
	
	/* Servo Init */
	CMT_PWM_QuickInit(100,0);
	/* Setting Servo Initial duty. 0-10000 represents 0%-100% */
	CMT_PWM_ChangeDuty(1569);
	
	/* Initialize FTM port, set frequency to 5000Hz */
	FTM_PWM_QuickInit(FTM0_CH0_PC01, kPWM_EdgeAligned, 5000);
	FTM_PWM_QuickInit(FTM0_CH1_PC02, kPWM_EdgeAligned, 5000);
	FTM_PWM_QuickInit(FTM0_CH2_PC03, kPWM_EdgeAligned, 5000);
	FTM_PWM_QuickInit(FTM0_CH3_PC04, kPWM_EdgeAligned, 5000);
	/* Setting the initial duty cycle to 0. Speed of motors is 0 */
	FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH0, 0);
	FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH1, 0);
	FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH2, 0);
	FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3, 0);
	
	/* Setting Interrupt Priority */
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);		// Priorities are grouped into 2 groups
	NVIC_SetPriority(PORTA_IRQn, NVIC_EncodePriority(NVIC_PriorityGroup_2, 1, 1));
	NVIC_SetPriority(DMA2_IRQn, NVIC_EncodePriority(NVIC_PriorityGroup_2, 1, 2));
	NVIC_SetPriority(PIT0_IRQn, NVIC_EncodePriority(NVIC_PriorityGroup_2, 2, 1));
	
	/* Initialize PIT0 interrupt */
	PIT_QuickInit(HW_PIT_CH0, 5 * 1000);				// 5 ms interruption period for speed control
	PIT_CallbackInstall(HW_PIT_CH0, PIT0_CallBack);		// Registering Callback function
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, ENABLE);	// Enable channel 0 interruption
	
	/* Initialize LCD */
	LCD_Init();
	
	/* Initialize Camera */
	Cam_Init();
		
	while(1)
	{
		/* Get image */
		Cam_GetImage();
		Img_Extract(camBufferPool);
		
		/* image processing to search track */
    Track_Search();
		/* real time control */
		Real_Time();
		
		/* LCD works */
		if(sw1 == 1)
		{
			LCD_DispImg();
			print();
		}
		/* UART works */
		if(sw2 == 1)
		{
			Cam_Send();
		}
		
		/* LED Normal Indicator twinkles */
		GPIO_ToggleBit(HW_GPIOA, 14);
		
		/* Timer count resets to 0 */
		if(timer_50ms > 10000)
		{
			timer_50ms = 0;
		}

	}
}

/****************************************
Function: void print(void)
Description: LCD print
Parameters: None
Retrun:	None
****************************************/
void print(void)
{
	LCD_Printf(85, 34, "hello,my car");
}

/****************************************
Function: void Cam_Send(void)
Description: send image data through UART
Parameters: None
Retrun:	None
****************************************/
void Cam_Send(void)
{
	uint8_t color[2] = {0xfe, 0x00};
	uint8_t i, j;
	
	for(i = 0; i < CAM_H; i++)
	{
		for(j = 0; j < CAM_W; j++)
		{
			if(j == center[i])
			{
				UART_WriteByte(HW_UART0, 0);
			}
			else
			{
				UART_WriteByte(HW_UART0, color[img[i][j] & 0x01]);
			}
		}
	}
	
	UART_WriteByte(HW_UART0, 0xff);
}

/****************************************
Function: LCD_DispImg(void)
Description: LCD display image
Parameters: None
Retrun:	None
****************************************/
void LCD_DispImg(void)
{
	uint8_t i, j;
	uint16_t color[2] = {WHITE, BLACK};
	
	for(i = 0; i < CAM_H; i++)
	{
		for(j = 0; j < CAM_W; j++)
		{
			if(j == center[i])
			{
				LCD_Draw_Point(CAM_W - j - 1, CAM_H - i + 30, RED);
			}
			else
			{
				LCD_Draw_Point(CAM_W - j - 1, CAM_H - i + 30, color[img[i][j]]);
			}
		}
	}
	for(i = 1; i < 6; i++)
	{
		for(j = 0; j < 5; j++)
		{
			LCD_Draw_Point(CAM_W + j, i * 10 + 30, BLUE);
		}
	}
}

/****************************************
Function: void SysTick_Handler(void)
Description: SysTick interruption handler. Interrupt period is 50ms
Parameters: None
Retrun:	None
****************************************/
void SysTick_Handler(void)
{
	timer_50ms++;
}
