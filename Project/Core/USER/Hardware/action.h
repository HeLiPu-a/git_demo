#ifndef _ACTION_H_
#define _ACTION_H_
#include <cstring>
#include "main.h"

// Action����ǰ������
typedef struct{
	uint8_t ch;							// ������ֽ�
	uint8_t count;						// ���ڽ����պ�������Ϊ��ͬ��״̬��
	uint8_t i;							// �������ݰ��е�����ʱ���ڼ���
	union{								// ���ݰ��������,��������������Խ�ԭʼ���ݴ���Ϊʵ������
		uint8_t data[24];
		float ActVal[6];
	}RawPosture;
} ActionOrigin_t;

// Action����������
typedef struct{
	float zAngle, xAngle, yAngle;		// �����,������,�����
	float xPos, yPos;					// x����,y����
	float zw;							// �������
} ActionData_t;

// Action����
class action{
	// �ӿڲ���
	public:
		ActionData_t posture;
		void data_analyse(uint8_t rec);
		action(void){
			std::memset(&posture,0,sizeof(posture));
			std::memset(&raw,0,sizeof(raw));
		}
	//�ǽӿڲ���
	private:
		ActionOrigin_t raw;
};


#endif
