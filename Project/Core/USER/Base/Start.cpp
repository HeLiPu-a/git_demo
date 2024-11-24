#include "Start.h"

//SerialDevice U3 (&huart6, false);
M3508 front(&hcan1, 0x01);
VOFA vofa_debug(&huart2);
xbox xbox1(&huart1);
extern "C" void before_Start_tasks(void)
{
//	Package U3_PK;
//	U3_PK.Head_0   = 0x0d;
//	U3_PK.Head_1   = 0x0a;
//	U3_PK.End_0    = 0x0a;
//	U3_PK.End_1 	 = 0x0d; 
//	U3_PK.length   = 3; 
//	U3_PK.frame_id = 1;
//	U3.SetRxPackage_identity(U3_PK);
//  U3.startUartReceiveIT();
	M3508::CanDevice::Can_Init();
	front.Set_PID(10,0,0,0,0);
	vofa_debug.startUartReceiveIT();
	xbox1.startUartReceiveIT();
}

extern "C" void Start_tasks(void)
{
	front.Set_PID(vofa_debug.rxData_.RxFloat_buf[1],
								vofa_debug.rxData_.RxFloat_buf[2],
								vofa_debug.rxData_.RxFloat_buf[3],
								vofa_debug.rxData_.RxFloat_buf[4],
								vofa_debug.rxData_.RxFloat_buf[5]);
	//front.Set_target_RPM(vofa_debug.rxData_.RxFloat_buf[0]);//60就是1s每转
  front.Set_target_RPM(200*xbox1.joy.normalizedLX);
	front.PID_caculation();
	M3508::Send_Motor_data(&hcan1,0x200);
	vofa_debug.SendFloat(front.pid.target);
	vofa_debug.SendFloat(front.rxdata.RPM);
	vofa_debug.VOFA_SendEND();
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