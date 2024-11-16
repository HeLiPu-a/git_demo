#ifndef _PID_H_
#define _PID_H_

// PID控制器类
class PID{
	// 接口部分
	public:
		PID(float Kp_, float Ki_, float Kd_, float output_max_, float output_min_);		// 构造函数,初始化PID参数
		float pid_cal(float error_,float dt);											// PID控制函数,参数: 本次误差, 每次PID计算的时间间隔
		float pi_cal(float error_, float dt);											// PI控制函数,参数: 本次误差, 每次PID计算的时间间隔
	// 非接口部分
	private:
		float Kp, Ki, Kd;																// PID基本参数Kp,Ki,Kd
		float error, previous_error, integral, rate;									// 本次误差,上次误差,误差累积,误差速率
	float output_max, output_min;														// 输出限幅:最大输出，最小输出
};

#endif
