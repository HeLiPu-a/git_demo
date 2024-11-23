#include "Start.h"

//SerialDevice U3 (&huart6, false);
M3508 front(&hcan1, 0x01);
VOFA vofa_debug(&huart2);

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
}

extern "C" void Start_tasks(void)
{
	front.Set_PID(vofa_debug.rxData_.RxFloat_buf[1],
								vofa_debug.rxData_.RxFloat_buf[2],
								vofa_debug.rxData_.RxFloat_buf[3],
								vofa_debug.rxData_.RxFloat_buf[4],
								vofa_debug.rxData_.RxFloat_buf[5]);
	front.Set_target_RPM(vofa_debug.rxData_.RxFloat_buf[0]);//60就是1s每转
  front.PID_caculation();
	M3508::Send_Motor_data(&hcan1,0x200);
	vofa_debug.SendFloat(front.pid.target);
	vofa_debug.SendFloat(front.rxdata.RPM);
	vofa_debug.VOFA_SendEND();
}