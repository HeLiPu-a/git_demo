#include "xbox_hid.h"

void xbox::msgs_update(uint8_t *dat){
    // A,B,X,Y按键
    xbox_msgs.btnY = dat[0];
    xbox_msgs.btnB = dat[1];
    xbox_msgs.btnA = dat[2];
    xbox_msgs.btnX = dat[3];
	// 菜单操作键
    xbox_msgs.btnShare = dat[4];
    xbox_msgs.btnStart = dat[5];
    xbox_msgs.btnSelect = dat[6];
    xbox_msgs.btnXbox = dat[7];
	// 左右肩键
    xbox_msgs.btnLB = dat[8];
    xbox_msgs.btnRB = dat[9];
	// 左右摇杆按钮
    xbox_msgs.btnL3 = dat[10];
    xbox_msgs.btnR3 = dat[11];
	// 十字键
    xbox_msgs.btnDPadUp = dat[12];
    xbox_msgs.btnDPadLeft = dat[13];
    xbox_msgs.btnDPadRight = dat[14];
    xbox_msgs.btnDPadDown = dat[15];
	// 左右摇杆
    xbox_msgs.joyLX = ((uint16_t)dat[16] << 8) | dat[17];
    xbox_msgs.joyLY = ((uint16_t)dat[18] << 8) | dat[19];
    xbox_msgs.joyRX = ((uint16_t)dat[20] << 8) | dat[21];
    xbox_msgs.joyRY = ((uint16_t)dat[22] << 8) | dat[23];
	// 左右扳机键
    xbox_msgs.trigL = ((uint16_t)dat[24] << 8) | dat[25];
    xbox_msgs.trigR = ((uint16_t)dat[26] << 8) | dat[27];
	// 摇杆数据转化为极坐标
	xbox::joyDataCal();
}

void xbox::joyDataCal(void){
	// 摇杆坐标转化为(-1,1)上的实际数据
	xbox_msgs.normalizedLX = (xbox_msgs.joyLX - 32768) / 65535;
	xbox_msgs.normalizedLY = (xbox_msgs.joyLY - 32768) / 65535;
	xbox_msgs.normalizedRX = (xbox_msgs.joyRX - 32768) / 65535;
	xbox_msgs.normalizedRY = (xbox_msgs.joyRY - 32768) / 65535;
	// 转化为极坐标形式
	xbox_msgs.joyAngleL = atan2(xbox_msgs.normalizedLY, xbox_msgs.normalizedLX);
	xbox_msgs.joyAngleR = atan2(xbox_msgs.normalizedRY, xbox_msgs.normalizedRX);
	xbox_msgs.joyRadiusL = sqrt(xbox_msgs.normalizedLX * xbox_msgs.normalizedLX + xbox_msgs.normalizedLY * xbox_msgs.normalizedLY);
	xbox_msgs.joyRadiusR = sqrt(xbox_msgs.normalizedRX * xbox_msgs.normalizedRX + xbox_msgs.normalizedRY * xbox_msgs.normalizedRY);
}

