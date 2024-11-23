#include "M3508.h"
int16_t M3508::m3508_can1_num =0;
int16_t M3508::m3508_can2_num =0;
uint8_t M3508::M_can1_Tdata_buf1[8];
uint8_t M3508::M_can1_Tdata_buf2[8];
uint8_t M3508::M_can2_Tdata_buf1[8];
uint8_t M3508::M_can2_Tdata_buf2[8];
//M3508* M3508::m3508_can2_ptrBUF[M3508_can2_num]={nullptr};

bool M3508::M3508_error_flag=false;//当发生错误的时候，会被赋值为false

M3508::M3508(CAN_HandleTypeDef *hcan_, uint32_t can_id_):CanDevice(hcan_,can_id_){
	//can_id的范围只在1~8之间，对应电调的8个id
	if(can_id_>8)
	{
		M3508_error_flag= true;//置错误位，可以通过debug查看
		return;
	}
		
	
	//不用判断id是否重复，因为candivice的构造函数已经帮我们做好了
	if(hcan_ == &hcan1)
	{	
		m3508_can1_num++;
		//当电机数量超过最大容量的时候就会置错误标志位
		if(m3508_can1_num>=max_M3508_can1_num)
		{
			M3508_error_flag= true;//置错误位，可以通过debug查看
			return;
		}
		
		//将电调的id号对应到M3508的指针数组中，注意是对应放入，所以当不连续放入时
		//有些为空指针，这时候就需要进行判断（在后面每次使用到都得判断）
		//m3508_can1_ptrBUF[can_id_] = this;
		
	}else if(hcan_ == &hcan2)
	{
		m3508_can2_num++;
		if(m3508_can2_num>=max_M3508_can2_num)
		{
			M3508_error_flag= true;//置错误位，可以通过debug查看
			return;
		}
		//先进行前面的层层判断再赋值，否则可能赋值了，但是实际是超出了，这样子可能会被其他
		//线程调用，有危险
		//m3508_can2_ptrBUF[can_id_] = this;
	}
}

//需要在所有M3508实例调用完PID和发送数据装填之后才调用！！！！
void M3508::Send_Motor_data(CAN_HandleTypeDef *hcan_,uint32_t send_ID)
	{	//当输入的标识符不是这两个，说明参数传错
	if(send_ID !=0x200 &&send_ID != 0x1FF)
	{
		M3508_error_flag = true;
	}
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t msg_box = 0;
	//如果是0x200则控制的是电调为1~4的电机，如果是0x1FF则控制的是电调为5~8的电机
	TxHeader.StdId = send_ID; 
  TxHeader.ExtId = 0x00;   	  //不使用扩展帧
	TxHeader.RTR = CAN_RTR_DATA;  //使用数据帧,这个帧包括下那个都是hal库提供的
	TxHeader.IDE = CAN_ID_STD;    //使用标准帧
  TxHeader.DLC = 8;  		   	  //数据长度为8字节
  TxHeader.TransmitGlobalTime = DISABLE;  //不使用全局时间戳
//	if(hcan_== &hcan1)
//	{
//			uint8_t start;
//			uint8_t end;
//			if(send_ID == 0x200)
//			{
//				start = 0;
//				start = 4;
//			}else if(send_ID == 0x1FF)
//			{
//				start = 5;
//				start = 8;
//			}
//			for(uint8_t i=start;i<end;i++)
//			{
//				if(m3508_can1_ptrBUF[i] !=nullptr)
//				{
//					m3508_can1_ptrBUF[i]->add_M3508_buf();
//					send_buf[i] = 
//				}
//			}
//	}
//	else if(hcan_ == &hcan2)
//	{
//		for(int i=0;i<max_M3508_can2_num;i++)
//		{
//			M3508::CanDevice::Can2_Instances[i]->Can_SendData();
//		}
//	}
		if(send_ID == 0x200)
		{
			if(hcan_ == &hcan1)
			HAL_CAN_AddTxMessage(hcan_,&TxHeader,M3508::M_can1_Tdata_buf1,&msg_box);
			if(hcan_ == &hcan2)
			HAL_CAN_AddTxMessage(hcan_,&TxHeader,M3508::M_can2_Tdata_buf1,&msg_box);	
		}
		else if(send_ID == 0x1FF)
		{
		  if(hcan_ == &hcan1)
			HAL_CAN_AddTxMessage(hcan_,&TxHeader,M3508::M_can1_Tdata_buf2,&msg_box);
			if(hcan_ == &hcan2)
			HAL_CAN_AddTxMessage(hcan_,&TxHeader,M3508::M_can2_Tdata_buf2,&msg_box);	
		}
}

