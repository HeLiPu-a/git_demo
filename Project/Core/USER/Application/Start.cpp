#include "Start.h"

//#define VOFA_DEBUG
//#defien VOFA_SetPID
EventGroupHandle_t myxEventGroupHandle_t = NULL;
//extern EventGroupHandle_t XboxEventHandle_t;
//extern SemaphoreHandle_t Xbox_Process_Dis_bias;

#define max_speed 482
xbox   xbox1(&huart1);
DetectBtn DetectBtn1(&xbox1);
action action1(&huart3); 
chasis chasis1;

	
M3508 left_3508 (&hcan1, 0x202);
M3508 right_3508(&hcan1, 0x203);
M3508 front_3508(&hcan1, 0x201);

point_track point_track1;

uint32_t last_time = 0;
uint32_t current_time = 0;
uint32_t deleat_time = 0;
/* 速度映射 */
uint16_t Spe_map = 50;
uint16_t W_map   = 30;

extern chassis_ctrl_Mode_e chassis_ctrl_Mode;
extern Is_Stop_e Is_Stop;


#ifdef VOFA_DEBUG 

VOFA vofa_debug(&huart2);

//static void VOFA_set_pid(M3508* motor)
//{
//	(*motor).Set_PID(vofa_debug.rxData_.RxFloat_buf[1],
//								vofa_debug.rxData_.RxFloat_buf[2],
//								vofa_debug.rxData_.RxFloat_buf[3],
//								vofa_debug.rxData_.RxFloat_buf[4],
//								vofa_debug.rxData_.RxFloat_buf[5]);
//	//如果用手柄设置速度的话就要屏蔽掉这个，否则VOFA会和手柄冲突
//	//(*motor).Set_target_RPM(vofa_debug.rxData_.RxFloat_buf[0]);//60就是1s每转
//}
char count = 0;
int32_t last_x_pos;
int32_t last_y_pos;
int32_t deleat_x_pos;
int32_t deleat_y_pos;
static void VOFA_establish_channels()
{	
		count++;

	
	if(count == 10)
	{
		current_time = HAL_GetTick();
		deleat_time = current_time - last_time;
		if(deleat_time == 0)
		{
			return;
		}
		int32_t current_x_pos = action1.getdata()->xPos;
		int32_t current_y_pos = action1.getdata()->yPos;
		deleat_x_pos = current_x_pos - last_x_pos;
		deleat_y_pos = current_y_pos - last_y_pos;
		float speed_x = (float)deleat_x_pos/(float)deleat_time;
		float speed_y = (float)deleat_y_pos/(float)deleat_time;
		vofa_debug.SendFloat(speed_x);
		vofa_debug.SendFloat(speed_y);
		vofa_debug.VOFA_SendEND();
		count = 0;
	
	last_x_pos = current_x_pos;
	last_y_pos = current_y_pos;
	last_time = current_time;
	}
	/* 需要保证这一个函数在最后才被调用 */
	
}

#endif

extern "C" void before_Start_tasks(void)
{
	
	PID_Init();
  /* 初始化can邮箱 */
	M3508::CanDevice::Can_Init();
	/* 初始化各个电机的PID参数 */
	//芜湖 已简化
	
	/* 开启串口中断 */
	xbox1.startUartReceiveIT(36);
	action1.startUartReceiveIT();
	
	myxEventGroupHandle_t = xEventGroupCreate();
	

#ifdef VOFA_DEBUG
	vofa_debug.startUartReceiveIT();
	vofa_debug.rxData_.RxFloat_buf[1] = 18.175;
	vofa_debug.rxData_.RxFloat_buf[2] = 1;
	vofa_debug.rxData_.RxFloat_buf[3] = 0.900;
	vofa_debug.rxData_.RxFloat_buf[4] = 30;
	vofa_debug.rxData_.RxFloat_buf[5] = 1700; 
#endif
	
}

