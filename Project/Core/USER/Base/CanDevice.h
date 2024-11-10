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
#define MAX_INSTANCES 4 // 一条can上最多挂四个3508

enum Can_id
{
    m3508_id_1 = 0x201,
    m3508_id_2 = 0x202,
    m3508_id_3 = 0x203,
    m3508_id_4 = 0x204,

    gm6020_id_1 = 0x205,
    gm6020_id_2 = 0x206,
    gm6020_id_3 = 0x207,
    gm6020_id_4 = 0x208

};

class CanDevice
{
public:
    CAN_HandleTypeDef *hcan_;  // CAN 句柄
    uint8_t can_id = 0;
    static CanDevice *Can1_Instances[MAX_INSTANCES]; // 保存can1和can2上的所有实例
    static CanDevice *Can2_Instances[MAX_INSTANCES];
    uint8_t Can1_instance_index = 0; // 实例索引
    uint8_t Can2_instance_index = 0;

    virtual int16_t motor_process(); // 给m3508用的接口，其他电机不要管
    virtual void Can_update(uint8_t can_RxData[8]) = 0;
    void CAN1_Filter_Init(void);
    void CAN2_Filter_Init(void);

    CanDevice(CAN_HandleTypeDef *hcan_, uint8_t can_id_);
};

class CanManager : public ITaskProcessor
{
private:
    
    CAN_TxHeaderTypeDef TxHeader1,TxHeader2;
    uint8_t send_buf1[8] = {0};
    uint8_t send_buf2[8] = {0};
    uint32_t msg_box1 = 0;
    uint32_t msg_box2 = 0;
    int8_t error_flag = 0;

public:
    void process_data();
    CanManager();

    static uint8_t RxData1[8];
    static uint8_t RxData2[8];
};

#endif

#endif
