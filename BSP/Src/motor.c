#include "motor.h"
#include "stm32f1xx_hal.h"
#include "stdlib.h"
#include <math.h> 

/**
 * @brief 初始化电机结构体
 * @param motor 指向Motor_t结构体的指针
 * @param htim 指向对应定时器句柄的指针
 */
void Motor_Init(Motor_t* motor, TIM_HandleTypeDef* encoder_tim, TIM_HandleTypeDef* pwm_tim, uint32_t pwm_channel,
                GPIO_TypeDef* in1_port, uint16_t in1_pin, GPIO_TypeDef* in2_port, uint16_t in2_pin)
{
    //测速部分
    motor->htim = encoder_tim;
    motor->last_count = 0;
    motor->total_pulses_count=0;
    motor->speed_rpm = 0.0f;
    
    motor->pulses_per_revolution = 1040.0f; // 13(线) * 4(倍频) * 20(减速比),电机输出轴旋转一圈单片机测得的总脉冲数
    motor->sample_time_ms = 10;          // 测速周期10ms
    
    // 启动定时器的编码器模式
    HAL_TIM_Encoder_Start(motor->htim, TIM_CHANNEL_ALL);

    //驱动部分
    motor->pwm_tim = pwm_tim;
    motor->pwm_channel = pwm_channel;
    motor->in1_port = in1_port;
    motor->in1_pin = in1_pin;
    motor->in2_port = in2_port;
    motor->in2_pin = in2_pin;
    HAL_TIM_PWM_Start(motor->pwm_tim, motor->pwm_channel); // 启动PWM输出
}


// 更新电机速度的函数
void Motor_Update_Speed(Motor_t* motor)
{
    // 获取当前编码器计数值
    int32_t current_count = (int16_t)__HAL_TIM_GET_COUNTER(motor->htim);

    // 计算脉冲差值 (处理计数器溢出)
    int32_t diff_count = current_count - motor->last_count;
    if(diff_count > 32767) { 
        diff_count -= 65536;
    }
    if(diff_count < -32768) {
        diff_count += 65536;
    }
    motor->total_pulses_count+=diff_count;
    // 计算转速 (RPM)
    float sample_time_s = motor->sample_time_ms / 1000.0f;
    motor->speed_rpm = (diff_count / motor->pulses_per_revolution) / sample_time_s * 60.0f;

    // 更新上一次的计数值
    motor->last_count = current_count;
}

void Motor_Set_PWM(Motor_t* motor, int16_t pwm_val)
{
    if (pwm_val > 0) // 正转
    {
        HAL_GPIO_WritePin(motor->in1_port, motor->in1_pin, GPIO_PIN_SET);   // IN1 = High
        HAL_GPIO_WritePin(motor->in2_port, motor->in2_pin, GPIO_PIN_RESET); // IN2 = Low

        __HAL_TIM_SET_COMPARE(motor->pwm_tim,motor->pwm_channel,pwm_val);
    }
    else if(pwm_val < 0) // 反转
    {
        HAL_GPIO_WritePin(motor->in1_port, motor->in1_pin, GPIO_PIN_RESET); // IN1 = Low
        HAL_GPIO_WritePin(motor->in2_port, motor->in2_pin, GPIO_PIN_SET);   // IN2 = High

        __HAL_TIM_SET_COMPARE(motor->pwm_tim,motor->pwm_channel,-pwm_val);
    }
    else //刹车
    {
        HAL_GPIO_WritePin(motor->in1_port, motor->in1_pin, GPIO_PIN_SET);   // IN1 = High
        HAL_GPIO_WritePin(motor->in2_port, motor->in2_pin, GPIO_PIN_SET);   // IN2 = High

        __HAL_TIM_SET_COMPARE(motor->pwm_tim,motor->pwm_channel,0);
    }
}

int32_t Motor_Get_Toal_Pulses(Motor_t*this)
{
    return this->total_pulses_count;
}
//获得当前小车总移动距离m
//方便停车
float Motor_GetPosition_m(Motor_t* motor)
{
    return motor->total_pulses_count/motor->pulses_per_revolution*0.15f;//总脉冲数/一圈的脉冲数*轮子周长m 
}
