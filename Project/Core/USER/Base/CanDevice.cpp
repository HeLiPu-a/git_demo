#include "CanDevice.h"

uint8_t CanManager::RxData1[8]={0};
uint8_t CanManager::RxData2[8]={0};

CanDevice* CanDevice::Can1_Instances[MAX_INSTANCES] = {nullptr};
CanDevice* CanDevice::Can2_Instances[MAX_INSTANCES] = {nullptr};

CanDevice::CanDevice(CAN_HandleTypeDef *hcan_, uint8_t can_id_):hcan(hcan_), can_id(can_id_){
    if(hcan_ == &hcan1){
        if(Can1_instance_index < MAX_INSTANCES){
            int insertPos = Can1_instance_index;
            for (int i = 0; i < Can1_instance_index; ++i)
            {
                if (Can1_Instances[i]->can_id > can_id_)
                {
                    insertPos = i;
                    break;
                }
            }

            for (int i = Can1_instance_index; i > insertPos; --i)
            {
                Can1_Instances[i] = Can1_Instances[i - 1];
            }

            Can1_Instances[insertPos] = this;
            Can1_instance_index++;
        }
    }else if(hcan_ == &hcan2){
        if(Can2_instance_index < MAX_INSTANCES){
            int insertPos = Can2_instance_index;
            for (int i = 0; i < Can2_instance_index; ++i)
            {
                if (Can2_Instances[i]->can_id > can_id_)
                {
                    insertPos = i;
                    break;
                }
            }

            for (int i = Can2_instance_index; i > insertPos; --i)
            {
                Can2_Instances[i] = Can2_Instances[i - 1];
            }

            Can2_Instances[insertPos] = this;
            Can2_instance_index++;
        }
    }

    

    CAN1_Filter_Init();
    CAN2_Filter_Init();
}

CanDevice::CAN1_Filter_Init(){
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

CanDevice::CAN2_Filter_Init(){
    CAN_FilterTypeDef  sFilterConfig;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.FilterBank = 14;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterIdHigh = 0x00;
    sFilterConfig.FilterIdLow = 0x00;
    sFilterConfig.FilterMaskIdHigh = 0x00;
    sFilterConfig.FilterMaskIdLow = 0x00;
    sFilterConfig.FilterMode = FILTER_MODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;

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

CanManager::process_data(){
    
}