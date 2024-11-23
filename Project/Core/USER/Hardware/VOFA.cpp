#include "VOFA.h"
VOFA* VOFA::VOFA_instances_buf[Max_Channel_Num] = {nullptr};
uint8_t VOFA::VOFA_instances_count=0;//记录实际的通道个数
bool VOFA::VOFA_Error_flag=0;
UART_HandleTypeDef* VOFA::huart_=nullptr;
VOFA::VOFA(UART_HandleTypeDef *huartx)
     :SerialDevice(huartx)
{
    if(VOFA_instances_count>Max_Channel_Num)
    {//当指定的通道超过最大值或者小于1时，报错并返回
        VOFA_Error_flag=1;
        return;
    }
    //由于是所有实例个公用一个串口，所以需要判断串口是否一致
    if(VOFA_instances_count !=0 && huartx != huart_)
    {
        VOFA_Error_flag=1;
        return;
    }
    //将实例的指针放入指针数组中
		huart_ = huartx;
    VOFA_instances_buf[VOFA_instances_count]=this;
    VOFA_instances_count++;
} 

/* VOFA的协议允许没有包头，数据段长度任意，只识别帧的结尾
将发送帧尾单独做成一个函数，从而实现通道数目可任意（当然不能超过最大量）
只要满足该函数 ！！慢于！！ SendFloat函数调用即可*/
void VOFA::VOFA_SendEND()
{
    uint8_t END_buf[4]={0x00,0x00,0x80,0x7F};
    if(HAL_UART_Transmit(huart_,END_buf,4, 10) != HAL_OK)
		{
			VOFA_Error_flag=1;
      return;
		}
}

/* 这个拆包函数要根据VOFA定义的协议来写，
   VOFA那边设置的包头要和这边的对应 */
void VOFA::handleReceiveData(uint8_t byte) 
{
	 switch(state_)
	 {
		 case WAITING_FOR_HEADER_0:
            //接收到0xFF，说明接收到指令
            if(byte == 0xFA)
            {
                state_ = WAITING_FOR_ID;
            }
         break;
		 case WAITING_FOR_ID:
           /*接收到ID，判断是否是为8条指令中的某一条 
		       其中0x06是预留的指令，还没进行编写*/
            if(byte >= 0x00 && byte <= 0x07)
            {	
								rxData_.id = byte;
                state_ = WAITING_FOR_DATA;
            }
            else state_ = WAITING_FOR_HEADER_0;
         break;
		 case WAITING_FOR_DATA:
            //接收到数据，处理数据 
            if(rxData_.index<4)
            {	/* 由于发送过来的是一个浮点型的数据，
							所以用四字节长度的数组接收，后面合并为float*/
               rxData_.Rxdata_buf[rxData_.index] = byte;
               rxData_.index++;
            }
            if(rxData_.index == 4)
            {
                rxData_.index=0;
                state_ = WAITING_FOR_END_0;
            }
         break;
         case WAITING_FOR_END_0:
            //接收到0x00，说明接收到第一个包尾
             if(byte == 0xDD)
             {
                 state_ = WAITING_FOR_END_1;
             }
             else
            {
                state_ = WAITING_FOR_HEADER_0; 
            }
         break;
		 case WAITING_FOR_END_1:
            //接收到0x7F，说明接收到第二个包尾
            if(byte == 0xFF)
             {
							 if(rxData_.id !=7)
							 {
								 /*将刚才接收到的4字节数据转换为Float类型
								  并根据ID号填充到对应数组的位置里
								 */
								memcpy(&rxData_.RxFloat_buf[rxData_.id],
											  rxData_.Rxdata_buf, 4 );
							 }
               else //第七条指令为停止指令，直接对Float_buf[0]（对应target）里面写0
							{
							 rxData_.RxFloat_buf[0] = 0; 
							}
                state_ = WAITING_FOR_HEADER_0;
             }
            else
            {
                state_ = WAITING_FOR_HEADER_0;
            }
         break;
		 
         default:
	     break;
	 }
}