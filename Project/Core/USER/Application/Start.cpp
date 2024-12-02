#include "Start.h"
//#define VOFA_DEBUG

xbox   xbox1(&huart1);
DetectBtn DetectBtn1(&xbox1);
action action1(&huart3); 
chasis chasis1;
	
M3508 left_3508 (&hcan1, 0x02);
M3508 right_3508(&hcan1, 0x03);
M3508 front_3508(&hcan1, 0x01);

/* 速度映射 */
 uint16_t Spe_map = 50;
 uint16_t W_map   = 30;

#ifdef VOFA_DEBUG 

VOFA vofa_debug(&huart2);

static void VOFA_set_pid(M3508* motor)
{
	(*motor).Set_PID(vofa_debug.rxData_.RxFloat_buf[1],
								vofa_debug.rxData_.RxFloat_buf[2],
								vofa_debug.rxData_.RxFloat_buf[3],
								vofa_debug.rxData_.RxFloat_buf[4],
								vofa_debug.rxData_.RxFloat_buf[5]);
	//如果用手柄设置速度的话就要屏蔽掉这个，否则VOFA会和手柄冲突
	//(*motor).Set_target_RPM(vofa_debug.rxData_.RxFloat_buf[0]);//60就是1s每转
}

static void VOFA_establish_channels()
{	
	vofa_debug.SendFloat(front_3508.pid.target);
	vofa_debug.SendFloat(front_3508.rxdata.RPM);
	
	vofa_debug.SendFloat(left_3508.pid.target);
	vofa_debug.SendFloat(left_3508.rxdata.RPM);
	
	vofa_debug.SendFloat(right_3508.pid.target);
	vofa_debug.SendFloat(right_3508.rxdata.RPM);
	
	/* 需要保证这一个函数在最后才被调用 */
	vofa_debug.VOFA_SendEND();
}

#endif

extern "C" void before_Start_tasks(void)
{
  /* 初始化can邮箱 */
	M3508::CanDevice::Can_Init();
	/* 初始化各个电机的PID参数 */
	
	front_3508.Set_PID(18.175,1,0.900,30,1700);
	right_3508.Set_PID(18.175,1,0.900,30,1700);
	left_3508.Set_PID (18.175,1,0.900,30,1700);
	
	/* 开启串口中断 */
	xbox1.startUartReceiveIT();
	action1.startUartReceiveIT();
	
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
#ifdef VOFA_DEBUG
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

  front_3508.Set_target_RPM(chasis1.front_wheel_spe);
	left_3508.Set_target_RPM(chasis1.left_wheel_spe);
	right_3508.Set_target_RPM(chasis1.right_wheel_spe);
	
	front_3508.PID_caculation();
	left_3508.PID_caculation();
	right_3508.PID_caculation();
	
	/* 需要保证这一个函数在所有电机计算完PID之后才被调用 */
	M3508::Send_Motor_data(&hcan1,0x200);

#ifdef VOFA_DEBUG
  VOFA_establish_channels();
#endif


}


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
		Spe_map <= 200 && DetectBtn1.Stop_Flag == Move)
	{
	  Spe_map += 50;
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
	if(DetectBtn1.detectButtonEdge(&DetectBtn1.Select_r_btn))
	{
 		if(DetectBtn1.chasis_control_Mode == robot_ctrl)
		{
			DetectBtn1.chasis_control_Mode = world_ctrl;
		}
		else
		{
			DetectBtn1.chasis_control_Mode = robot_ctrl;
		}
	}
  
	
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