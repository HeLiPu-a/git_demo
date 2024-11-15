#include "CanDevice.h"
//静态变量的初始化
CanDevice* CanDevice::Can1_Instances[CAN_MAX_INSTANCES] = {nullptr};
CanDevice* CanDevice::Can2_Instances[CAN_MAX_INSTANCES] = {nullptr};
uint8_t CanDevice::Can1_Instances_Index = 0;
uint8_t CanDevice::Can2_Instances_Index = 0;
uint8_t CanDevice::RxData1[8] = {0};
uint8_t CanDevice::RxData2[8] = {0};

CanDevice::CanDevice(CAN_HandleTypeDef *hcan_, uint32_t can_id_):hcan(hcan_), can_id(can_id_){
    if(hcan == &hcan1){
        if(Can1_Instances_Index < CAN_MAX_INSTANCES){       //将can1上的所有实例指针存入数组
            uint8_t i;
            for(i=0;i <= CanDevice::Can1_Instances_Index;i++){      //遍历数组查找是否有can_id相同的实例，有则跳出循环
			    if(Can1_Instances[i]->can_id == can_id_ || Can1_Instances[i] == nullptr){
					break;
				}
			}
			if(i == CanDevice::Can1_Instances_Index){    //遍历完如果i等于CanDevice::Can1_Instances_Index，则说明没有找到相同的实例，则存入数组
				Can1_Instances[Can1_Instances_Index] = this;
				Can1_Instances_Index++;
			}
        }
    }else if(hcan_ == &hcan2){
        if(Can2_Instances_Index < CAN_MAX_INSTANCES){       //将can2上的所有实例指针存入数组
            uint8_t i;
            for(i=0;i <= CanDevice::Can2_Instances_Index;i++){      //遍历数组查找是否有can_id相同的实例，有则跳出循环
			    if(Can2_Instances[i]->can_id == can_id_ || Can2_Instances[i] == nullptr){
					break;
				}
			}
			if(i == CanDevice::Can2_Instances_Index){    //遍历完如果i等于CanDevice::Can2_Instances_Index，则说明没有找到相同的实例，则存入数组
				Can2_Instances[Can2_Instances_Index] = this;
				Can2_Instances_Index++;
			}
        }
    }
}

void CanDevice::Can_Init(){
	CAN1_Filter_Init();
	CAN2_Filter_Init();
}

void CanDevice::CAN1_Filter_Init(){
   CAN_FilterTypeDef sFilterconfig;
	sFilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;  //32位过滤器
	sFilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;  //掩码模式
	sFilterconfig.FilterIdHigh = 0;
	sFilterconfig.FilterIdLow = 0;
	sFilterconfig.FilterMaskIdHigh = 0;    //不过滤任何ID号，即接收所有ID号的报文
	sFilterconfig.FilterMaskIdLow = 0;
	sFilterconfig.FilterBank = 0;
	sFilterconfig.FilterActivation = CAN_FILTER_ENABLE;   //启用过滤器
	sFilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;  //通过过滤器的报文放置FIFO0
	if(HAL_OK != HAL_CAN_ConfigFilter(&hcan1,&sFilterconfig)){
		Error_Handler();
	}

	if(HAL_OK != HAL_CAN_Start(&hcan1)){   //启动can
		Error_Handler();
	}
	
	if(HAL_OK != HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING)){  //使能fifo0接收中断
		Error_Handler();
	}
}

void CanDevice::CAN2_Filter_Init(){

    CAN_FilterTypeDef sFilterconfig;
	sFilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;  //32位过滤器
	sFilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;  //掩码模式
	sFilterconfig.FilterIdHigh = 0;
	sFilterconfig.FilterIdLow = 0;
	sFilterconfig.FilterMaskIdHigh = 0;    //不过滤任何ID号，即接收所有ID号的报文
	sFilterconfig.FilterMaskIdLow = 0;
	sFilterconfig.FilterBank = 14;
	sFilterconfig.FilterActivation = CAN_FILTER_ENABLE;   //启用过滤器
	sFilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;  //通过过滤器的报文放置FIFO0
    sFilterconfig.SlaveStartFilterBank = 14;  //can2的过滤器从14号开始,避免与can1的过滤器冲突
	if(HAL_OK != HAL_CAN_ConfigFilter(&hcan2, &sFilterconfig)){
		Error_Handler();
	}
	if(HAL_OK != HAL_CAN_Start(&hcan2)){   //启动can
		Error_Handler();
	}
	
	if(HAL_OK != HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING)){  //使能fifo0接收中断
		Error_Handler();
	}
}

void CanDevice::Can_update(uint8_t can_RxData[8],CAN_RxHeaderTypeDef* RxHeader){
    //具体实现在子类中，不写成纯虚函数是为了兼容不使用该函数的CAN设备
}

void CanDevice::Can_SendData(){
    //具体实现在子类中，不写成纯虚函数是为了兼容不使用该函数的CAN设备
}

extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
    if(hcan == &hcan1){
        CAN_RxHeaderTypeDef RxHeader1;
        if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader1, CanDevice::RxData1) != HAL_OK){
            Error_Handler();
        }
        for(uint8_t i = 0; i < CanDevice::Can1_Instances_Index; i++){
            if(CanDevice::Can1_Instances[i] != nullptr){
                CanDevice::Can1_Instances[i]->Can_update(CanDevice::RxData1, &RxHeader1);    //调用所有实例中的Can_update函数，如何识别是否是该实例所需的报文，由子类实现，比如判断stdid等
            }
        }
    }else if(hcan == &hcan2){
        CAN_RxHeaderTypeDef RxHeader2;
        if(HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &RxHeader2, CanDevice::RxData2) != HAL_OK){
            Error_Handler();
        }
        for(uint8_t i = 0; i < CanDevice::Can2_Instances_Index; i++){
            if(CanDevice::Can2_Instances[i] != nullptr){
                CanDevice::Can2_Instances[i]->Can_update(CanDevice::RxData2, &RxHeader2);
            }
        }
    }
}