// void M3508::Can_SendData(){
// 	// TxHeader1.StdId = send_Std_id; //用于控制id为0x201~0x204的电机的can报文数据头
//     // TxHeader1.ExtId = 0x00;   	   //不使用扩展帧
//     // TxHeader1.RTR = CAN_RTR_DATA;  //使用数据帧,这两个帧都是hal库提供的
//     // TxHeader1.IDE = CAN_ID_STD;    //使用标准帧
//     // TxHeader1.DLC = 8;  		   //数据长度为8字节
//     // TxHeader1.TransmitGlobalTime = DISABLE;  //不使用全局时间戳
// 	// if(hcan == &hcan1)
// 	// {
// 	// 	for(int i=0;i<m3508_can1_num;i++)
// 	// }
// 	// else if(hcan == &hcan2)
// 	// {
// 	// 	for(int i=0;i<m3508_can2_num;i++)
// 	// }
// 	// HAL_CAN_AddTxMessage(hcan,&TxHeader1,send_buf1,&msg_box1);

// }
void M3508::Set_PID(float kp_,float ki_,float kd_,float dead_zone_,
			     	float integral_separate_,float limmit_output_)
{
	pid.kp = kp_;
	pid.ki = ki_;
	pid.kd = kd_;
	pid.dead_zone = dead_zone_;
	pid.limit_output = limmit_output_;
	pid.integral_separate = integral_separate_;

}
void M3508::Set_target_RPM(float target_RPM)
{
	pid.target = target_RPM*19;
}

float M3508::limit(float IN,float max)
{
	if(IN > max)	  IN = max;
	else if(IN < -max)IN = -max;	
	return IN;
}

void M3508::PID_caculation()
{
	float result;	
	pid.error = pid.target - pid.current;
	if(pid.error > -pid.dead_zone && pid.error < pid.dead_zone)
	{//此时说明误差在死区当中，不进行控制
		return;
	}
	pid.p_out = pid.kp * pid.error;
	if(pid.error>pid.integral_separate||pid.error<-pid.integral_separate)
	{	//当误差大于积分分界值时，积分部分不进行积分，同时对i的输出赋值为0，只进行PD控制
		//如果不赋值为0，那么当error迅速变换，超过积分分界值时，i_out其实不为0，会影响输出
		//但是积分限幅可能会导致PID响应变慢。但是突然将i_out赋值为0会不会不太好。
		//pid.i_out = 0;
		pid.integral = 0;
	}else{
		pid.integral += pid.error;
		pid.i_out = pid.ki * pid.integral;
	}
	pid.d_out = pid.kd * (pid.error - pid.last_error);
	pid.last_error = pid.error;
	result = pid.p_out + pid.i_out + pid.d_out;
	pid.output = limit(result,pid.limit_output);
	
	//将pid的浮点数输出转化为16位整型的电流，同时赋值给send_current
	pidOUT2current();
	add_M3508_buf();
}
void M3508::pidOUT2current()
{
	send_current = pid.output;
}
void M3508::add_M3508_buf()
{
	if(M3508::CanDevice::can_id>=1 && M3508::CanDevice::can_id <= 4)
	{
		uint8_t send_buf_idex = (M3508::CanDevice::can_id-1)*2;
		if(hcan == &hcan1)
		{
			M_can1_Tdata_buf1[send_buf_idex]   = (uint8_t)(send_current>>8);
			M_can1_Tdata_buf1[send_buf_idex+1] = (uint8_t)send_current;
		}else{
			M_can2_Tdata_buf1[send_buf_idex]   = (uint8_t)(send_current>>8);
			M_can2_Tdata_buf1[send_buf_idex+1] = (uint8_t)send_current;
		}
	}else if(M3508::CanDevice::can_id>=5 && M3508::CanDevice::can_id <= 8)
	{
		uint8_t send_buf_idex = (M3508::CanDevice::can_id-5)*2;
		if(hcan == &hcan1)
		{
			M_can1_Tdata_buf2[send_buf_idex]   = (uint8_t)(send_current>>8);
			M_can1_Tdata_buf2[send_buf_idex+1] = (uint8_t)send_current;
		}else{
			M_can2_Tdata_buf2[send_buf_idex]   = (uint8_t)(send_current>>8);
			M_can2_Tdata_buf2[send_buf_idex+1] = (uint8_t)send_current;
		}
	}
}

//下面是有关接收数据的函数

void M3508::Can_update(uint8_t can_RxData[8],CAN_RxHeaderTypeDef* RxHeader)
{
	if(RxHeader->StdId == M3508::CanDevice::can_id+0x200)
	{
		rxdata.angle 	   = can_RxData[0]<<8 | can_RxData[1];
		rxdata.RPM 		   = can_RxData[2]<<8 | can_RxData[3];
		rxdata.Current 	   = can_RxData[4]<<8 | can_RxData[5];
		rxdata.temperature = can_RxData[6];
		pid.current = rxdata.RPM;//这里是转子的RPM，外圈的转速要经过19的减速比。
		updata_ok = 1;
	}
	
}

void M3508::rxdata2info()
{	
	if(updata_ok)
	{
		M3508_info.angle   = rxdata.angle*360.0/8191.0;
		M3508_info.RPM     = rxdata.RPM;
		M3508_info.Current = rxdata.Current*20/16384;
		M3508_info.temperature = rxdata.temperature;
		updata_ok =0;
	}
} 

void M3508_error_handle()
{
	while (1)
	{
		;
	}
	
}