extern "C" void Start_tasks(void)
{
#ifdef VOFA_SetPID
  VOFA_set_pid(&left_3508);
#endif
	

	if(DetectBtn1.chasis_control_Mode == robot_ctrl)
		{
			chasis1.robot_cal(xbox1.joy.normalizedLX * Spe_map,
	                      xbox1.joy.normalizedLY * Spe_map,
	                      xbox1.joy.normalizedRX * W_map);
		}
		else if(DetectBtn1.chasis_control_Mode == world_ctrl)
		{
			chasis1.world_cal(xbox1.joy.normalizedLX * Spe_map,
	                      xbox1.joy.normalizedLY * Spe_map,
	                      xbox1.joy.normalizedRX * W_map,
									      action1.GetDeltaData()->D_zAngle);  
		}
		else if(DetectBtn1.chasis_control_Mode == auto_ctrl)
		{
			point_track1.SetTargetPoint(1000,1000);  //设置目标点位置
			//使用斜边模长求出斜边方向的目标速度
   		point_track1.point_track_(&action1);
			float target_speed_result = position_pid.caculate(point_track1.point_track_info.distan_error,0); 
   
      //将斜边方向的目标速度乘以夹角求出x方向的目标速度;
	    point_track1.target_speed.target_speed_x = target_speed_result * \
			                                           cos(point_track1.point_track_info.alfa);         
      point_track1.target_speed.target_speed_y = target_speed_result * \
			                                           sin(point_track1.point_track_info.alfa);
	    //将速度交给运动解算
	    chasis1.world_cal(point_track1.target_speed.target_speed_x,
                        point_track1.target_speed.target_speed_y,
                        0,
	    					        action1.GetDeltaData()->D_zAngle);
		}
//	SEGGER_RTT_printf(0,"PID_caculation start at %d \r\n",HAL_GetTick());	
	front_3508.set_current(mypid_front.caculate(front_3508.Set_Point(60),front_3508.rxdata.RPM));
  left_3508.set_current(mypid_left.caculate(left_3508.Set_Point(chasis1.left_wheel_spe),left_3508.rxdata.RPM));
	right_3508.set_current(mypid_right.caculate(right_3508.Set_Point(chasis1.right_wheel_spe),right_3508.rxdata.RPM));
//SEGGER_RTT_printf(0,"PID_caculation finish at %d \r\n",HAL_GetTick());	
	/* 需要保证这一个函数在所有电机计算完PID之后才被调用 */
	xEventGroupSetBits(myxEventGroupHandle_t,0x01);	
//	M3508::Send_Motor_data(&hcan1,0x200);

#ifdef VOFA_DEBUG
  VOFA_establish_channels();
#endif


}

//待优化
/* 按键判断任务，每3ms判断一次 判断速度高于回传速度（回传是130Hz左右)*/
extern "C" void xbox_detectbtn_tasks(void)
{
	/* 在非急停的状态下检查是否加速减速 */
	if(DetectBtn1.detectButtonEdge(&DetectBtn1.x_r_btn) && \
		 Spe_map >= 100 && DetectBtn1.Stop_Flag == Move)
	{
		 Spe_map -= 50;
	}
	if(DetectBtn1.detectButtonEdge(&DetectBtn1.b_r_btn) && \
		Spe_map <= max_speed && DetectBtn1.Stop_Flag == Move)
	{
	  Spe_map += 50;
		if(Spe_map >=max_speed)
		{
			Spe_map = max_speed;
		}
	}
	
	/* 检查是否急停 */
	if(DetectBtn1.detectButtonEdge(&DetectBtn1.RB_r_btn) && \
		DetectBtn1.Stop_Flag == Move)
	{//只有在运动状态按下才有效，避免和长按的冲突
		DetectBtn1.Stop_Flag = Stop;
		Spe_map = 0;
		W_map   = 0;
	}
	//长按两秒解锁并恢复初始速度
	if( DetectBtn1.detectButtonPress(&DetectBtn1.RB_press_btn,2000) )
	{
		Spe_map = 50;
		W_map   = 30;
		DetectBtn1.Stop_Flag = Move;
	}
	
	/* 检查是否更新原点 */
	if(DetectBtn1.detectButtonEdge(&DetectBtn1.xbox_r_btn))
	{
		action1.delta_posture.record_angle = action1.getdata()->zAngle;
		action1.delta_posture.record_x     = action1.getdata()->xPos;
		action1.delta_posture.record_y     = action1.getdata()->yPos;
	}
	
	/* 检查是否切换底盘控制模式 */
	switch ( DetectBtn1.detectButtonEdge(&DetectBtn1.Select_r_btn,2) )
	{
		case robot_ctrl:
			DetectBtn1.chasis_control_Mode = robot_ctrl;
		break;
		case world_ctrl:
			DetectBtn1.chasis_control_Mode = world_ctrl;
		break;
		case auto_ctrl:
			DetectBtn1.chasis_control_Mode = auto_ctrl;
		break;
	}
//	if(DetectBtn1.detectButtonEdge(&DetectBtn1.Select_r_btn),2)
//	{
// 		if(DetectBtn1.chasis_control_Mode == robot_ctrl)
//		{
//			DetectBtn1.chasis_control_Mode = world_ctrl;
//		}
//		else if(DetectBtn1.chasis_control_Mode == robot_ctrl
//		{
//			DetectBtn1.chasis_control_Mode = robot_ctrl;
//		}
//	}
  
	
	/* 当30ms没有接收到M3508返回的信息，视为掉线，防止留在缓冲数组的数据经ki不断累加
	进而导致重新给电机上电时，疯狂旋转	*/
	M3508::stop_flag ++;
	if(M3508::stop_flag >= 5)
	{
		M3508::stop_flag = 10;
		front_3508.rxdata.RPM = 0;
		left_3508.rxdata.RPM = 0;
		right_3508.rxdata.RPM = 0;
	} 
}   



