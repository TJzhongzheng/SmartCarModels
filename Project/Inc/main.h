#ifndef __MAIN_H_
#define __MAIN_H_

#include "control.h"
#include "pit.h"
#include "lcd.h"
#include "systick.h"
#include "uart.h"

extern uint32_t timer_50ms;

void print(void);

void Cam_Send(void);

void LCD_DispImg(void);

#endif
