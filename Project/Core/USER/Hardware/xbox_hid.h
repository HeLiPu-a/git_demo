#ifndef _XBOX_HID_H_
#define _XBOX_HID_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include "main.h"
#include "SerialDevice.h"
#include <string.h>
#include <math.h>
/*引用外部文件end*/	

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*在此处进行宏定义：         begin*/	

#define PI 3.1415926
#define MAX_DATA_LENGTH_XBOX 64
#define FRAME_HEAD_0_XBOX 0xFC
#define FRAME_HEAD_1_XBOX 0xFB
#define FRAME_END_0_XBOX 0xFD
#define FRAME_END_1_XBOX 0xFE
#define FRAME_ID_XBOX 
/*宏定义end*/	


/*在此处进枚举类型定义：         begin*/	
//结构体
typedef struct{
	// 从数据包中获取的数据
  bool btnY, btnB, btnA, btnX; // A,B,X,Y按键
	bool btnY_last, btnB_last, btnA_last, btnX_last;
	
  bool btnShare, btnStart, btnSelect, btnXbox;				   // 菜单操作键
	bool btnShare_last, btnStart_last, btnSelect_last, btnXbox_last;
	
  bool btnLB, btnRB;					 // 左右肩键
	bool btnLB_last, btnRB_last;
	
  bool btnLS, btnRS;					 // 左右摇杆按钮
	bool btnLS_last, btnRS_last;
	
  bool btnDPadUp, btnDPadLeft, btnDPadRight, btnDPadDown;// 十字键
	bool btnDPadUp_last, btnDPadLeft_last, btnDPadRight_last, btnDPadDown_last;
	
  uint16_t joyLX, joyLY;										// 左摇杆
  uint16_t joyRX, joyRY;										// 右摇杆	
  uint16_t trigL, trigR;										// 左右扳机键
} XboxOriginData_t;

/* 摇杆的处理后数据（为了节省空间，这里只对摇杆的数据做了定义），并不涉及按键 */
typedef struct{
	float normalizedLX, normalizedLY;							// 左摇杆转化后的x,y坐标(-1,1)
	float normalizedRX, normalizedRY;						  // 右摇杆转化后的x,y坐标(-1,1)
	float joyAngleL, joyAngleR;								    // 摇杆的角度(0,2pi)
	float joyRadiusL, joyRadiusR;								  // 摇杆拨出的半径(0,1)
} XboxData_t;

typedef struct serial_frame_mat
{
	uint8_t frame_head[2];									// 帧头
	uint8_t frame_id = 0;									// ID
	uint8_t data_length = 0; 								// 数据长度(单位:字节)
	uint8_t rx_temp_data_mat[MAX_DATA_LENGTH_XBOX];			// 数据
	
	union data
	{
        float msg_get[MAX_DATA_LENGTH_XBOX / 4] = {0.0f};	// 用于浮点数的接收
        uint8_t buff_msg[MAX_DATA_LENGTH_XBOX];				// 用于字节流的接收
  } data;
	
	uint8_t frame_end[2];									// 帧尾
	uint16_t crc_calculated = 0;							// CRC校验计算值
	union check_code
	{										// CRC校验码
		uint16_t crc_code;
		uint8_t crc_buff[2];								// CRC 校验的字节形式
	}check_code;

} serial_frame_mat_t;
/*枚举定义end*/	


/*在此处进行类和结构体的定义：begin*/	
// xbox类
class xbox:public SerialDevice{
	// 接口部分
	public:
		xbox(UART_HandleTypeDef *huartx)						// 构造函数
		:SerialDevice(huartx){									
			std::memset(&xbox_msgs,0,sizeof(xbox_msgs));
		}
		void handleReceiveData(uint8_t byte) override;			// 从SerialDevice继承的虚函数:数据处理函数,在串口中断中被调用
		XboxOriginData_t xbox_msgs;								          // 从Xbox手柄上接收到的数据
	  XboxData_t joy;
	// 非接口部分
	private:
		void btn_update(void);									// 将"这次的按键值"赋给"上次的按键值",完成按键值的更新
		void msgs_update(uint8_t len, uint8_t *dat);			// xbox数据包数据解析,输入数据包,输出原始数据(到结构体xbox_msgs中)
		
		void joyDataCal(void);									// 将原始数据计算得到摇杆实际数据
	
		uint8_t rxIndex_;										// 当前接收到的字节的索引
		serial_frame_mat_t rx_frame_mat;						// 接收数据的数据帧结构体
		enum rxState
	 {	// 数据接收的状态机										
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
/*类和结构体定义end*/	


/*在此处进行函数定义：       begin*/	

/*函数定义end*/	

#endif

#endif 

