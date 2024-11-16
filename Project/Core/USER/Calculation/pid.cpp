#include "pid.h"

// 构造函数
PID::PID(float Kp_, float Ki_, float Kd_, float output_max_, float output_min_){
	Kp = Kp_;
	Ki = Ki_;
	Kd = Kd_;
	output_max = output_max_;
	output_min = output_min_;
	integral = 0;
	previous_error = 0;
}

// PID控制器
float PID::pid_cal(float error_,float dt){
	float output;
	
	// 更新误差累计,误差,速率
	integral += error_ * dt;
	error = error_;
	rate = (error - previous_error) / dt;
	
	// 计算得到输出
	output = Kp * error + Ki * integral + Kd * rate;
	
	// 最大值限幅
	if(output > 0 && output > output_max){
		output = output_max;
	}
	else if(output <0 && output < -output_max){
		output = - output_min;
	}
	
	// 最小值限幅
	if(output > 0 && output < output_min){
		output = output_min;
	}
	else if(output < 0 && output > - output_min){
		output = -output_min;
	}
	
	// 更新过去误差
	previous_error = error_;
	
	// 输出
	return output;
}

// PI控制器
float PID::pi_cal(float error_, float dt){
	float output;
	
	// 更新误差累计,误差,速率
	integral += error_ * dt;
	error = error_;
	rate = (error - previous_error) / dt;
	
	// 计算得到输出
	output = Kp * error + Ki * integral;
	
	// 最大值限幅
	if(output > 0 && output > output_max){
		output = output_max;
	}
	else if(output <0 && output < -output_max){
		output = - output_min;
	}
	
	// 最小值限幅
	if(output > 0 && output < output_min){
		output = output_min;
	}
	else if(output < 0 && output > - output_min){
		output = -output_min;
	}
	
	// 更新过去误差
	previous_error = error_;
	
	// 输出
	return output;
}
