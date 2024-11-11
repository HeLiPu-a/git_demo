#include "CanDevice.h"

uint8_t CanManager::RxData1[8]={0};
uint8_t CanManager::RxData2[8]={0};

CanDevice* CanDevice::Can1_Instances1[MAX_INSTANCES+1] = {nullptr};
CanDevice* CanDevice::Can1_Instances2[MAX_INSTANCES+1] = {nullptr};
CanDevice* CanDevice::Can2_Instances1[MAX_INSTANCES+1] = {nullptr};
CanDevice* CanDevice::Can2_Instances2[MAX_INSTANCES+1] = {nullptr};

CanDevice::CanDevice(CAN_HandleTypeDef *hcan_, uint8_t can_id_):hcan(hcan_), can_id(can_id_){
    if(hcan_ == &hcan1){
        if(can_id_ <= 4){
            Can1_Instances1[can_id_] = this;
        }else if(can_id_ <= 8){
            Can1_Instances2[can_id_-4] = this;
        }
    }else if(hcan_ == &hcan2){
        if(can_id_ <= 4){
            Can2_Instances1[can_id_] = this;
        }else if(can_id_ <= 8){
            Can2_Instances2[can_id_-4] = this;
        }
    }

    CAN1_Filter_Init();
    CAN2_Filter_Init();
}

void CanDevice::CAN1_Filter_Init(){
    CAN_FilterTypeDef  sFilterConfig;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterIdHigh = 0x00;
    sFilterConfig.FilterIdLow = 0x00;
    sFilterConfig.FilterMaskIdHigh = 0x00;
    sFilterConfig.FilterMaskIdLow = 0x00;
    sFilterConfig.SlaveStartFilterBank = 14;
    if(HAL_CAN_ConfigFilter(hcan1, &sFilterConfig)!= HAL_OK){
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
        /* Start Error */
        Error_Handler();
    }

     if (HAL_CAN_Start(&hcan1) != HAL_OK)
    {
        /* Start Error */
        Error_Handler();
    }
}

void CanDevice::CAN2_Filter_Init(){

    CAN_FilterTypeDef  sFilterConfig;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.FilterMode = FILTER_MODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterBank = 14;
    sFilterConfig.FilterIdHigh = 0x00;
    sFilterConfig.FilterIdLow = 0x00;
    sFilterConfig.FilterMaskIdHigh = 0x00;
    sFilterConfig.FilterMaskIdLow = 0x00;
    
    if(HAL_CAN_ConfigFilter(hcan2, &sFilterConfig)!= HAL_OK){
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
        /* Start Error */
        Error_Handler();
    }

    if (HAL_CAN_Start(&hcan2) != HAL_OK)
    {
        /* Start Error */
        Error_Handler();
    }
}

CanManager::CanManager(){
    TxHeader1.StdId = 0x200;
    TxHeader1.ExtId = 0x00;
    TxHeader1.RTR = CAN_RTR_DATA;
    TxHeader1.IDE = CAN_ID_STD;
    TxHeader1.DLC = 8;
    TxHeader.TransmitGlobalTime = DISABLE;

    TxHeader2.StdId = 0x1FF;
    TxHeader2.ExtId = 0x00;
    TxHeader2.RTR = CAN_RTR_DATA;
    TxHeader2.IDE = CAN_ID_STD;
    TxHeader2.DLC = 8;
    TxHeader2.TransmitGlobalTime = DISABLE;

}

void CanManager::process_data(){
    for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
        int16_t temp_vcurrent = 0;
        if(CanDevice::Can1_Instances1[i] != nullptr){
            temp_vcurrent = CanDevice::Can1_Instances1[i]->motor_process();
        }
        send_buf1[2 * i - 2] = (uint8_t)(temp_vcurrent >> 8);
        send_buf1[2 * i - 1] = (uint8_t)temp_vcurrent;
    }
    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader1, send_buf1, &msg_box1) == HAL_ERROR)
    {
        Error_Handler();
        // Failed to add message to the transmit mailbox
    }

    for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
        int16_t temp_vcurrent = 0;
        if(CanDevice::Can1_Instances2[i] != nullptr){
            temp_vcurrent = CanDevice::Can1_Instances2[i]->motor_process();
        }
        send_buf1[2 * i - 2] = (uint8_t)(temp_vcurrent >> 8);
        send_buf1[2 * i - 1] = (uint8_t)temp_vcurrent;
    }
    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader2, send_buf1, &msg_box1) == HAL_ERROR)
    {
        Error_Handler();
        // Failed to add message to the transmit mailbox
    }

    for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
        int16_t temp_vcurrent = 0;
        if(CanDevice::Can2_Instances1[i] != nullptr){
            temp_vcurrent = CanDevice::Can2_Instances1[i]->motor_process();
        }
        send_buf2[2 * i - 2] = (uint8_t)(temp_vcurrent >> 8);
        send_buf2[2 * i - 1] = (uint8_t)temp_vcurrent;
    }
    if (HAL_CAN_AddTxMessage(&hcan2, &TxHeader1, send_buf2, &msg_box2) == HAL_ERROR)
    {
        Error_Handler();
        // Failed to add message to the transmit mailbox
    }

    for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
        int16_t temp_vcurrent = 0;
        if(CanDevice::Can2_Instances2[i] != nullptr){
            temp_vcurrent = CanDevice::Can2_Instances2[i]->motor_process();
        }
        send_buf1[2 * i - 2] = (uint8_t)(temp_vcurrent >> 8);
        send_buf1[2 * i - 1] = (uint8_t)temp_vcurrent;
    }
    if (HAL_CAN_AddTxMessage(&hcan2, &TxHeader2, send_buf2, &msg_box2) == HAL_ERROR)
    {
        Error_Handler();
        // Failed to add message to the transmit mailbox
    }
}

extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
    if(hcan == &hcan1){

        CAN_RxHeaderTypeDef RxHeader1;
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader1, CanManager::RxData1);

        for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
            if(CanDevice::Can1_Instances1[i]->can_id == RxHeader1.StdId - 0x200){
                CanDevice::Can1_Instances1[i]->can_update(CanManager::RxData1);
            }
        }

        for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
            if(CanDevice::Can1_Instances2[i]->can_id == RxHeader1.StdId - 0x200){
                CanDevice::Can1_Instances2[i]->can_update(CanManager::RxData1);
            }
        }

    }else if(hcan == &hcan2){

        CAN_RxHeaderTypeDef RxHeader2;
        HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &RxHeader2, CanManager::RxData2);

        for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
            if(CanDevice::Can2_Instances1[i]->can_id == RxHeader2.StdId - 0x200){
                CanDevice::Can2_Instances1[i]->can_update(CanManager::RxData2);
            }
        }

        for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
            if(CanDevice::Can2_Instances2[i]->can_id == RxHeader2.StdId - 0x200){
                CanDevice::Can2_Instances2[i]->can_update(CanManager::RxData2);
            }
        }
    }    
}