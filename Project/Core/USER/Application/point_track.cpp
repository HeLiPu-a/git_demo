#include   "point_track.h"


void point_track::point_track_(action *action1)
{
    // 计算目标点与当前点的偏移向量
    point_track_info.dx = point_track_info.target_x - (*action1).posture.xPos;
    point_track_info.dy = point_track_info.target_y - (*action1).posture.yPos;

     //计算到目标点的距离（模长）
    point_track_info.distan_error = sqrt(point_track_info.dx * point_track_info.dx + point_track_info.dy * point_track_info.dy);
        
    // 判断是否已到达目标点
    if (point_track_info.distan_error < 0.01) {
        point_track_info.distan_error = 0.0f;
       // stop_chassis(); // 停止底盘
        return ;
    }

    // 计算单位方向向量（归一化），避免除零错误
    float direction_x = 0.0f, direction_y = 0.0f;
    if (point_track_info.distan_error > 0.0f) {
        direction_x = point_track_info.dx / point_track_info.distan_error;
        direction_y = point_track_info.dy / point_track_info.distan_error;
    }
    
    point_track_info.alfa = acos(point_track_info.dx/point_track_info.distan_error);   //计算斜边和x边的夹角为deta
    if (point_track_info.dy < 0)
    {
    point_track_info.alfa = acos(point_track_info.dx/point_track_info.distan_error);   //计算斜边和x边的夹角为deta
        point_track_info.alfa = 2*PI - point_track_info.alfa;
    }


    ////float target_speed_result = PID_Position_caculation(point_track_info.distan_error);   //使用斜边模长求出斜边方向的目标速度
    
    ////target_speed.target_speed_x = target_speed_result * cos(alfa);           //将斜边方向的目标速度乘以夹角求出x方向的目标速度
    ////target_speed.target_speed_y = target_speed_result * sin(alfa);

}