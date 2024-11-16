#ifndef _XBOX_HID_H_
#define _XBOX_HID_H_
#include "main.h"
#include <cstring>
#include <cmath>

#define pi 3.1415926

//结构体
typedef struct{
	// 从数据包中获取的数据
    bool btnY, btnB, btnA, btnX;								// A,B,X,Y按键
    bool btnShare, btnStart, btnSelect, btnXbox;				// 菜单操作键
    bool btnLB, btnRB;											// 左右肩键
    bool btnL3, btnR3;											// 左右摇杆按钮
    bool btnDPadUp, btnDPadLeft, btnDPadRight, btnDPadDown;		// 十字键
    uint16_t joyLX, joyLY;										// 左摇杆			
    uint16_t joyRX, joyRY;										// 右摇杆	
    uint16_t trigL, trigR;										// 左右扳机键
	// 计算处理后的数据
	double normalizedLX, normalizedLY;							// 左摇杆转化后的x,y坐标(-1,1)
	double normalizedRX, normalizedRY;							// 右摇杆转化后的x,y坐标(-1,1)
	double joyAngleL, joyAngleR;								// 摇杆的角度(0,2pi)
	double joyRadiusL, joyRadiusR;								// 摇杆拨出的半径(0,1)
} XboxData_t;

// xbox类
class xbox{
	// 接口部分
	public:
		xbox(void){												// 构造函数,使所有参数初始化时均为0
			std::memset(&xbox_msgs,0,sizeof(xbox_msgs));
		}
		void msgs_update(uint8_t *dat);							// xbox数据包数据解析,输入数据包,输出原始数据(到结构体xbox_msgs中)
		XboxData_t xbox_msgs;									// 从Xbox手柄上接收到的原始数据
	// 非接口部分
	private:
		void joyDataCal(void);									// 将原始数据计算得到摇杆实际数据
};

#endif