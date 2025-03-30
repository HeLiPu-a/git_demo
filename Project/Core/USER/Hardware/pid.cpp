#include "pid.h"

pid::pid(pid_param_t *param_, pid_func_t *func_)
{
    memcpy(&param, param_, sizeof(param));
    memcpy(&func, func_, sizeof(func));
	deadzone_time = 0;
}
float pid::caculate(float setpiont_, float input_)
{
    // 保存输出
    memcpy(&output_last, &output, sizeof(output));

    input.setpoint = setpiont_;                 // 设定值
    input.input = input_;                       // 输入值
    input.error = input.setpoint - input.input; // 偏差
    
    if(func.USE_DEADZONE)
    {
        if (input.error < param.deadzone && input.error > -param.deadzone)           // 死区实现
        {
            input.error = 0;
			deadzone_time += param.dt;
			if(deadzone_time>=100)
			{
				integral = 0;
				deadzone_time = 0;
			}
        }
		else
		{
			deadzone_time =0;
		}
    }

    if(func.USE_INTEGRAL_SEPERATE)
    {
        if (input.error < param.Integral_Separation || input.error > -param.Integral_Separation) // 积分分离实现
        {
            integral += input.error; // 更新积分值
        }
    }
    else
    {
        integral += input.error; // 更新积分值
    }

    output.p = param.Kp * input.error;                                 // 比例输出
    output.i = param.Ki * integral * param.dt;                         // 积分输出
    output.d = param.Kd * (input.error - input_last.error) / param.dt; // 微分输出
    
    if(func.USE_I_MAX)
    {
      if (output.i > param.i_max)                              // 输出限幅
      {
        output.i = param.i_max;
      }
      else if (output.i < -param.i_max)
      {
        output.i = -param.i_max;
      } 
        
    }
    
    output.output = output.p + output.i + output.d;                    // 输出
    

    
    if(func.USE_OUTPUT_MAX)
    {
        if (output.output > param.output_max)                              // 输出限幅
        {
            output.output = param.output_max;
        }
        else if (output.output < -param.output_max)
        {
            output.output = -param.output_max;
        }
    }

    // 保存输入
    memcpy(&input_last, &input, sizeof(input));

    // 输出结果
    return output.output;
}




//float M3508::limit(float IN,float max)
//{
//	if(IN > max)	  IN = max;
//	else if(IN < -max)IN = -max;	
//	return IN;
//}
