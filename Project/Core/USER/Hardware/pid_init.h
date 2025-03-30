#ifndef PID_INIT_H
#define PID_INIT_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "pid.h"
#include "pid_init.h"
	
void PID_Init(void);

#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern pid_param_t M3508param;
extern pid_func_t M3508func ;
extern pid mypid_front;
extern pid mypid_left;
extern pid mypid_right;

extern pid_param_t position_pid_param;
extern pid_func_t position_pid_func;
extern pid position_pid;
#endif
#endif
