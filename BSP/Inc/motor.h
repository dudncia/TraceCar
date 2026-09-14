#ifndef __MOTOR_H
#define __MOTOR_H
#include "main.h"
#include "stm32f1xx_hal.h"

typedef struct 
{
    TIM_HandleTypeDef*  htim;                   // 电机编码器对应的定时器句柄指针
    TIM_HandleTypeDef*  pwm_tim;                // PWM输出对应的定时器句柄指针

    int32_t             last_count;             // 上一次的编码器计数值
    float               speed_rpm;              // 电机转�2�� (RPM)
    
    float               pulses_per_revolution;  // 输出轴转一圈的总脉冲数
    int32_t             total_pulses_count;      // 输出轴从开始到目前为止的总脉冲数
    uint32_t            sample_time_ms;         // 测速周期

    uint32_t            pwm_channel;            // PWM输出的通道 (e.g., TIM_CHANNEL_1)
    GPIO_TypeDef*       in1_port;               // IN1 引脚的端口
    uint16_t            in1_pin;                // IN1 引脚的编号
    GPIO_TypeDef*       in2_port;               // IN2 引脚的端口
    uint16_t            in2_pin;                // IN2 引脚的编号
}Motor_t;

void Motor_Init(Motor_t* motor, TIM_HandleTypeDef* encoder_tim, TIM_HandleTypeDef* pwm_tim, uint32_t pwm_channel,
                GPIO_TypeDef* in1_port, uint16_t in1_pin, GPIO_TypeDef* in2_port, uint16_t in2_pin); //电机初始化
void Motor_Update_Speed(Motor_t* motor);                 // 更新电机速度 (核心测速函数)
void Motor_Set_PWM(Motor_t* motor, int16_t pwm_val);     // 设置电机PWM输出
int32_t Motor_Get_Toal_Pulses(Motor_t*this);//总脉冲数
float Motor_GetPosition_m(Motor_t* motor);
#endif
