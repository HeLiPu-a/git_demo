#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include <FreeRTOS.h>
#include <task.h>
#include <cmsis_os.h>
#include "Error_Handle.h"
/*引用外部文件end*/	

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*在此处进行宏定义：         begin*/	
#define MAX_TASKS 10
#define MAX_CLASSES_PER_TASK 20
#define MAX_TASK_NAME 12
/*宏定义end*/	


/*在此处进枚举类型定义：         begin*/	

/*枚举定义end*/	


/*在此处进行类和结构体的定义：begin*/	

// 接口类
//将组件的处理数据函数定义为虚函数，这样就可以在子类中重写该函数，实现不同的处理逻辑。
class ITaskProcessor
{
public:
    virtual void process_data() = 0; // 纯虚函数
    virtual ~ITaskProcessor() {};
    char name[16] = {'0'}; // 实例的名字，便于debug
};

// 任务信息结构体，存放任务的信息，还有最重要的ITaskProcessor类指针数组，
//是用于存放实例的指针的，便于最终调用实例的 process_data函数
struct TaskInfo
{
    TaskHandle_t handle = nullptr;
    char name[MAX_TASK_NAME];
    uint8_t Priority = 0; // 在1到5之间
    uint32_t delay_ms;
    ITaskProcessor *instances[MAX_CLASSES_PER_TASK] = {nullptr}; // 使用接口指针
    int instanceCount;
    uint32_t stack_size = 512;
};

/*任务管理者类，进行任务创建，和实例注册（即把重写的process_data函数注册到任务中），
这两个功能是分开的,创建任务不代表就就直接把实例加入任务，而是相当于写了一个空任务，
只有注册了实例之后（process_data函数所实现的内容）
才会执行实例的process_data函数。
*/
class TaskManager
{
public:
    TaskManager(int      taskID, 
                uint8_t  priority, 
                char*    task_name,
                uint32_t delay_ms, 
                uint32_t stack_size = 512);//512*4=2048字节，栈空间开辟这么大应该是足够使用的
    void registerTask(int taskID, ITaskProcessor *instance); // 注册实例 存储实例信息

private:
    static TaskInfo tasks[MAX_TASKS];
    static void TaskFunction(void *parameters); // 所有任务的入口函数
};
/*类和结构体定义end*/	


/*在此处进行函数定义：       begin*/	

/*函数定义end*/	

#endif

#endif // TASK_MANAGER_H
