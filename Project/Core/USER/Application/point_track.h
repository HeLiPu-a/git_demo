#ifndef __POINT_TRACK_H
#define __POINT_TRACK_H

#ifdef __cplusplus
extern "C"
{       
#endif

/*在此处引用外部文件：       begin*/	
#include "main.h"
#include "point_track.h"
#include "Start.h"
/*引用外部文件end*/	


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*在此处进行宏定义：         begin*/	

/*宏定义end*/	


/*在此处进枚举类型定义：         begin*/	

/*枚举定义end*/	


/*在此处进行类和结构体的定义：begin*/	
typedef struct{
    float target_point_x;
    float target_point_y;
    float dx;
    float dy;
     float distan_error;
    float alfa;
}point_track_info_t;

typedef struct{
    float target_speed_x;
    float target_speed_y;
}target_speed_t;

 class point_track
{
    public:
    point_track_info_t point_track_info;
    target_speed_t target_speed;
    void SetTargetPoint(float x, float y);
    void point_track_(action *action1);
 };


/*类和结构体定义end*/	


/*在此处进行函数定义：       begin*/	
void track_point(action *action1);
/*函数定义end*/	

#endif



#endif