#ifndef __SERIALDEVICE_H
#define __SERIALDEVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include "stm32f4xx_hal.h"
#include "usart.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "SEGGER_RTT.h"
#include "FreeRTOS.h"
/*引用外部文件end*/	

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*在此处进行宏定义：         begin*/	
#define RX_BUFFER_SIZE 1
#define Max_Package_Length 128

#define MAX_INSTANCES 8

#define ERROR_LOG

/*宏定义end*/	


/*在此处进枚举类型定义：         begin*/	
#define Error_huart_init 0
typedef enum
{
	Event_OK = 1,
	Event_Error = 2,
	Event_NoDef = 3
}Event_Status_t;	

typedef enum
{
	Receive_Not_OK    = 0,
	Receive_Incorrect = 1,
	Receive_OK        = 2,
}Receive_Status;

/*枚举定义end*/	


/*在此处进行类和结构体的定义：begin*/



class SerialDevice
{
   public:
   uint8_t Frame_length  = 0;
   uint8_t active_index  = 0;
   uint8_t rxBuffer_[Max_Package_Length];
   uint8_t rxBuffer2_[Max_Package_Length];
   uint8_t* activeBuffer = rxBuffer_;
   UART_HandleTypeDef *huart_; 					  // 保存 UART 句柄
    
   bool init_status = false;
  
   uint8_t receive_ok_flag = 0;	  				    //接收完成的标志
   uint8_t RxPK_ok_flag    = 0;  	//接收时的帧格式（包）完成的标志
   static SerialDevice *instances_[MAX_INSTANCES]; // 保存所有实例(最多八个)
   static int instanceCount_;                      // 记录保存实例个数
	
    //构造函数，将创建出来的实例和串口进行绑定
    SerialDevice(UART_HandleTypeDef *huartx);   

    bool SendByte   (uint8_t  data);
    bool SendString (char    *data);
    bool SendArray  (uint8_t *data,
				  uint8_t data_len);
    bool SendFloat  (float    data);
    bool SendInt32  (int32_t  data);
    bool SendInt16  (int16_t  data);
    
			void 		   startUartReceiveIT();	
			void  		   startUartReceiveIT(uint8_t DMA_Frame_length);
    virtual void 		   handleReceiveData(uint8_t byte);//串口接收数据处理函数,可以在子类中根据对应模块的通信协议来实现具体逻辑
			void 		   Change_DisBuf();
	virtual Event_Status_t SendEvent_Msg(BaseType_t *pxHigherPriorityTaskWoken);
			
    //static void registerInstance(SerialDevice *instance);
};
/*类和结构体定义end*/	


/*在此处进行函数定义：       begin*/	
uint16_t CRC16_Table(uint8_t *p, uint8_t counter);
uint8_t  CRC8_Table (uint8_t *p, uint8_t counter);
/*函数定义end*/	

#endif

#endif