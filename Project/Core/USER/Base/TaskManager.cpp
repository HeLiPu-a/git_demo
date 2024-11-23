#include "TaskManager.h"

TaskInfo TaskManager::tasks[MAX_TASKS];

TaskManager::TaskManager(int taskID, 
                         uint8_t priority, 
                         char* task_name, 
                         uint32_t delay_ms, 
                         uint32_t stack_size)
{   
    int i = 0;
    for(; i < MAX_TASK_NAME && task_name[i]!='\0'; i++)
    {
        tasks[taskID].name[i] = task_name[i]; 
    }if (i == MAX_TASK_NAME) {
        return;
    }else tasks[taskID].name[i] = '\0'; 
		
    tasks[taskID].Priority = priority;
    tasks[taskID].delay_ms = delay_ms;
    tasks[taskID].stack_size = stack_size;// 栈大小，不填入则默认为512字即512*4=2K字节

    xTaskCreate(TaskFunction, tasks[taskID].name, 
                tasks[taskID].stack_size, 
                (void *)taskID, tasks[taskID].Priority, 
                &tasks[taskID].handle);
}

void TaskManager::registerTask(int taskID, ITaskProcessor *instance)
{
    if (taskID < 0 || taskID >= MAX_TASKS || tasks[taskID].instanceCount >= MAX_CLASSES_PER_TASK||tasks[taskID].handle == nullptr)
    {
        return;
    }
    // 在对应任务中注册实例
    tasks[taskID].instances[tasks[taskID].instanceCount] = instance;
    tasks[taskID].instanceCount++;
}

/*TaskFunction本身也是一个freertos任务，同时也是所有任务共用的入口函数，
不同任务之间通过parameters参数进行区分，比如传入参数0,表示执行第一个任务，
传入1，表示执行第二个任务，等等等。     这么做是为了提高代码的复用性。
需要特別注意的是，执行vTaskDelay之后是对应任务被挂起，进而进行任务调度，
不是对应入口函数被挂起，理解这点对理解整个框架很是重要
*/
void TaskManager::TaskFunction(void *parameters)
{

    int run_taskID = (int)(uintptr_t)parameters;
    // TaskInfo &taskInfo = tasks[taskID];
    while (true)
    {
        for (int i = 0; i < tasks[run_taskID].instanceCount; ++i)
        { // 执行任务中的所有实例
            if (tasks[run_taskID].instances[i])
            {
                tasks[run_taskID].instances[i]->process_data();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(tasks[run_taskID].delay_ms)); //该任务挂起时间
    }
}

