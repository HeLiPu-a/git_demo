#ifndef __START_H
#define __START_H

#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include "SerialDevice.h"
#include "TaskManager.h"
#include "M3508.h"
#include "xbox_hid.h"
#include "VOFA.h"
#include "action.h"
#include "chasis_caculation.h"
/*引用外部文件end*/	


/*在此处向.c文件声明函数：       begin*/
void Start_tasks(void);	
void before_Start_tasks(void);
void xbox_detectbtn_tasks(void);
/*函数声明end*/	
	
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*在此处进行宏定义：         begin*/	

/*宏定义end*/	


/*在此处进枚举类型定义：         begin*/	

/*枚举定义end*/	


/*在此处进行类和结构体的定义：begin*/	

/*类和结构体定义end*/	
typedef struct btn_flag
{
	bool single_simple;
	uint8_t Flag;
}btn_flag_t;

/*在此处进行函数定义：       begin*/	

//如果是要在.C调用，要放到上面（翻上去看）

/*函数定义end*/	

#endif

#endif 
