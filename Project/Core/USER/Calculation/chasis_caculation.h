#ifndef _CHASIS_CACULATION_H_
#define _CHASIS_CACULATION_H_
					
#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include <math.h>

/*引用外部文件end*/	


/*在此处向.c文件声明函数：       begin*/

/*函数声明end*/	
	
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*在此处进行宏定义：         begin*/	
#define PI 3.1415926	// 定义π
/*宏定义end*/	


/*在此处进枚举类型定义：         begin*/	

/*枚举定义end*/	


/*在此处进行类和结构体的定义：begin*/	

// 电机底盘的类
class chasis{
	// 接口部分
	public:
		float front_wheel_spe ,left_wheel_spe, right_wheel_spe;											// 三个轮子的目标速度
		chasis(void){													// 构造函数,将三个参数均初始化为0
			front_wheel_spe = 0;
			left_wheel_spe  = 0;
			right_wheel_spe = 0;
		}
		void robot_cal(float tgvx, float tgvy, float tgw);				// 机器人坐标系下的解算
		void world_cal(float tgvx, float tgvy, float tgw, float angle_bias);		// 世界坐标系下的解算
	//非接口部分
	private:
};

/*类和结构体定义end*/	


/*在此处进行函数定义：       begin*/	


/*函数定义end*/	

#endif

#endif

