#include "cmt.h"

static uint16_t FullDuty;

void CMT_PWM_QuickInit(uint16_t frequence, uint16_t duty)
{
	uint32_t clock;
	
	/* Obtaining System Board Clock Frequency */
	clock = GetClock(kBusClock);
	
	/* Enable CMT Clock */
	SIM -> SCGC4 |= SIM_SCGC4_CMT_MASK;
	
	/* Pin multiplexing */
	PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt2);
	
	/* Bus clock frequency division to generate IF clock signal */
	CMT -> PPS = CMT_PPS_PPSDIV(10);
	
	/* CMT Clock IF Clock Signal Frequency Division */
	CMT -> MSC = CMT_MSC_CMTDIV(0);
	
	/* Calculate counting values based on frequencies */
	FullDuty = clock / (80 * frequence);

	CMT -> CMD1 = (uint8_t)((duty >> 8) & CMT_CMD1_MB_MASK);
	CMT -> CMD2 = (uint8_t)(duty & CMT_CMD2_MB_MASK);
	CMT -> CMD3 = (uint8_t)(((FullDuty - duty) >> 8) & CMT_CMD3_SB_MASK);
	CMT -> CMD4 = (uint8_t)((FullDuty-duty) & CMT_CMD4_SB_MASK);

	/* Baseband mode enablement */
	CMT -> MSC &= ~CMT_MSC_FSK_MASK;
	CMT -> MSC |= CMT_MSC_BASE_MASK;
	
	/* Carrier Modulated Wave Enablation */
	CMT -> MSC |= CMT_MSC_MCGEN_MASK;
	
	/* Flip level */
	CMT -> OC |= CMT_OC_CMTPOL_MASK;
	
	/* Output enable */
	CMT -> OC |= CMT_OC_IROPEN_MASK;
}

/****************************************
Function：void CMT_PWM_ChangeDuty(uint16_t duty)
Description：Change CMT PWM ouput duty to Servo motor. It is the most basic function to change the angle of Servo motor.
Parameters:	duty  /0-10000 represents 0%-100%/
Retrun:	None
****************************************/
void CMT_PWM_ChangeDuty(uint16_t duty)
{
	duty = duty * FullDuty / 10000;
	
	CMT -> CMD1 = (uint8_t)((duty >> 8) & CMT_CMD1_MB_MASK);
	CMT -> CMD2 = (uint8_t)(duty & CMT_CMD2_MB_MASK);
	CMT -> CMD3 = (uint8_t)(((FullDuty-duty) >> 8) & CMT_CMD3_SB_MASK);
	CMT -> CMD4 = (uint8_t)((FullDuty-duty) & CMT_CMD4_SB_MASK);
}
