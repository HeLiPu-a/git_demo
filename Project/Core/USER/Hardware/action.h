#ifndef _ACTION_H_
#define _ACTION_H_

// ͷ�ļ�
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

// �궨��&ö������
#ifdef __cplusplus
#define MAX_DATA_LENGTH_ACTION 64
#define FRAME_HEAD_0_ACTION 0x0D
#define FRAME_HEAD_1_ACTION 0x0A
#define FRAME_END_0_ACTION 0x0A
#define FRAME_END_1_ACTION 0x0D
#define FRAME_ID_ACTION 

// Action����������
typedef struct{
	float zAngle, xAngle, yAngle;	// �����,������,�����
	float xPos, yPos;							// x����,y����
	float zw;											// �������
} ActionData_t;

// Action����
class action:public SerialDevice{
	// �ӿڲ���
	public:
		ActionData_t posture;								                 // �洢ʵ������
		void handleReceiveData(uint8_t byte) override;		  // ��д��SerialDevice�̳е��麯��
		ActionData_t* getdata(void);						           // ��ȡ����(����Ϊ�ṹ����,ʹ��ʾ��:getdata->xPos)
		
	  action(UART_HandleTypeDef *huartx)					      // ���캯��,��ʼ��ʱ�����в���������Ϊ0
		:SerialDevice(huartx){
			std::memset(&posture, 0, sizeof(posture));	
			std::memset(&Origindata, 0, sizeof(Origindata));
		}
	//�ǽӿڲ���
	private:
		union{												// ���ݰ��������,��������������Խ�ԭʼ���ݴ���Ϊʵ������
			uint8_t data[24];
			float ActVal[6];
		}Origindata;
		enum rxState{										// ���ݽ��յ�״̬��
			WAITING_FOR_HEADER_0,
			WAITING_FOR_HEADER_1,
			WAITING_FOR_DATA,
			WAITING_FOR_END_0,
			WAITING_FOR_END_1
		} state_;
		uint8_t rxIndex_;									// ��ǰ���յ����ֽڵ�����
};

#endif
#endif
