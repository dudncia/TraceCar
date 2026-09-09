#include"pid.h"
/**
 * @brief:初始化PID控制器
 */
void PID_Init(PID_t*pid,float Kp,float Ki,float Kd,float out_max,float out_min)
{
    pid->Kp=Kp;
    pid->Ki=Ki;
    pid->Kd=Kd;
    pid->output_max=out_max;
    pid->output_min=out_min;

    pid->target=0.0f;
    pid->error=0.0f;
    pid->last_error=0.0f;
    pid->integral=0.0f;
}

/**
 * @brief:计算PID输出值
 * @param:pid 指向PID结构体的指针
 * @param:current_value 当前测量值
 * @retval：PID计算出的控制输出
 */
float PID_Calculate(PID_t*pid)
{
    //计算误差值error
    pid->error=pid->target - pid->current_value;
    //计算积分项的值（带抗积分饱和）
    pid->integral+=pid->error;
    if(pid->integral>pid->output_max){
        pid->integral=pid->output_max;
    }
    if(pid->integral<pid->output_min){
        pid->integral=pid->output_min;
    }
    //计算微分项
    float derivative=pid->error-pid->last_error;
    //计算总输出
    float output=pid->Kp*pid->error+
                pid->Ki*pid->integral+
                pid->Kd*derivative;
    //输出限幅
    if(output>pid->output_max){
        output=pid->output_max;
    }
    if(output<pid->output_min){
        output=pid->output_min;
    }
    pid->last_error = pid->error;
    pid->pwm_v=output;
	return output;
}
