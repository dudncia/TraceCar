#ifndef __CONTROLLER_H
#define __CONTROLLER_H
#include "stm32f1xx_hal.h"
#include "motor.h"

void Controller_Init(void);


void Controller_Set_Target_Position(int32_t pos);


void pos_Controller_Update_Callback(void);
void gray_Controller_Update_Callback(float base_speed,float gray_error);
void SetBaseSpeed(void);
#endif
