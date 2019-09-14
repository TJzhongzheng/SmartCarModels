#ifndef __CMT_H
#define __CMT_H

#include "gpio.h"

void CMT_PWM_QuickInit(uint16_t frequence, uint16_t duty);

void CMT_PWM_ChangeDuty(uint16_t duty);

#endif
