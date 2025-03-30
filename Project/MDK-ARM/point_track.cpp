#include   "point_track.h"
//extern action action1;

/* action坐标系相对于车的重心坐标系的偏移量 */
#define x_deviation -225
#define y_deviation 50

void point_track::SetTargetPoint(float x, float y) {
    // 将目标点设置为相对于最后设置的零点
    point_track_info.target_point_x = x;  // 假设last_x_pos为零点_X坐标
    point_track_info.target_point_y = y;  // 假设last_y_pos为零点_Y坐标
}

void point_track::point_track_(action *action1)
{
    // 计算目标点与当前点的偏移向量
//    point_track_info.dx = point_track_info.target_point_x - (*action1).GetDeltaData()->D_xPos;
//    point_track_info.dy = point_track_info.target_point_y - (*action1).GetDeltaData()->D_yPos;
//      point_track_info.dx = point_track_info.target_point_x - ( (*action1).posture.xPos + x_deviation );
//      point_track_info.dy = point_track_info.target_point_y - ( (*action1).posture.yPos + y_deviation );
		point_track_info.dx = point_track_info.target_point_x - ( (*action1).posture.xPos + x_deviation );
    point_track_info.dy = point_track_info.target_point_y - ( (*action1).posture.yPos + y_deviation );
     //计算到目标点的距离（模长）
    point_track_info.distan_error = sqrt(point_track_info.dx * point_track_info.dx + point_track_info.dy * point_track_info.dy);
        
    // 判断是否已到达目标点
//    if (point_track_info.distan_error < 100) {
//        point_track_info.distan_error = 0.0f;
//       // stop_chassis(); // 停止底盘
//        return ;
//    }

    // 计算单位方向向量（归一化），避免除零错误
    float direction_x = 0.0f, direction_y = 0.0f;
    if (point_track_info.distan_error > 0.0f) {
        direction_x = point_track_info.dx / point_track_info.distan_error;
        direction_y = point_track_info.dy / point_track_info.distan_error;
    }
    
    point_track_info.alfa = acos(point_track_info.dx/point_track_info.distan_error);   //计算斜边和x边的夹角为deta
    if (point_track_info.dy < 0)
    {
        point_track_info.alfa = - point_track_info.alfa;
    }


    ////float target_speed_result = PID_Position_caculation(point_track_info.distan_error);   //使用斜边模长求出斜边方向的目标速度
    
    ////target_speed.target_speed_x = target_speed_result * cos(alfa);           //将斜边方向的目标速度乘以夹角求出x方向的目标速度
    ////target_speed.target_speed_y = target_speed_result * sin(alfa);

}