extern "C" void point_track_tasks(void)
{

//   point_track1.SetTargetPoint(1000,1000);  //设置目标点位置
//   //使用斜边模长求出斜边方向的目标速度
//  point_track1.point_track_(&action1);
//	float target_speed_result = position_pid.caculate(point_track1.point_track_info.distan_error,0); 
//   
//   //将斜边方向的目标速度乘以夹角求出x方向的目标速度;
//	point_track1.target_speed.target_speed_x = target_speed_result * cos(point_track1.point_track_info.alfa);         
//  point_track1.target_speed.target_speed_y = target_speed_result * sin(point_track1.point_track_info.alfa);
//	
//	//将速度交给运动解算
//	chasis1.world_cal(point_track1.target_speed.target_speed_x,
//                    point_track1.target_speed.target_speed_y,
//                    0,
//						        action1.GetDeltaData()->D_zAngle);
//   
//   front_3508.set_current (mypid_front.caculate(front_3508.Set_Point(chasis1.front_wheel_spe),front_3508.rxdata.RPM));
//   left_3508.set_current  (mypid_left.caculate(left_3508.Set_Point(chasis1.left_wheel_spe),left_3508.rxdata.RPM));
//	 right_3508.set_current (mypid_right.caculate(right_3508.Set_Point(chasis1.right_wheel_spe),right_3508.rxdata.RPM));

//	/* 需要保证这一个函数在所有电机计算完PID之后才被调用 */
//	M3508::Send_Motor_data(&hcan1,0x200);
}

/**
 * @brief  Xbox 处理函数，负责从队列中获取数据并解析处理。
 * 
 * @note   该函数会先检查 Xbox_Process_Queue 是否被正确创建。
 *         如果成功获取消息，则解析接收到的数据。
 *         若队列未收到数据，且 Frame_length 为 0，则删除任务和队列；
 *         若队列未收到数据，但 Frame_length 不为 0，则重新启动 DMA 搬运，
 *         防止 ESP32 掉线导致 IDLEIE 失效。
 * 
 * @param  None
 * @retval None
 */
extern "C" void Xbox_Handle(void)
{
	if(NULL == xbox1.Xbox_Process_Queue)
	{
#ifdef DEBUG
		SEGGER_RTT_printf(0,"EventGroup creat Fail\r\n");
#endif
		return ;
	}
	Event_Status_t ret = Event_Not_OK;
	uint32_t Process_Address = 0;
	ret = xbox1.Wait_Msg(&Process_Address);
	if(Event_OK == ret)
	{
		for(int i = 0;i<xbox1.Frame_length;i++)
		{
			//将Process_Address按照一个字节的方式访问
			xbox1.handleReceiveData(((uint8_t*)(Process_Address))[i]);
		}
	}
	else
	{
		/*如果帧长度为0，说明其实是没有打开DMA搬运，此时删除掉这个线程，
		和这个队列。处理字节的流程已经在单字节中断中进行了*/
		if(0 == xbox1.Frame_length)
		{
#ifdef DEBUG
		SEGGER_RTT_printf(0,"Delect the process data thread\r\n");
#endif			
			xbox1.Delect_Event();
			vTaskDelete(NULL);
		}
		else
		{	
		/* 假如DMA搬运中断没有发送消息过来，说明可能esp32可能掉线了，此时不断
		进行重启DMA搬运，直到esp32重新上线，因为不重启的话，串口的IDLE位会被置0，
		此时不能进入再次进入串口空闲中断，整个xbox接收并处理数据线程都会死掉 
		但我不知道为什么IDLEIE位会被赋值为0*/
#ifdef DEBUG
			SEGGER_RTT_printf(0,"DMA Restart at %d\r\n",HAL_GetTick());
#endif				
			xbox1.startUartReceiveIT(xbox1.Frame_length);
		}
	} 
}