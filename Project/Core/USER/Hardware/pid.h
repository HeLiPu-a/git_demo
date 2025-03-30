#ifndef _PID_H_
#define _PID_H_
#include <string.h>
#ifdef __cplusplus
extern "C" 
{
#endif
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
typedef struct
{
    float p; // 输出
    float i; // 积分输出
    float d; // 微分输出
    float output; // 最终输出
} pid_output_t;  // pid输出

typedef struct
{
    float Kp;                  // 比例增益
    float Ki;                  // 积分增益
    float Kd;                  // 微分增益
    float Integral_Separation; // 积分分离
    float deadzone;            // 死区
    float i_max;               // 积分限幅
    float output_max;          // 输出限幅
    float dt;                  // 步长
} pid_param_t;                 // pid参数

typedef struct
{
    float setpoint; // 目标值
    float input;    // 当前值
    float error;    // 误差
} pid_input_t;      // pid输入

typedef struct
{
    bool USE_INTEGRAL_SEPERATE;
    bool USE_DEADZONE ;
    bool USE_I_MAX;
    bool USE_OUTPUT_MAX;  
}pid_func_t;

// pid类
class pid
{
public:
    pid(pid_param_t *param_, pid_func_t *func_);
    float caculate(float setpoint, float input);    // 计算输出

 private:
    pid_param_t param;
    pid_input_t input, input_last;
    pid_output_t output, output_last;
    pid_func_t func;
    float integral;
	float deadzone_time;			// 死区计时，用于清空积分项
};

#endif
#endif
