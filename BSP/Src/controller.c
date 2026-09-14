#include "controller.h"
#include "motor.h"
#include "pid.h"
#include "vofa.h"
#include "gray.h"

extern UART_HandleTypeDef huart1;

//Motor
extern Motor_t B_right_motor;
extern Motor_t A_left_motor;

//PID实例
extern PID_t gray_position_pid;
extern PID_t A_position_pid;
extern PID_t A_speed_pid;
extern PID_t B_position_pid;
extern PID_t B_speed_pid;

//基础速度
extern float BaseSpeed;


//滤波参数
static const float filter_a=0.8f;

/**
 * @brief:pid初始化
 */
void Controller_Init(void)
{
    //编码器位置环
	PID_Init(&A_position_pid, 0.05f, 0.0f, 0.03f, 320.0f, -320.0f);
    PID_Init(&B_position_pid, 0.05f, 0.0f, 0.03f, 320.0f, -320.0f);
    //偏移位置环
    //参数只有kp
    PID_Init(&gray_position_pid, 39.0f, 0.0f, 0.0f, 320.0f, -320.0f);
    //速度环
    PID_Init(&A_speed_pid, 5.35f, 0.89f, 0.011f, 1000.0f, -1000.0f);
    PID_Init(&B_speed_pid, 5.35f, 0.89f, 0.011f, 1000.0f, -1000.0f);
}


/**
 * @brief:设定目标位置
 * @param: pos 目标位置
 */
void Controller_Set_Target_Position(int32_t pos)
{
    A_position_pid.target=(float)pos;
}


/**
 * @brief:回调函数，关心位置和速度
 */
void pos_Controller_Update_Callback(void)
{
    //更新速度
    Motor_Update_Speed(&A_left_motor);
    A_position_pid.current_value=A_left_motor.total_pulses_count;
    A_speed_pid.current_value=A_left_motor.speed_rpm;

    //滤波参数设定
    A_speed_pid.filtered_value=filter_a*A_speed_pid.filtered_value+(1-filter_a)*A_speed_pid.current_value;

    //pid计算
    float target_speed=PID_Calculate(&A_position_pid);
    A_speed_pid.target=target_speed;
    int16_t pwm_out=(int16_t)PID_Calculate(&A_speed_pid);

    //输出pwm
    Motor_Set_PWM(&A_left_motor,pwm_out);
}


/*关心偏转量和速度（外部传参basespeed可由pos_Controller_Update_Callback（）设定，
                  从而实现电机基本速度的可调节）*/
void gray_Controller_Update_Callback(float base_speed,float gray_error)
{
    //更新转向量
    gray_position_pid.target=0.0f;
    gray_position_pid.current_value=gray_error;
    float steer = PID_Calculate(&gray_position_pid);// 输出转向量
    
    //更新当前电机转速
    Motor_Update_Speed(&A_left_motor);
    Motor_Update_Speed(&B_right_motor);
    A_speed_pid.current_value=A_left_motor.speed_rpm;
    B_speed_pid.current_value=B_right_motor.speed_rpm;  
    //记录轮子转过的距离方便最后停车
    //A_position_pid.current_value=A_left_motor.total_pulses_count;    
    //B_position_pid.current_value=B_right_motor.total_pulses_count;

    
    //设定目标速度
    A_speed_pid.target = base_speed - steer; // 左轮
    B_speed_pid.target = base_speed + steer; // 右轮

    //滤波参数设定
    A_speed_pid.filtered_value=filter_a*A_speed_pid.filtered_value+(1-filter_a)*A_speed_pid.current_value;

    //pid计算
    int16_t A_pwm_out=(int16_t)PID_Calculate(&A_speed_pid);
    int16_t B_pwm_out=(int16_t)PID_Calculate(&B_speed_pid);

    //输出pwm
    Motor_Set_PWM(&A_left_motor,A_pwm_out);
    Motor_Set_PWM(&B_right_motor,B_pwm_out);
}


//通过设定小车的目标距离来改变小车的basespeed，最后的目标是停车
void SetBaseSpeed(void)
{
    
    //主回调已更新过速度，这里就不更新了
    //Motor_Update_Speed(&A_left_motor);
    A_position_pid.current_value=A_left_motor.total_pulses_count;
    A_speed_pid.current_value=A_left_motor.speed_rpm;

    //滤波参数设定
    A_speed_pid.filtered_value=filter_a*A_speed_pid.filtered_value+(1-filter_a)*A_speed_pid.current_value;

    //pid计算
    float target_speed=PID_Calculate(&A_position_pid);
    BaseSpeed=target_speed;//只计算出基础速度是多少不输出pwm，
                            //输出pwm留在gray_Controller_Update_Callback(float base_speed,float gray_error)里
}

