#include "lcd.h"
#include "SimSun6x12.h"

bool display_on_LCD = 1;

static font_t _gFontTbl[] =
{
	{"SimSun", FONT_SimSun6x12_XSize, FONT_SimSun6x12_YSize, FONT_SimSun6x8}, 
};

/****************************************
Function：LCD_Init(void)
Description: LCD_Init
Parameters: None
Retrun:	None
Details: LCD_SCK			PTB21
				 LCD_SDA			PTB22
				 LCD_A0				PTE9
				 LCD_RESET		PTE8
				 LCD_CS0			PTB20
****************************************/
void LCD_Init(void)
{
	/* Init GPIO */
	GPIO_QuickInit(HW_GPIOB, 20, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOE, 8, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOE, 9, kGPIO_Mode_OPP);
	
	/* Init SPI2, Channel speed is 25000000Hz */
	SPI_QuickInit(SPI2_SCK_PB21_SOUT_PB22_SIN_PB23, kSPI_CPOL0_CPHA0, 25000000);
	
	/* Enable FIFO send*/
	SPI_EnableTxFIFO(HW_SPI2, ENABLE);
	
	/* Hardware reset LCD */
	LCD_RESET_LOW;
	DelayMs(100);
	LCD_RESET_HIGH;
	DelayMs(100);
	
	/* LCD initialization sequence */
	/* Exit Sleep Mode */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x11, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	DelayMs(20);
	
	/* Set the default Gamma value */
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x26, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x04, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Set frame rate */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0xB1, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x0E, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x10, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Set VRH1[4: 0]&VC[2: 0]forVCI1&GVDD */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0xC0, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x08, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x00, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Set BT[2: 0]forAVDD&VCL&VGH&VGL */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0xC1, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x05, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Set VMH[6: 0]&VML[6: 0]forVOMH&VCOML */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0xC5, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x38, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x40, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Setting the color format */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x3A, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x05, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Setting Storage Channel Control */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x36, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0xC8, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Setting Column Address */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x2A, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x00, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x00, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x00, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x7F, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Setting Page Address */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x2B, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x00, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x00, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x00, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x9F, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Setting Display Flip Control */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0xB4, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x00, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Enabling Gamma Bit */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0xF2, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x01, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Setting Positive Gamma Correction Parameters */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0xE0, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x3F, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x22, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x20, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x30, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x29, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x0C, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x4E, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0xB7, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x3C, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x19, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x22, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x1E, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x02, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x01, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x00, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Setting Negative Gamma Correction Parameters */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0xE1, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x00, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x1B, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x1F, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x0F, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x16, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x13, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x31, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x84, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x43, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x06, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x1D, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x21, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x3D, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x3E, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x3F, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Open */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x29, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Set whole background color white */
	LCD_Display_BackgroundColor(WHITE);
}

/****************************************
Function：LCD_Display_BackgroundColor(uint16_t color)
Description：Set LCD BackgroundColor
Parameters: color
Retrun:	None
****************************************/
void LCD_Display_BackgroundColor(uint16_t color)
{
	uint8_t i, j;
	
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x2C, HW_SPI_CS0, kSPI_PCS_KeepAsserted);
	LCD_A0_HIGH;
	
	for(i = 0; i < 160; i++)
	{
		for(j = 0; j < 128; j++)
		{
			SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, color >> 8, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
			SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, color, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
		}
	}
}

/****************************************
Function: LCD_Draw_Point(uint8_t x, uint8_t y, uint16_t color)
Description: LCD draw point
Parameters:	x				Abscissa
						y				Ordinate
						color		16-bit RGB color
Retrun:	None
****************************************/
void LCD_Draw_Point(uint8_t x, uint8_t y, uint16_t color)
{
	/* Setting Column Address */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x2A, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, x, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, x, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Setting Page Address */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x2B, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, y, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, y, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	
	/* Write data */
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x2C, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	LCD_A0_HIGH;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, color >> 8, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, color, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
}

/****************************************
Function: LCD_Sleep(void)
Description: LCD Sleep
Parameters: None
Retrun:	None
****************************************/
void LCD_Sleep(void)
{
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x28, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x10, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
}

/****************************************
Function: LCD_WakeUp(void)
Description: LCD wakeup
Parameters: None
Retrun:	None
****************************************/
void LCD_WakeUp(void)
{
	LCD_A0_LOW;
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x11, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0x29, HW_SPI_CS0, kSPI_PCS_ReturnInactive);
}

/****************************************
Function: LCD_DispChar(char c, int x, int y, const char *pdata, int font_xsize, int font_ysize, int fcolor, int bcolor)
Description: LCD display character
Parameters: 
Retrun:	None
****************************************/
void LCD_DispChar(char c, int x, int y, const char *pdata, int font_xsize, int font_ysize, int fcolor, int bcolor)
{
	uint8_t j, pos, t;
	uint8_t temp;
	uint8_t XNum;
	uint32_t base;
	XNum = (font_xsize / 8) + 1;
	
	if(font_ysize % 8 == 0)
	{
		XNum--;
	}
	if(c < ' ')
	{
		return;
	}
	
	c = c - ' ';
	base =(c * XNum * font_ysize);

	for(j = 0; j < XNum; j++)
	{
		for(pos = 0; pos < font_ysize; pos++)
		{
			temp = (uint8_t)pdata[base + pos + j * font_ysize];
			if(j < XNum)
			{
				for(t = 0; t < 8; t++)
				{
					if((temp >> t) & 0x01)
					{
						LCD_Draw_Point(x + t, y + pos, fcolor);
					}
					else
					{
						LCD_Draw_Point(x + t, y + pos, bcolor);
					}
				}
			}
			else
			{
				for(t = 0; t < font_xsize % 8; t++)
				{
					if((temp >> t) & 0x01)
					{
						LCD_Draw_Point(x + t, y + pos, fcolor);
					}
					else
					{
						LCD_Draw_Point(x + t, y + pos, bcolor);
					}
				}
			}
		}
		x += 8;
	}
}

/****************************************
Function: LCD_Printf(int x, int y, const char *format, ...)
Description: LCD display Character string
Parameters: x						
						y						
						format			
Retrun:	number of chars string
****************************************/
int LCD_Printf(int x, int y, const char *format, ...)
{
	int chars;
	int i;
	__va_list ap;
	char printbuffer[64];
	va_start(ap, format);
	chars = vsprintf(printbuffer, format, ap);
	va_end(ap);
	
	for(i = 0; i < chars; i++)
	{
		LCD_DispChar(printbuffer[i], x + i * _gFontTbl[0].x_size, y, _gFontTbl[0].data, _gFontTbl[0].x_size, _gFontTbl[0].y_size, 0xFFFF, 0x0000);
	}
	return chars;
}
