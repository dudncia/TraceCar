#include "bsp.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern Motor_t B_right_motor;
extern Motor_t A_left_motor;
/**
 * @brief: 初始化外设
 */
void BSP_Init(void)
{
    //初始化电机
    Motor_Init(&A_left_motor, &htim4, &htim3, TIM_CHANNEL_4,
               AIN1_Moter_GPIO_Port, AIN1_Moter_Pin,
               AIN2_Moter_GPIO_Port, AIN2_Moter_Pin);
    Motor_Init(&B_right_motor, &htim2, &htim3, TIM_CHANNEL_3,
               BIN1_Moter_GPIO_Port, BIN1_Moter_Pin,
               BIN2_Moter_GPIO_Port, BIN2_Moter_Pin);
    
    //初始化传感器
    Gray_Init(&huart2);
    
    //初始化串口
    VOFA_Init(&huart1);
}
