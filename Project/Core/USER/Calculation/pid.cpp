#include "pid.h"

// ���캯��
PID::PID(float Kp_, float Ki_, float Kd_, float output_max_, float output_min_){
	Kp = Kp_;
	Ki = Ki_;
	Kd = Kd_;
	output_max = output_max_;
	output_min = output_min_;
	integral = 0;
	previous_error = 0;
}

// PID������
float PID::pid_cal(float error_,float dt){
	float output;
	
	// ��������ۼ�,���,����
	integral += error_ * dt;
	error = error_;
	rate = (error - previous_error) / dt;
	
	// ����õ����
	output = Kp * error + Ki * integral + Kd * rate;
	
	// ���ֵ�޷�
	if(output > 0 && output > output_max){
		output = output_max;
	}
	else if(output <0 && output < -output_max){
		output = - output_min;
	}
	
	// ��Сֵ�޷�
	if(output > 0 && output < output_min){
		output = output_min;
	}
	else if(output < 0 && output > - output_min){
		output = -output_min;
	}
	
	// ���¹�ȥ���
	previous_error = error_;
	
	// ���
	return output;
}

// PI������
float PID::pi_cal(float error_, float dt){
	float output;
	
	// ��������ۼ�,���,����
	integral += error_ * dt;
	error = error_;
	rate = (error - previous_error) / dt;
	
	// ����õ����
	output = Kp * error + Ki * integral;
	
	// ���ֵ�޷�
	if(output > 0 && output > output_max){
		output = output_max;
	}
	else if(output <0 && output < -output_max){
		output = - output_min;
	}
	
	// ��Сֵ�޷�
	if(output > 0 && output < output_min){
		output = output_min;
	}
	else if(output < 0 && output > - output_min){
		output = -output_min;
	}
	
	// ���¹�ȥ���
	previous_error = error_;
	
	// ���
	return output;
}
