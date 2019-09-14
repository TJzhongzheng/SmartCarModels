#include "ov7725.h"

#define OV7725_DEBUG		1

#if (OV7725_DEBUG == 1)
#include <stdio.h>
#define OV7725_TRACE	printf
#else
#define OV7725_TRACE	(...)
#endif

struct ov7725_reg
{
	uint8_t addr;
	uint8_t val;
};

struct
{
	uint32_t i2c_instance;
	uint8_t addr;
	uint32_t h_size;
	uint32_t v_size;
}h_ov7725;

static uint8_t ov7725_chip_addr_table[] = {0x42};
static const struct ov7725_reg default_reg_value[] =
{
	/* Image frame rate */
	// 50fps
	{OV7725_COM4,		0xC1},
	{OV7725_CLKRC,		0x02},
	// 75fps
	// {OV7725_COM4,		0x41},
	// {OV7725_CLKRC,		0x00},
	// 112fps
	// {OV7725_COM4,		0x81},
	// {OV7725_CLKRC,		0x00},
	{OV7725_COM2,		0x01},		
	{OV7725_COM3,		0xD0},		
	{OV7725_COM7,		0x40},		
	{OV7725_HSTART,		0x3F},		
	{OV7725_HSIZE,		0x50},		
	{OV7725_VSTRT,		0x03},		
	{OV7725_VSIZE,		0x78},		
	{OV7725_HREF,		0x00},		
	{OV7725_SCAL0,		0x0A},		
	{OV7725_AWB_Ctrl0,	0xE0},		
	{OV7725_DSPAuto,	0xFF},		
	{OV7725_DSP_Ctrl2,	0x0C},		
	{OV7725_DSP_Ctrl3,	0x00},		
	{OV7725_DSP_Ctrl4,	0x00},		
	{OV7725_EXHCH,		0x00},		
	{OV7725_GAM1,		0x0C},		
	{OV7725_GAM2,		0x16},		
	{OV7725_GAM3,		0x2A},		
	{OV7725_GAM4,		0x4E},		
	{OV7725_GAM5,		0x61},		
	{OV7725_GAM6,		0x6F},		
	{OV7725_GAM7,		0x7B},		
	{OV7725_GAM8,		0x86},		
	{OV7725_GAM9,		0x8E},		
	{OV7725_GAM10,		0x97},		
	{OV7725_GAM11,		0xA4},		
	{OV7725_GAM12,		0xAF},		
	{OV7725_GAM13,		0xC5},		
	{OV7725_GAM14,		0xD7},		
	{OV7725_GAM15,		0xE8},		
	{OV7725_SLOP,		0x20},		
	{OV7725_LC_RADI,	0x00},		
	{OV7725_LC_COEF,	0x13},		
	{OV7725_LC_XC,		0x08},		
	{OV7725_LC_COEFB,	0x14},		
	{OV7725_LC_COEFR,	0x17},		
	{OV7725_LC_CTR,		0x05},		
	{OV7725_BDBase,		0x99},		
	{OV7725_BDMStep,	0x03},		
	{OV7725_SDE,		0x04},		
	{OV7725_BRIGHT,		0x00},		
	{OV7725_CNST,		24	},	// Parameters for Controlling Black-White Contrast. Modifying this parameter to improve the effect of binarization.
	{OV7725_SIGN,		0x06},		
	{OV7725_UVADJ0,		0x11},		
	{OV7725_UVADJ1,		0x02},		
};

int ov7725_probe(void)
{
	int i, j;
	int r;
	for(i = 0; i < ARRAY_SIZE(ov7725_chip_addr_table); i++)
	{
		if(i2c_write_byte(ov7725_chip_addr_table[i], OV7725_COM7, 0x80) == 0)
		{
			/* Search for device */
			OV7725_TRACE("devicefoundaddr:0x%X\r\n", ov7725_chip_addr_table[i]);
			
			/* Reset */
			DelayMs(3);
			for(j = 0; j < ARRAY_SIZE(default_reg_value); j++)
			{
				r = i2c_write_byte(ov7725_chip_addr_table[i], default_reg_value[j].addr, default_reg_value[j].val);
				DelayMs(3);
				if(r)
				{
					OV7725_TRACE("device[addr:0x%X]regiser[addr:0x%X]writeerror!\r\n", ov7725_chip_addr_table[i], default_reg_value[j].addr);
				}
				else
				{
					// OV7725_TRACE("device[addr:0x%X]regiser[addr:0x%X]writeOK!\r\n", ov7725_chip_addr_table[i], default_reg_value[j].addr);
				}

			}
			h_ov7725.addr = ov7725_chip_addr_table[i];
			h_ov7725.h_size = 80;
			h_ov7725.v_size = 60;
			return 0;
		}
	}
	OV7725_TRACE("No Sccb Device Found!\r\n");
	return 1;
}

int ov7725_set_image_size(ov7725_size size)
{
	int r;
	r = 0;
	switch(size)
	{
		case H_60_W_80:
			h_ov7725.h_size = 80;
			h_ov7725.v_size = 60;
			r += i2c_write_byte(h_ov7725.addr, OV7725_HOutSize, 0x14);
			r += i2c_write_byte(h_ov7725.addr, OV7725_VOutSize, 0x1E);
			break;
		case H_120_W_160:
			h_ov7725.h_size = 160;
			h_ov7725.v_size = 120;
			r += i2c_write_byte(h_ov7725.addr, OV7725_HOutSize, 0x28);
			r += i2c_write_byte(h_ov7725.addr, OV7725_VOutSize, 0x3C);
			break;
		case H_180_W_240:
			h_ov7725.h_size = 240;
			h_ov7725.v_size = 180;
			r += i2c_write_byte(h_ov7725.addr, OV7725_HOutSize, 0x3C);
			r += i2c_write_byte(h_ov7725.addr, OV7725_VOutSize, 0x5A);
			break;
		case H_240_W_320:
			h_ov7725.h_size = 320;
			h_ov7725.v_size = 240;
			r += i2c_write_byte(h_ov7725.addr, OV7725_HOutSize, 0x50);
			r += i2c_write_byte(h_ov7725.addr, OV7725_VOutSize, 0x78);
			break;
		default:
			r++;
			OV7725_TRACE("Wrong Param In Func:ov7725_set_image_size\r\n");
			break;
	}
	return r;
}
