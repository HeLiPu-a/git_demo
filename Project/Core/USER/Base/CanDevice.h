#ifndef CAN_DEVICE_H
#define CAN_DEVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include "stm32f4xx_hal.h"
#include "can.h"
#include <stdint.h>
/*引用外部文件end*/	

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#define CAN_MAX_INSTANCES 10 //暂时设置一条can设备最多可以创建10个实例,后面有需要可以再修改

class CanDevice
{
public: 
    CanDevice(CAN_HandleTypeDef *hcan_, uint32_t can_id_);

    CAN_HandleTypeDef *hcan = nullptr;  // 保存CAN 句柄
    CAN_TxHeaderTypeDef TxHeader1,TxHeader2;  // CAN下发报文的数据头,在子类中根据需要配置

    uint32_t can_id = 0;  //保存每个实例的can_id
    uint8_t send_buf1[8] = {0};  // CAN1下发报文的数据
    uint8_t send_buf2[8] = {0};  // CAN2下发报文的数据
    uint32_t msg_box1 = 0;  //保存can1下发报文使用的邮箱
    uint32_t msg_box2 = 0;  //保存can2下发报文使用的邮箱
    
    static CanDevice *Can1_Instances[CAN_MAX_INSTANCES];  // 保存can1上的所有实例
	static uint8_t Can1_Instances_Index;  // can1实例的数量
    static CanDevice *Can2_Instances[CAN_MAX_INSTANCES];  // 保存can2上的所有实例
	static uint8_t Can2_Instances_Index;  // can2实例的数量
    static uint8_t RxData1[8];  //存储CAN1接收到的数据
    static uint8_t RxData2[8];  //存储CAN2接收到的数据
    
    virtual void Can_SendData();  //CAN发送报文函数
    virtual void Can_update(uint8_t can_RxData[8],CAN_RxHeaderTypeDef* RxHeader);  //用于更新c6020电调上传的数据，其他设备有类似操作也可以重写该函数
    static void CAN1_Filter_Init(void);  //can过滤器配置函数
    static void CAN2_Filter_Init(void);  //默认接收所有can数据帧，使用32位过滤器掩码模式
	static void Can_Init();  //初始化过滤器，整个工程只需要调用一次即可
};

#endif

#endif
