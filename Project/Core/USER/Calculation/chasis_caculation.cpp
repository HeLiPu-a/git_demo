#include "chasis_caculation.h"

// ����������ϵ�µļ���
void chasis::robot_cal(float tgvx, float tgvy, float tgw){
	// ������õ����������Ŀ���ٶ�
	front_wheel_spe = tgvx + 2*tgw;
	left_wheel_spe = 0.70711*tgvx - 0.70711*tgvy + 2.82843*tgw;
	right_wheel_spe = 0.70711*tgvx + 0.70711*tgvy + 2.82843*tgw;
}

// ��������ϵ�µļ���
void chasis::world_cal(float tgvx, float tgvy, float tgw, float angle_bias){
	// ����������ϵ�µ��ٶȷֽ⵽����������ϵ��
	float tgvx_ = cos(angle_bias)*tgvx + sin(angle_bias)*tgvy;
	float tgvy_ = sin(angle_bias)*tgvx + cos(angle_bias)*tgvy;
	// �ֽ⵽����������ϵ�º��ٽ�����������ϵ�µļ���
	robot_cal(tgvx_, tgvy_, tgw);
}
