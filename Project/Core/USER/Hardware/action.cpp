#include "action.h"
#include <stdint.h>
#define angle2radian -0.01745

// 构造函数,初始化时将所有参数均设置为0
action::action(UART_HandleTypeDef *huartx):SerialDevice(huartx)					   
{
	memset(&posture, 0, sizeof(posture));	
	memset(&delta_posture, 0, sizeof(posture));	
	memset(&Origindata, 0, sizeof(Origindata));
}

// 获取数据句柄
ActionData_t* action::getdata(void){
	 return &posture;
}

// 进行差运算，再获取数据句柄
Delta_Data_t* action::GetDeltaData(void)
{
	delta_posture.D_zAngle = posture.zAngle - delta_posture.record_angle;
	delta_posture.D_xPos   = posture.xPos   - delta_posture.record_x;
	delta_posture.D_yPos   = posture.yPos   - delta_posture.record_y;
	return &delta_posture;
}

void action::handleReceiveData(uint8_t byte){
	switch(state_){
		// 检测包头0x0D,0x0A
		case WAITING_FOR_HEADER_0:
			if(byte == FRAME_HEAD_0_ACTION){
				state_ = WAITING_FOR_HEADER_1;
			}
			break;
		case WAITING_FOR_HEADER_1:
			if(byte == FRAME_HEAD_1_ACTION){
				state_ = WAITING_FOR_DATA;
				rxIndex_ = 0;
			}
			else if (byte == FRAME_HEAD_0_ACTION)
        {
            /* code */
        }
			else{
				state_ = WAITING_FOR_HEADER_0;
			}
			break;
			
		// 数据接收
		case WAITING_FOR_DATA:
			Origindata.data[rxIndex_++] = byte;
			if(rxIndex_ >= 24){
				rxIndex_ = 0;
				state_ = WAITING_FOR_END_0;
			}
			break;
			
		// 检测包尾0x0A,0x0D,若检测到说明数据包有效
		case WAITING_FOR_END_0:
			if(byte == FRAME_END_0_ACTION){
				state_ = WAITING_FOR_END_1;
			}
			else{
				state_ = WAITING_FOR_HEADER_0;
			}
			break;
		case WAITING_FOR_END_1:
			if(byte == FRAME_END_1_ACTION){
				posture.zAngle = Origindata.ActVal[0] * 0.01745;
				posture.xAngle = Origindata.ActVal[1] * 0.01745;
				posture.yAngle = Origindata.ActVal[2] * 0.01745;
				posture.xPos = Origindata.ActVal[3];
				posture.yPos = Origindata.ActVal[4];
				posture.zw = Origindata.ActVal[5];
			}
			state_ = WAITING_FOR_HEADER_0;
			break;
		default:
			state_ = WAITING_FOR_HEADER_0;
			break;
	}
}
