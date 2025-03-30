#include "M3508.h"

int16_t M3508::m3508_can1_num = 0;
int16_t M3508::m3508_can2_num = 0;
uint8_t M3508::stop_flag = 0;
uint8_t M3508::M_can1_Tdata_buf1[8];
uint8_t M3508::M_can1_Tdata_buf2[8];
uint8_t M3508::M_can2_Tdata_buf1[8];
uint8_t M3508::M_can2_Tdata_buf2[8];
QueueHandle_t M3508::M3508_SD_Queue_Handle = xQueueCreate(10,sizeof(int*));
// M3508* M3508::m3508_can2_ptrBUF[M3508_can2_num]={nullptr};

bool M3508::M3508_error_flag = false; // 当发生错误的时候，会被赋值为false

M3508::M3508(CAN_HandleTypeDef *hcan_, uint32_t can_id_) : CanDevice(hcan_, can_id_)
{
	// can_id的范围只在1~8之间，对应电调的8个id
	if (can_id_ > 8)
	{
		M3508_error_flag = true; // 置错误位，可以通过debug查看
		return;
	}
	// 不用判断id是否重复，因为candivice的构造函数已经帮我们做好了
	if (hcan_ == &hcan1)
	{
		m3508_can1_num++;
		// 当电机数量超过最大容量的时候就会置错误标志位
		if (m3508_can1_num >= max_M3508_can1_num)
		{
			M3508_error_flag = true; // 置错误位，可以通过debug查看
			return;
		}
		// 将电调的id号对应到M3508的指针数组中，注意是对应放入，所以当不连续放入时
		// 有些为空指针，这时候就需要进行判断（在后面每次使用到都得判断）
		// m3508_can1_ptrBUF[can_id_] = this;
	}
	else if (hcan_ == &hcan2)
	{
		m3508_can2_num++;
		if (m3508_can2_num >= max_M3508_can2_num)
		{
			M3508_error_flag = true; // 置错误位，可以通过debug查看
			return;
		}
		// 先进行前面的层层判断再赋值，否则可能赋值了，但是实际是超出了，这样子可能会被其他
		// 线程调用，有危险
		// m3508_can2_ptrBUF[can_id_] = this;
	}
	
}

// 需要在所有M3508实例调用完PID和发送数据装填之后才调用！！！！
void M3508::Send_Motor_data(CAN_HandleTypeDef *hcan_, uint32_t send_ID)
{ // 当输入的标识符不是这两个，说明参数传错
	if (send_ID != 0x200 && send_ID != 0x1FF)
	{
		M3508_error_flag = true;
	}
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t msg_box = 0;
	// 如果是0x200则控制的是电调为1~4的电机，如果是0x1FF则控制的是电调为5~8的电机
	TxHeader.StdId = send_ID;
	TxHeader.ExtId = 0x00;				   // 不使用扩展帧
	TxHeader.RTR = CAN_RTR_DATA;		   // 使用数据帧,这个帧包括下那个都是hal库提供的
	TxHeader.IDE = CAN_ID_STD;			   // 使用标准帧
	TxHeader.DLC = 8;					   // 数据长度为8字节
	TxHeader.TransmitGlobalTime = DISABLE; // 不使用全局时间戳
	if (send_ID == 0x200)
	{
		if (hcan_ == &hcan1)
			HAL_CAN_AddTxMessage(hcan_, &TxHeader, M3508::M_can1_Tdata_buf1, &msg_box);
		if (hcan_ == &hcan2)      
			HAL_CAN_AddTxMessage(hcan_, &TxHeader, M3508::M_can2_Tdata_buf1, &msg_box);
	}
	else if (send_ID == 0x1FF)
	{
		if (hcan_ == &hcan1)
			HAL_CAN_AddTxMessage(hcan_, &TxHeader, M3508::M_can1_Tdata_buf2, &msg_box);
		if (hcan_ == &hcan2)
			HAL_CAN_AddTxMessage(hcan_, &TxHeader, M3508::M_can2_Tdata_buf2, &msg_box);
	}
}

