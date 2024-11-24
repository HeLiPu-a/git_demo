#include "chasis_caculation.h"

// 机器人坐标系下的计算
void chasis::robot_cal(float tgvx, float tgvy, float tgw){
	// 逆解算后得到三个电机的目标速度
	front_wheel_spe = tgvx + 2*tgw;
	left_wheel_spe = 0.70711*tgvx - 0.70711*tgvy + 2.82843*tgw;
	right_wheel_spe = 0.70711*tgvx + 0.70711*tgvy + 2.82843*tgw;
}

// 世界坐标系下的计算
void chasis::world_cal(float tgvx, float tgvy, float tgw, float angle_bias){
	// 将世界坐标系下的速度分解到机器人坐标系下
	float tgvx_ = cos(angle_bias)*tgvx + sin(angle_bias)*tgvy;
	float tgvy_ = sin(angle_bias)*tgvx + cos(angle_bias)*tgvy;
	// 分解到机器人坐标系下后再进行世界坐标系下的计算
	robot_cal(tgvx_, tgvy_, tgw);
}
