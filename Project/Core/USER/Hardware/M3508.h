#ifndef __M3508_H
#define __M3508_H

#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include "stm32f4xx_hal.h"
#include "CanDevice.h"
#include <stdint.h>
#include "pid.h"
#include "pid_init.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "cmsis_os.h"
#include "SEGGER_RTT.h"
/*引用外部文件end*/	

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*在此处进行宏定义：         begin*/	
#define front_ID 0x201
#define Left_ID  0x202
#define Right_ID 0x203

#define Max_vCurrent 16384
#define Min_vCurrent -16384

#define max_M3508_can1_num 8//最大为8，根据实际情况来
#define max_M3508_can2_num 8

const osMessageQueueAttr_t M3508_Queue01_attributes = {
  .name = "M3508_Queue"
};


typedef struct rxdata//发送消息的频率为1KHz,
{
	int16_t angle;		//虚拟角度值，从0~360度 映射到0~8191
	int16_t RPM;		
	int16_t Current;	//虚拟电流值，从-20~20A 映射到-16384~16384
	uint8_t temperature;//温度值
}rxdata_t;

typedef struct M3508_info
{
	int16_t angle;
	int16_t RPM;
	int16_t Current; 
	uint8_t temperature;
} M3508_info_t;

class M3508 : public CanDevice
{
public:
	static int16_t  m3508_can1_num;
	static int16_t  m3508_can2_num;
	static uint8_t M_can1_Tdata_buf1[8];
	static uint8_t M_can1_Tdata_buf2[8];
	static uint8_t M_can2_Tdata_buf1[8];
	static uint8_t M_can2_Tdata_buf2[8];
	rxdata_t rxdata;
	M3508_info_t M3508_info;
	static uint8_t stop_flag;
	static QueueHandle_t M3508_SD_Queue_Handle;
	

	//设置3508要使用那个can设备和自身的id号
	M3508(CAN_HandleTypeDef *hcan_, uint32_t can_id_);
	float Set_Point(float target);
    void set_current(int16_t send_current)   ;
	static void Send_Motor_data(CAN_HandleTypeDef *hcan_,uint32_t send_ID);	
	void rxdata2info();
	void Can_update(uint8_t can_RxData[8], CAN_RxHeaderTypeDef* RxHeader);
	static bool M3508_error_flag; 
	private:
	uint8_t updata_ok;
};
#endif
#endif
