#ifndef _ACTION_H_
#define _ACTION_H_
#include <cstring>
#include "main.h"

// Action处理前的数据
typedef struct{
	uint8_t ch;							// 输入的字节
	uint8_t count;						// 用于将接收函数区分为不同的状态机
	uint8_t i;							// 接收数据包中的数据时用于计数
	union{								// 数据包里的数据,利用联合体的特性将原始数据处理为实际数据
		uint8_t data[24];
		float ActVal[6];
	}RawPosture;
} ActionOrigin_t;

// Action处理后的数据
typedef struct{
	float zAngle, xAngle, yAngle;		// 航向角,俯仰角,横滚角
	float xPos, yPos;					// x坐标,y坐标
	float zw;							// 航向角速
} ActionData_t;

// Action的类
class action{
	// 接口部分
	public:
		ActionData_t posture;
		void data_analyse(uint8_t rec);
		action(void){
			std::memset(&posture,0,sizeof(posture));
			std::memset(&raw,0,sizeof(raw));
		}
	//非接口部分
	private:
		ActionOrigin_t raw;
};


#endif
