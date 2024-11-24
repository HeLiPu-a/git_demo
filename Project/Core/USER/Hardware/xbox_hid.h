#ifndef _XBOX_HID_H_
#define _XBOX_HID_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*�ڴ˴������ⲿ�ļ���       begin*/	
#include "main.h"
#include "SerialDevice.h"
#include <string.h>
#include <math.h>
/*�����ⲿ�ļ�end*/	

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*�ڴ˴����к궨�壺         begin*/	

#define PI 3.1415926
#define MAX_DATA_LENGTH_XBOX 64
#define FRAME_HEAD_0_XBOX 0xFC
#define FRAME_HEAD_1_XBOX 0xFB
#define FRAME_END_0_XBOX 0xFD
#define FRAME_END_1_XBOX 0xFE
#define FRAME_ID_XBOX 
/*�궨��end*/	


/*�ڴ˴���ö�����Ͷ��壺         begin*/	
//�ṹ��
typedef struct{
	// �����ݰ��л�ȡ������
  bool btnY, btnB, btnA, btnX; // A,B,X,Y����
	bool btnY_last, btnB_last, btnA_last, btnX_last;
	
  bool btnShare, btnStart, btnSelect, btnXbox;				   // �˵�������
	bool btnShare_last, btnStart_last, btnSelect_last, btnXbox_last;
	
  bool btnLB, btnRB;					 // ���Ҽ��
	bool btnLB_last, btnRB_last;
	
  bool btnLS, btnRS;					 // ����ҡ�˰�ť
	bool btnLS_last, btnRS_last;
	
  bool btnDPadUp, btnDPadLeft, btnDPadRight, btnDPadDown;// ʮ�ּ�
	bool btnDPadUp_last, btnDPadLeft_last, btnDPadRight_last, btnDPadDown_last;
	
  uint16_t joyLX, joyLY;										// ��ҡ��
  uint16_t joyRX, joyRY;										// ��ҡ��	
  uint16_t trigL, trigR;										// ���Ұ����
} XboxOriginData_t;

/* ҡ�˵Ĵ�������ݣ�Ϊ�˽�ʡ�ռ䣬����ֻ��ҡ�˵��������˶��壩�������漰���� */
typedef struct{
	float normalizedLX, normalizedLY;							// ��ҡ��ת�����x,y����(-1,1)
	float normalizedRX, normalizedRY;						  // ��ҡ��ת�����x,y����(-1,1)
	float joyAngleL, joyAngleR;								    // ҡ�˵ĽǶ�(0,2pi)
	float joyRadiusL, joyRadiusR;								  // ҡ�˲����İ뾶(0,1)
} XboxData_t;

typedef struct serial_frame_mat
{
	uint8_t frame_head[2];									// ֡ͷ
	uint8_t frame_id = 0;									// ID
	uint8_t data_length = 0; 								// ���ݳ���(��λ:�ֽ�)
	uint8_t rx_temp_data_mat[MAX_DATA_LENGTH_XBOX];			// ����
	
	union data
	{
        float msg_get[MAX_DATA_LENGTH_XBOX / 4] = {0.0f};	// ���ڸ������Ľ���
        uint8_t buff_msg[MAX_DATA_LENGTH_XBOX];				// �����ֽ����Ľ���
  } data;
	
	uint8_t frame_end[2];									// ֡β
	uint16_t crc_calculated = 0;							// CRCУ�����ֵ
	union check_code
	{										// CRCУ����
		uint16_t crc_code;
		uint8_t crc_buff[2];								// CRC У����ֽ���ʽ
	}check_code;

} serial_frame_mat_t;
/*ö�ٶ���end*/	


/*�ڴ˴�������ͽṹ��Ķ��壺begin*/	
// xbox��
class xbox:public SerialDevice{
	// �ӿڲ���
	public:
		xbox(UART_HandleTypeDef *huartx)						// ���캯��
		:SerialDevice(huartx){									
			std::memset(&xbox_msgs,0,sizeof(xbox_msgs));
		}
		void handleReceiveData(uint8_t byte) override;			// ��SerialDevice�̳е��麯��:���ݴ�����,�ڴ����ж��б�����
		XboxOriginData_t xbox_msgs;								          // ��Xbox�ֱ��Ͻ��յ�������
	  XboxData_t joy;
	// �ǽӿڲ���
	private:
		void btn_update(void);									// ��"��εİ���ֵ"����"�ϴεİ���ֵ",��ɰ���ֵ�ĸ���
		void msgs_update(uint8_t len, uint8_t *dat);			// xbox���ݰ����ݽ���,�������ݰ�,���ԭʼ����(���ṹ��xbox_msgs��)
		
		void joyDataCal(void);									// ��ԭʼ���ݼ���õ�ҡ��ʵ������
	
		uint8_t rxIndex_;										// ��ǰ���յ����ֽڵ�����
		serial_frame_mat_t rx_frame_mat;						// �������ݵ�����֡�ṹ��
		enum rxState
	 {	// ���ݽ��յ�״̬��										
			WAITING_FOR_HEADER_0,
			WAITING_FOR_HEADER_1,
			WAITING_FOR_ID,
			WAITING_FOR_LENGTH,
			WAITING_FOR_DATA,
			WAITING_FOR_CRC_0,
			WAITING_FOR_CRC_1,
			WAITING_FOR_END_0,
			WAITING_FOR_END_1
		} state_;
};
/*��ͽṹ�嶨��end*/	


/*�ڴ˴����к������壺       begin*/	

/*��������end*/	

#endif

#endif 

