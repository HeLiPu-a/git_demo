#include "pid_init.h"

//typedef struct
//{
//    float Kp;                  // 比例增益
//    float Ki;                  // 积分增益
//    float Kd;                  // 微分增益
//    float Integral_Separation; // 积分分离
//    float deadzone;            // 死区
//    float i_max;               // 积分限幅
//    float output_max;          // 输出限幅
//    float dt;                  // 步长


// M3508的PID初始化
pid_param_t M3508param = {.Kp = 10, 
                          .Ki = 0.1, 
                          .Kd = 0.900, 
                          .Integral_Separation = 1700,
                          .deadzone = 50,
                          .i_max = 3000,
                          .output_max = 16384, 
                          .dt = 4};

pid_param_t position_param = {.Kp = 0.1, 
                              .Ki = 0.2, 
                              .Kd = 3.500, 
                              .Integral_Separation = 300,
                              .deadzone = 100,
                              .i_max = 20,
                              .output_max = 200, 
                              .dt = 4};

pid_func_t M3508func= {true, true, true,true};
pid mypid_front(&M3508param,&M3508func);
pid mypid_left(&M3508param,&M3508func);
pid mypid_right(&M3508param,&M3508func);
// 位置环PID初始化

pid_func_t position_pid_func = {true, true, true,true};
pid position_pid(&position_param, &position_pid_func);

void PID_Init(void)
{
	M3508param.Kp                  = 10;
	M3508param.Ki                  = 0.1;
	M3508param.Kd                  = 0.9;
	M3508param.Integral_Separation = 1700;
	M3508param.deadzone            = 50;
	M3508param.i_max               = 3000;
	M3508param.output_max          = 16384;
	M3508param.dt                  = 4;
}