//void M3508::Send_Motor_data(CAN_HandleTypeDef *hcan_, uint32_t send_ID)
//{ // 当输入的标识符不是这两个，说明参数传错
//	if (send_ID != 0x200 && send_ID != 0x1FF)
//	{
//		M3508_error_flag = true;
//	}
//	CAN_TxHeaderTypeDef TxHeader;
//	uint32_t msg_box = 0;
//	// 如果是0x200则控制的是电调为1~4的电机，如果是0x1FF则控制的是电调为5~8的电机
//	TxHeader.StdId = send_ID;
//	TxHeader.ExtId = 0x00;				   // 不使用扩展帧
//	TxHeader.RTR = CAN_RTR_DATA;		   // 使用数据帧,这个帧包括下那个都是hal库提供的
//	TxHeader.IDE = CAN_ID_STD;			   // 使用标准帧
//	TxHeader.DLC = 8;					   // 数据长度为8字节
//	TxHeader.TransmitGlobalTime = DISABLE; // 不使用全局时间戳
//	static uint8_t * address = NULL;
//	if (send_ID == 0x200)
//	{
//		if (hcan_ == &hcan1)
//		{
//			address = M3508::M_can1_Tdata_buf1;
//			BaseType_t ret = xQueueSend(M3508::M3508_SD_Queue_Handle,&address,0);
//			if(ret != pdPASS)
//			{
//				SEGGER_RTT_printf(0,"Fail to Send the 3508Msg\r\n");
//			}
//			else 
//			{
//				SEGGER_RTT_printf(0,"Seccusse to Send the 3508Msg,%x，at %d\r\n",address,HAL_GetTick());
//			}
//		}
//		
//		if (hcan_ == &hcan2)      
//			HAL_CAN_AddTxMessage(hcan_, &TxHeader, M3508::M_can2_Tdata_buf1, &msg_box);
//	}
//	else if (send_ID == 0x1FF)
//	{
//		if (hcan_ == &hcan1)
//			HAL_CAN_AddTxMessage(hcan_, &TxHeader, M3508::M_can1_Tdata_buf2, &msg_box);
//		if (hcan_ == &hcan2)
//			HAL_CAN_AddTxMessage(hcan_, &TxHeader, M3508::M_can2_Tdata_buf2, &msg_box);
//	}
//}

float M3508::Set_Point(float target)
{
	float setpoint = target * 19;
	return setpoint;
}


void M3508::set_current(int16_t send_current)
{
	if (M3508::CanDevice::can_id >= 1 && M3508::CanDevice::can_id <= 4)
	{
		uint8_t send_buf_idex = (M3508::CanDevice::can_id - 1) * 2;
		if (hcan == &hcan1)
		{
			M_can1_Tdata_buf1[send_buf_idex] = (uint8_t)(send_current >> 8);
			M_can1_Tdata_buf1[send_buf_idex + 1] = (uint8_t)send_current;
		}
		else
		{
			M_can2_Tdata_buf1[send_buf_idex] = (uint8_t)(send_current >> 8);
			M_can2_Tdata_buf1[send_buf_idex + 1] = (uint8_t)send_current;
		}
	}
	else if (M3508::CanDevice::can_id >= 5 && M3508::CanDevice::can_id <= 8)
	{
		uint8_t send_buf_idex = (M3508::CanDevice::can_id - 5) * 2;
		if (hcan == &hcan1)
		{
			M_can1_Tdata_buf2[send_buf_idex] = (uint8_t)(send_current >> 8);
			M_can1_Tdata_buf2[send_buf_idex + 1] = (uint8_t)send_current;
		}
		else
		{
			M_can2_Tdata_buf2[send_buf_idex] = (uint8_t)(send_current >> 8);
			M_can2_Tdata_buf2[send_buf_idex + 1] = (uint8_t)send_current;
		}
	}
}

// 下面是有关接收数据的函数
void M3508::Can_update(uint8_t can_RxData[8], CAN_RxHeaderTypeDef *RxHeader)
{
	if (RxHeader->StdId == M3508::CanDevice::can_id + 0x200)
	{
		rxdata.angle = can_RxData[0] << 8 | can_RxData[1];
		rxdata.RPM = can_RxData[2] << 8 | can_RxData[3];
		rxdata.Current = can_RxData[4] << 8 | can_RxData[5];
		rxdata.temperature = can_RxData[6];
		// 这里是转子的RPM，外圈的转速要经过19的减速比。
		updata_ok = 1;
	}
	stop_flag = 0;
}
void M3508::rxdata2info()
{
	if (updata_ok)
	{
		M3508_info.angle = rxdata.angle * 360.0 / 8191.0;
		M3508_info.RPM = rxdata.RPM;
		M3508_info.Current = rxdata.Current * 20 / 16384;
		M3508_info.temperature = rxdata.temperature;
		updata_ok = 0;
	}
}

void M3508_error_handle()
{
	while (1)
	{
		;
	}
}
