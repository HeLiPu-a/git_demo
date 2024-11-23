#ifndef __VOFA_H
#define __VOFA_H
#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include  "SerialDevice.h"

/*引用外部文件end*/	

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*在此处进行宏定义：         begin*/	
#define Max_Channel_Num 8     //所需使用的通道个数
/*宏定义end*/	


/*在此处进枚举类型定义：         begin*/	
static enum rxState
{
    WAITING_FOR_HEADER_0,
    WAITING_FOR_ID,
    WAITING_FOR_DATA,
    WAITING_FOR_END_0,
    WAITING_FOR_END_1
}state_;
typedef struct  VOFA_rxData 
{
    /* data */
    uint8_t id;
    uint8_t Rxdata_buf[4];
    float   RxFloat_buf[6];
    uint8_t index=0;
}VOFA_rxData_t;

/*枚举定义end*/	


/*在此处进行类和结构体的定义：begin*/	

/*类和结构体定义end*/	
class VOFA :public SerialDevice
{
public:
    static UART_HandleTypeDef* huart_;
    static VOFA* VOFA_instances_buf[Max_Channel_Num];
    static uint8_t VOFA_instances_count;//记录实际的通道个数
    static bool VOFA_Error_flag;
    float Txdata_buf[Max_Channel_Num];
    VOFA_rxData_t rxData_;
    VOFA(UART_HandleTypeDef *huartx);
    float process_data();
    static void VOFA_SendEND();
	void handleReceiveData(uint8_t byte);
   
};

/*在此处进行函数定义：       begin*/	

/*函数定义end*/	

#endif

#endif 
