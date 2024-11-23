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
#include "TaskManager.h"
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
/*宏定义end*/	


/*在此处进枚举类型定义：         begin*/	

/*枚举定义end*/	


/*在此处进行类和结构体的定义：begin*/	
typedef struct PID
{
	float kp = 0;
	float ki = 0;
	float kd = 0;
	float dead_zone;
	float integral_separate;
	float limit_output;

	float target;
	float current;
	float error = 0;
	float last_error = 0;
	float integral = 0;
	float p_out;
	float i_out;
	float d_out;
	float output;
	
}PID_t;

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
}M3508_info_t;

class M3508 : public CanDevice
{
public:

	static int16_t  m3508_can1_num;
	static int16_t  m3508_can2_num;
	static uint8_t M_can1_Tdata_buf1[8];
	static uint8_t M_can1_Tdata_buf2[8];
	static uint8_t M_can2_Tdata_buf1[8];
	static uint8_t M_can2_Tdata_buf2[8];
	PID_t	 pid;
	rxdata_t rxdata;
	M3508_info_t M3508_info;
	//设置3508要使用那个can设备和自身的id号
	M3508(CAN_HandleTypeDef *hcan_, uint32_t can_id_);
	void Set_PID(float kp_,float ki_,float kd_,float dead_zone_,
			     float integral_separate_,float limmit_output_=16384);
				 			 			  //最大输出默认为最大电流值	
	void Set_target_RPM(float target_RPM);
	void PID_caculation();
	//由于pid的输出是float类型的，但是发送的电流需要是int16_t类型的，所以需要进行转换
	void add_M3508_buf();
	static void Send_Motor_data(CAN_HandleTypeDef *hcan_,uint32_t send_ID);	
	
	void rxdata2info();
	//void Can_SendData();//将3508要发送的电流值通过can设备发送出去
	void Can_update(uint8_t can_RxData[8],
								  CAN_RxHeaderTypeDef* RxHeader);
		 		    	 		
	static bool M3508_error_flag;
	//static bool set_pid_ok=false;
	private:
	int16_t send_current;
	uint8_t updata_ok;
	float limit(float IN,float max);//对输出进行限幅
	
	void pidOUT2current();
};
/*类和结构体定义end*/	


/*在此处进行函数定义：       begin*/	

/*函数定义end*/	

#endif

#endif
