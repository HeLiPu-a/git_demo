#ifndef _PID_H_
#define _PID_H_

// PID��������
class PID{
	// �ӿڲ���
	public:
		PID(float Kp_, float Ki_, float Kd_, float output_max_, float output_min_);		// ���캯��,��ʼ��PID����
		float pid_cal(float error_,float dt);											// PID���ƺ���,����: �������, ÿ��PID�����ʱ����
		float pi_cal(float error_, float dt);											// PI���ƺ���,����: �������, ÿ��PID�����ʱ����
	// �ǽӿڲ���
	private:
		float Kp, Ki, Kd;																// PID��������Kp,Ki,Kd
		float error, previous_error, integral, rate;									// �������,�ϴ����,����ۻ�,�������
	float output_max, output_min;														// ����޷�:����������С���
};

#endif
