#include "Detect_btn.h"
#include <stdint.h>


extern xbox   xbox1;

DetectBtn::DetectBtn(xbox *xbox_instance)
{
	xbox_handle = xbox_instance;
	
	/* 2.将结构体内的东西全部赋值为0 */
	memset(&x_r_btn,0,sizeof(rising_edge_t));
	memset(&b_r_btn,0,sizeof(rising_edge_t));
	memset(&xbox_r_btn,0,sizeof(rising_edge_t));
	memset(&Select_r_btn,0,sizeof(rising_edge_t));
	memset(&RB_r_btn,0,sizeof(rising_edge_t));
	memset(&RB_press_btn,0,sizeof(rising_edge_t));
	
	/* 3.将xbox存放按键数据的地址赋值给结构体（相当于注册）*/
	x_r_btn.p_btn = &(xbox_handle->xbox_msgs.X);
	b_r_btn.p_btn = &(xbox_handle->xbox_msgs.B);
	xbox_r_btn.p_btn = &(xbox_handle->xbox_msgs.Xbox);
	Select_r_btn.p_btn = &(xbox_handle->xbox_msgs.Select);
	RB_r_btn.p_btn = &(xbox_handle->xbox_msgs.RB);
	RB_press_btn.p_btn = &(xbox_handle->xbox_msgs.RB);
	/* 4.在start文件中调用函数，传入上面注册的结构体的地址 */
}


/* 传入按键此次和上一次的数值做上升沿判断 */
bool DetectBtn::detectButtonEdge(rising_edge_t *Rise_btn,uint8_t maxState)
{     
	/* 当检测到第一个上升沿的时候，置一个标志位，
	   当这个标志位被置起的时候认为后面的上升沿都是未更新数据导致的，
		 只有当上一个按键状态和当前按键状态同为1的时候这个标志位才被置0 */
	if((*Rise_btn).p_btn->btn && \
		 (*Rise_btn).p_btn->btn_last)
	{
		Rise_btn->single_simple = 0;
	}
	bool record1 = (*Rise_btn).p_btn->btn;
	bool record2 = (*Rise_btn).p_btn->btn_last;
	if((*Rise_btn).p_btn->btn      && \
		!(*Rise_btn).p_btn->btn_last && \
	  !Rise_btn->single_simple)
	{
		Rise_btn->single_simple = 1;
		if(maxState)
		{/* 这是另一套逻辑（逻辑B)，返回的是按键按下的次数（超过会变为0）
			  只有maxState大于0才有效*/
			Rise_btn->Flag = ( Rise_btn->Flag + 1 ) % ( maxState + 1 );
			return Rise_btn->Flag;
		}
		
		/* 当maxState赋值为0，则返回的是按键按下与否 */
		return 1;
	}

	
	if(maxState)
	{/* 在此逻辑B下返回的是按下次数，
		  即使没有按下也得返回之前的按下次数 */
		return Rise_btn->Flag;
	}
	/* 在逻辑A下，没有按下就返回0 */
	return 0;
}


/* 检查按下时间，max_time的单位是ms */
bool DetectBtn::detectButtonPress(press_t *Press_Btn,uint32_t max_time)
{
  if((*Press_Btn).p_btn->btn == 1)
	{
		Press_Btn->press_time += Interval_time;
		if( Press_Btn->press_time > max_time )
		{
		  (Press_Btn->press_time) = max_time;
			return 1;
		}
	}
	else if((*Press_Btn).p_btn->btn == 0)
	{
		Press_Btn->press_time -= Interval_time;
		/* 预留出最后的时间间隔，避免溢出 */
		if(	Press_Btn->press_time < Interval_time)
		{
		  Press_Btn->press_time = Interval_time;
		}
	}
	
	return 0;
}	


