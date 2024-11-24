#ifndef _ACTION_H_
#define _ACTION_H_

// 头文件
#ifdef __cplusplus
extern "C"
{
#endif
#include <string.h >
#include "main.h"
#include "SerialDevice.h"
#ifdef __cplusplus
}
#endif

// 宏定义&枚举类型
#ifdef __cplusplus
#define MAX_DATA_LENGTH_ACTION 64
#define FRAME_HEAD_0_ACTION 0x0D
#define FRAME_HEAD_1_ACTION 0x0A
#define FRAME_END_0_ACTION 0x0A
#define FRAME_END_1_ACTION 0x0D
#define FRAME_ID_ACTION 

// Action处理后的数据
typedef struct{
	float zAngle, xAngle, yAngle;	// 航向角,俯仰角,横滚角
	float xPos, yPos;							// x坐标,y坐标
	float zw;											// 航向角速
} ActionData_t;

// Action的类
class action:public SerialDevice{
	// 接口部分
	public:
		ActionData_t posture;								                 // 存储实际数据
		void handleReceiveData(uint8_t byte) override;		  // 重写从SerialDevice继承的虚函数
		ActionData_t* getdata(void);						           // 获取数据(类型为结构体句柄,使用示例:getdata->xPos)
		
	  action(UART_HandleTypeDef *huartx)					      // 构造函数,初始化时将所有参数均设置为0
		:SerialDevice(huartx){
			std::memset(&posture, 0, sizeof(posture));	
			std::memset(&Origindata, 0, sizeof(Origindata));
		}
	//非接口部分
	private:
		union{												// 数据包里的数据,利用联合体的特性将原始数据处理为实际数据
			uint8_t data[24];
			float ActVal[6];
		}Origindata;
		enum rxState{										// 数据接收的状态机
			WAITING_FOR_HEADER_0,
			WAITING_FOR_HEADER_1,
			WAITING_FOR_DATA,
			WAITING_FOR_END_0,
			WAITING_FOR_END_1
		} state_;
		uint8_t rxIndex_;									// 当前接收到的字节的索引
};

#endif
#endif
