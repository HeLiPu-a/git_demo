#ifndef _XBOX_HID_H_
#define _XBOX_HID_H_
#include "main.h"
#include <cstring>
#include <cmath>

#define pi 3.1415926

//�ṹ��
typedef struct{
	// �����ݰ��л�ȡ������
    bool btnY, btnB, btnA, btnX;								// A,B,X,Y����
    bool btnShare, btnStart, btnSelect, btnXbox;				// �˵�������
    bool btnLB, btnRB;											// ���Ҽ��
    bool btnL3, btnR3;											// ����ҡ�˰�ť
    bool btnDPadUp, btnDPadLeft, btnDPadRight, btnDPadDown;		// ʮ�ּ�
    uint16_t joyLX, joyLY;										// ��ҡ��			
    uint16_t joyRX, joyRY;										// ��ҡ��	
    uint16_t trigL, trigR;										// ���Ұ����
	// ���㴦��������
	double normalizedLX, normalizedLY;							// ��ҡ��ת�����x,y����(-1,1)
	double normalizedRX, normalizedRY;							// ��ҡ��ת�����x,y����(-1,1)
	double joyAngleL, joyAngleR;								// ҡ�˵ĽǶ�(0,2pi)
	double joyRadiusL, joyRadiusR;								// ҡ�˲����İ뾶(0,1)
} XboxData_t;

// xbox��
class xbox{
	// �ӿڲ���
	public:
		xbox(void){												// ���캯��,ʹ���в�����ʼ��ʱ��Ϊ0
			std::memset(&xbox_msgs,0,sizeof(xbox_msgs));
		}
		void msgs_update(uint8_t *dat);							// xbox���ݰ����ݽ���,�������ݰ�,���ԭʼ����(���ṹ��xbox_msgs��)
		XboxData_t xbox_msgs;									// ��Xbox�ֱ��Ͻ��յ���ԭʼ����
	// �ǽӿڲ���
	private:
		void joyDataCal(void);									// ��ԭʼ���ݼ���õ�ҡ��ʵ������
};

#endif