#ifndef __PID_H
#define __PID_H

typedef struct
{
    //å¯è°ƒèŠ‚çš„å‚æ•°
    float Kp;
    float Ki;
    float Kd;

    //ç›®æ ‡å€¼Öµ
    float target;
    
    //å½“å‰å€¼
    float current_value;
    
    //é™åˆ¶
    float output_max;
    float output_min;

    //å†…éƒ¨å‚æ•°
    float error;
    float last_error;
		
		float filtered_value;
    float integral;
    float pwm_v;
}PID_t;

/**
 * @brief:PID²ÎÊı³õÊ¼»¯
 * @param:
 */
void PID_Init(PID_t*pid,float Kp,float Ki,float Kd,float out_max,float out_min);

/**
 * @brief:PIDÊä³ö
 */
float PID_Calculate(PID_t*pid);
#endif
