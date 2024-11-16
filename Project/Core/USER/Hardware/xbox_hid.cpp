#include "xbox_hid.h"

void xbox::msgs_update(uint8_t *dat){
    // A,B,X,Y����
    xbox_msgs.btnY = dat[0];
    xbox_msgs.btnB = dat[1];
    xbox_msgs.btnA = dat[2];
    xbox_msgs.btnX = dat[3];
	// �˵�������
    xbox_msgs.btnShare = dat[4];
    xbox_msgs.btnStart = dat[5];
    xbox_msgs.btnSelect = dat[6];
    xbox_msgs.btnXbox = dat[7];
	// ���Ҽ��
    xbox_msgs.btnLB = dat[8];
    xbox_msgs.btnRB = dat[9];
	// ����ҡ�˰�ť
    xbox_msgs.btnL3 = dat[10];
    xbox_msgs.btnR3 = dat[11];
	// ʮ�ּ�
    xbox_msgs.btnDPadUp = dat[12];
    xbox_msgs.btnDPadLeft = dat[13];
    xbox_msgs.btnDPadRight = dat[14];
    xbox_msgs.btnDPadDown = dat[15];
	// ����ҡ��
    xbox_msgs.joyLX = ((uint16_t)dat[16] << 8) | dat[17];
    xbox_msgs.joyLY = ((uint16_t)dat[18] << 8) | dat[19];
    xbox_msgs.joyRX = ((uint16_t)dat[20] << 8) | dat[21];
    xbox_msgs.joyRY = ((uint16_t)dat[22] << 8) | dat[23];
	// ���Ұ����
    xbox_msgs.trigL = ((uint16_t)dat[24] << 8) | dat[25];
    xbox_msgs.trigR = ((uint16_t)dat[26] << 8) | dat[27];
	// ҡ������ת��Ϊ������
	xbox::joyDataCal();
}

void xbox::joyDataCal(void){
	// ҡ������ת��Ϊ(-1,1)�ϵ�ʵ������
	xbox_msgs.normalizedLX = (xbox_msgs.joyLX - 32768) / 65535;
	xbox_msgs.normalizedLY = (xbox_msgs.joyLY - 32768) / 65535;
	xbox_msgs.normalizedRX = (xbox_msgs.joyRX - 32768) / 65535;
	xbox_msgs.normalizedRY = (xbox_msgs.joyRY - 32768) / 65535;
	// ת��Ϊ��������ʽ
	xbox_msgs.joyAngleL = atan2(xbox_msgs.normalizedLY, xbox_msgs.normalizedLX);
	xbox_msgs.joyAngleR = atan2(xbox_msgs.normalizedRY, xbox_msgs.normalizedRX);
	xbox_msgs.joyRadiusL = sqrt(xbox_msgs.normalizedLX * xbox_msgs.normalizedLX + xbox_msgs.normalizedLY * xbox_msgs.normalizedLY);
	xbox_msgs.joyRadiusR = sqrt(xbox_msgs.normalizedRX * xbox_msgs.normalizedRX + xbox_msgs.normalizedRY * xbox_msgs.normalizedRY);
}

