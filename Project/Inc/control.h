#ifndef __CONTROL_H_
#define __CONTROL_H_

#include "cmt.h"
#include "ftm.h"
#include "img_processing.h"

#define SERVO_MIDDLE	1569 //1569 
#define SERVO_LEFT		SERVO_MIDDLE + 140
#define SERVO_RIGHT		SERVO_MIDDLE - 140 

typedef enum
{
	car_left,	// left motor
	car_right,	// right motor
}SpeedControlType;

typedef struct
{
	float Kp;
	float Ki;
	float Kd;
}PID_Controller;

extern uint8_t sw1;
extern uint8_t sw2;
extern uint8_t sw3;
extern uint8_t sw4;

extern uint32_t QD_value_left;
extern uint32_t QD_value_right;

extern volatile int16_t center_ave;
extern volatile int16_t center_range;
extern volatile int16_t speed_ept;

void Real_Time(void);

void Get_MidlineError(void);

void Servo_Control(void);

void PIT0_CallBack(void);

void Motor_Control(void);

void Car_PWM_ChangeDuty(int32_t duty, SpeedControlType dir);

#endif
