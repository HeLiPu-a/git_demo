#include "action.h"

void action::data_analyse(uint8_t rec){
	raw.ch = rec;
	switch(raw.count){
		// ����ͷ0x0d,0x0a
		case 0:
			if(raw.ch == 0x0d){
				raw.count ++;
			}
			else{
				raw.count = 0;
			}
			break;
		case 1:
			if(raw.ch == 0x0a){
				raw.count ++;
			}
			else{
				raw.count = 0;
			}
			break;
			
		// ���ݽ���
		case 2:
			raw.RawPosture.data[raw.i] = raw.ch;
			raw.i ++;
			if(raw.i >= 24){
				raw.i = 0;
				raw.count ++;
			}
			break;
			
		// ����β0x0a,0x0d,����⵽˵�����ݰ���Ч
		case 3:
			if(raw.ch == 0x0a){
				raw.count ++;
			}
			else{
				raw.count =0;
			}
			break;
		case 4:
			if(raw.ch == 0x0d){
				posture.zAngle = raw.RawPosture.ActVal[0];
				posture.xAngle = raw.RawPosture.ActVal[1];
				posture.yAngle = raw.RawPosture.ActVal[2];
				posture.xPos = raw.RawPosture.ActVal[3];
				posture.yPos = raw.RawPosture.ActVal[4];
				posture.zw = raw.RawPosture.ActVal[5];
			}
			raw.count = 0;
			break;
		default:
			raw.count = 0;
			break;
	}
}
