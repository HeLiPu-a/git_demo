#include "Start.h"
//#define VOFA_DEBUG
#define spe_map 200
M3508 left_3508 (&hcan1, 0x01);
M3508 right_3508(&hcan1, 0x02);
M3508 front_3508(&hcan1, 0x03);

xbox   xbox1(&huart1);
action action1(&huart3); 
chasis chasis1;

#ifdef VOFA_DEBUG 

VOFA vofa_debug(&huart2);

static void VOFA_set_pid()
{
	front_3508.Set_PID(vofa_debug.rxData_.RxFloat_buf[1],
								vofa_debug.rxData_.RxFloat_buf[2],
								vofa_debug.rxData_.RxFloat_buf[3],
								vofa_debug.rxData_.RxFloat_buf[4],
								vofa_debug.rxData_.RxFloat_buf[5]);
	front_3508.Set_target_RPM(vofa_debug.rxData_.RxFloat_buf[0]);//60就是1s每转
}

static void VOFA_establish_channels()
{
	vofa_debug.SendFloat(front_3508.pid.target);
	vofa_debug.SendFloat(front_3508.rxdata.RPM);
	
	/* 需要保证这一个函数在最后才被调用 */
	vofa_debug.VOFA_SendEND();
}

#endif

extern "C" void before_Start_tasks(void)
{
  /* 初始化can邮箱 */
	M3508::CanDevice::Can_Init();
	/* 初始化各个电机的PID参数 */
	front_3508.Set_PID(4.075,1.425,0.600,30,300);
	right_3508.Set_PID(4.075,1.425,0.600,30,300);
	left_3508.Set_PID (4.075,1.425,0.600,30,300);
	
	/* 开启串口中断 */
	xbox1.startUartReceiveIT();
	action1.startUartReceiveIT();
	
#ifdef VOFA_DEBUG
	vofa_debug.startUartReceiveIT();
#endif
	
}

extern "C" void Start_tasks(void)
{
#ifdef VOFA_DEBUG
  VOFA_set_pid();
#endif
	chasis1.robot_cal(xbox1.joy.normalizedLX * spe_map,
	                  xbox1.joy.normalizedLY * spe_map,
	                  xbox1.joy.normalizedRX * 50);
	
  front_3508.Set_target_RPM(chasis1.front_wheel_spe);
	front_3508.PID_caculation();
  left_3508.Set_target_RPM(chasis1.left_wheel_spe);
	left_3508.PID_caculation();
	right_3508.Set_target_RPM(chasis1.right_wheel_spe);
	right_3508.PID_caculation();
	/* 需要保证这一个函数在所有电机计算完PID之后才被调用 */
	M3508::Send_Motor_data(&hcan1,0x200);

#ifdef VOFA_DEBUG
  VOFA_establish_channels();
#endif


}

btn_flag_t X_btn;

/* 传入按键此次和上一次的数值做上升沿判断 */
static void detectButtonEdge(bool currentBtnState, bool lastBtnState, bool *single_simple,
                             uint8_t *toggleState, uint8_t maxState)
{     
    if (currentBtnState && !(lastBtnState) && true != *single_simple)
    { // 检测到上升沿
      *toggleState = (*toggleState + 1) % (maxState + 1);
			*single_simple =1;
    }
		
		/* 当检测到第一个上升沿的时候，置一个标志位，当这个标志位被置起的时候认为后面的上升沿都是未更新数据导致的，
		只有当上一个按键状态和当前按键状态同为1的时候这个标志位才被置0 */
		if(currentBtnState && lastBtnState)
		{
				*single_simple=0;
		}
}

/* 按键判断任务，每3ms判断一次 判断速度高于回传速度（回传是130Hz左右)*/
extern "C" void xbox_detectbtn_tasks(void)
{
		detectButtonEdge( xbox1.xbox_msgs.btnX , xbox1.xbox_msgs.btnX_last ,
	                    &X_btn.single_simple, &X_btn.Flag , 10 );
}