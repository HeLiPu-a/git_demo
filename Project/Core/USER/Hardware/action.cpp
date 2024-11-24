#include "action.h"

// ��ȡ���ݾ��
ActionData_t* action::getdata(void){
	 return &posture;
}

void action::handleReceiveData(uint8_t byte){
	switch(state_){
		// ����ͷ0x0D,0x0A
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
			else{
				state_ = WAITING_FOR_HEADER_0;
			}
			break;
			
		// ���ݽ���
		case WAITING_FOR_DATA:
			Origindata.data[rxIndex_++] = byte;
			if(rxIndex_ >= 24){
				rxIndex_ = 0;
				state_ = WAITING_FOR_END_0;
			}
			break;
			
		// ����β0x0A,0x0D,����⵽˵�����ݰ���Ч
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
				posture.zAngle = Origindata.ActVal[0];
				posture.xAngle = Origindata.ActVal[1];
				posture.yAngle = Origindata.ActVal[2];
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
