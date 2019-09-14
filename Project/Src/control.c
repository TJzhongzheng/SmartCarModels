#include "control.h"

uint8_t sw1 = 1;	// Enable LCD
uint8_t sw2 = 1;	// Enable UART
uint8_t sw3 = 1;
uint8_t sw4 = 1;

/* use to control speed */
PID_Controller motor_pid;
volatile int32_t motorDuty_left;
volatile int32_t motorDuty_right;
volatile uint32_t motorDuty_ave;
uint32_t QD_value_left = 0;
uint32_t QD_value_right = 0;

volatile int16_t speed_ept = 0;
volatile int16_t speed_now = 0;
volatile int16_t speed_error = 0;
volatile int16_t speed_errorD1 = 0;
volatile int16_t speed_errorD2 = 0;
volatile int16_t speed_add = 1000;

/* use to control direction */
PID_Controller servo_pid;
volatile uint16_t servoDuty = 0;
volatile uint8_t diff_turn = 22;	// Coefficient of Differential speed

volatile int16_t center_ave = IMG_CENTER;
volatile int16_t center_ave_old = IMG_CENTER;
volatile int16_t center_ave_error;
volatile int16_t center_range = 0;
volatile int16_t center_ave_speed = 0;


/****************************************
Function：void Real_Time(void)
Description：real time control
Parameters: None
Retrun:	None
****************************************/
void Real_Time(void)
{
	/* Servo_Control */
	/* PIT0_CallBack */
}

/****************************************
Function：void Servo_Control(void)
Description：Control Servo. The control strategy is not complete!!!!!!
Parameters: None
Retrun:	None
****************************************/
void Servo_Control(void)
{	
  servo_pid.Kp = 0.0;
	servo_pid.Kd = 0.0; 

	servoDuty = SERVO_MIDDLE + servo_pid.Kp * center_ave + servo_pid.Kd * (center_ave - center_ave_old);
	
	if(servoDuty > SERVO_LEFT)
	{
		servoDuty = SERVO_LEFT;
	}
	if(servoDuty < SERVO_RIGHT)
	{
		servoDuty = SERVO_RIGHT;
	}
	
	CMT_PWM_ChangeDuty(servoDuty);
}

/****************************************
Function：void PIT0_CallBack(void)
Description：PIT interruption function to get real time speed saved in speed_now.
						 Using speed_now we can call function Motor_Control() to control speed.
Parameters: None
Retrun:	None
****************************************/
void PIT0_CallBack(void)
{
	uint8_t dir;
	
	FTM_QD_GetData(HW_FTM2, &QD_value_left, &dir);
	FTM_QD_GetData(HW_FTM1, &QD_value_right, &dir);
	
	/* FTM clean up counters */
	FTM_QD_ClearCount(HW_FTM1);
	FTM_QD_ClearCount(HW_FTM2);
	
	if(QD_value_left != 0)
	{
		QD_value_left = 65536 - QD_value_left;
	}
	
	/* Reversal limit */
	if(QD_value_left > 1000)
	{
		QD_value_left = 0;
	}
	if(QD_value_right > 1000)
	{
		QD_value_right = 0;
	}
	
	speed_now = (QD_value_left + QD_value_right) / 2;
	
//	send_data(QD_value_left,QD_value_right,speed_now,speed_error);
	/* Using speed_now we can call function Motor_Control() to control speed */
	Motor_Control();
}

/****************************************
Function：void Motor_Control(void)
Description：Conrol Motor. The control strategy is not complete!!!!!!
Parameters: None
Retrun:	None
****************************************/
void Motor_Control(void)
{
	if(sw1 == 1 || sw2 == 1)
	{
		return;
	}
		
	motorDuty_left = 2000;
	motorDuty_right = 2000;
	
	/* limit */
	if(motorDuty_left > 10000)
	{
		motorDuty_left = 10000;
	}
	else if(motorDuty_left < -10000)
	{
		motorDuty_left = -10000;
	}
	if(motorDuty_right > 10000)
	{
		motorDuty_right = 10000;
	}
	else if(motorDuty_right < -10000)
	{
		motorDuty_right = -10000;
	}
	Car_PWM_ChangeDuty(motorDuty_left, car_left);
	Car_PWM_ChangeDuty(motorDuty_right, car_right);
}

/****************************************
Function：void Car_PWM_ChangeDuty(int32_t duty, SpeedControlType dir)
Description：change PWM ouput duty to car motor. It is the most basic function to change the speed of motor.
Parameters:	duty
						dir    car_left / car_right // means that to change the speed of the left motor / the right motor
Retrun:	None
****************************************/
void Car_PWM_ChangeDuty(int32_t duty, SpeedControlType dir)
{
	uint8_t temp;
	temp = duty >= 0 ? 1 : 0;
	duty = __fabs(duty);
	if(dir == car_left)
	{
		if(temp == 1)
		{
			FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH1, 0);
			FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH0, duty);
		}
		else
		{
			FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH0, 0);
			FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH1, duty);
		}
	}
	else
	{
		if(temp == 1)
		{
			FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3, 0);
			FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH2, duty);
		}
		else
		{
			FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH2, 0);
			FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3, duty);
		}
	}
}



