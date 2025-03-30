#ifndef __DETECT_BTN_H
#define __DETECT_BTN_H

#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include "main.h"
#include "xbox_hid.h"
#include "Start.h"
/*引用外部文件end*/	

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*在此处进行宏定义：         begin*/	

/*宏定义end*/	


/*在此处进枚举类型定义：         begin*/	
typedef enum 
{
	robot_ctrl = 0,
	world_ctrl,
	auto_ctrl
}chassis_ctrl_Mode_e;

typedef enum 
{
	Move = 0,
	Stop
}Is_Stop_e;
/*枚举定义end*/	


/*在此处进行类和结构体的定义：begin*/	

typedef struct rising_edge
{
	btn_t *p_btn;
	bool single_simple;
	uint8_t Flag;
}rising_edge_t;

typedef struct press
{
	btn_t *p_btn;
	uint32_t press_time;
}press_t;

/* 在这个类中对按键进行注册 */
class DetectBtn
{ 
	public:
	xbox *xbox_handle;
	uint8_t chasis_control_Mode = 0;
	bool Stop_Flag = 0;

	/* 1.添加按键 */
	
	/* 检测上升沿的按键 */
	rising_edge_t x_r_btn;
	rising_edge_t b_r_btn;
	rising_edge_t xbox_r_btn;
	rising_edge_t Select_r_btn;
	rising_edge_t RB_r_btn;
	
	press_t RB_press_btn;
	
	DetectBtn(xbox* xbox_instance);
	/* 检测上升沿的函数 */
	uint8_t detectButtonEdge(rising_edge_t *Rise_btn,uint8_t maxState=0);
	bool detectButtonPress(press_t *Press_Btn,uint32_t max_time);
	/* 检测按下的函数 */
	
	/* 检测长按的函数 */
	
};

/*类和结构体定义end*/	


/*在此处进行函数定义：       begin*/	

/*函数定义end*/	

#endif

#endif 
