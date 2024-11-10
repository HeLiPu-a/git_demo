
bool SendPackage(Package    *package);
//发送数据包，可以根据自己的模块进行而修改、适配
bool SerialDevice::SendPackage(Package *package)
{
    if (huart_ == nullptr) {
        return false;  // 如果 UART 句柄为空，返回失败
    }
    uint8_t sendBuffer[Max_Package_Length];
    sendBuffer[0] = (*package).Head_0;
    sendBuffer[1] = (*package).Head_1;
    sendBuffer[2] = (*package).frame_id;
    sendBuffer[3] = (*package).length;
    for(uint8_t i = 0;i<(*package).length;i++)//向驮载数据的数组填充数据
    {
        sendBuffer[4+i] = (*package).PK_Data.u8_PK_data[i];
    }
    /*计算CRC校验值,存入到联合体check_code中，由于联合体是共享内存，
    所以crc_buff[0]刚好是crc_code的低8位，[1]为高八位*/
    (*package).check_code.crc_code = CRC16_Table((*package).PK_Data.u8_PK_data, 
                                                 (*package).length);
    sendBuffer[4+(*package).length] =  (*package).check_code.crc_buff[0];
    sendBuffer[5+(*package).length] =  (*package).check_code.crc_buff[1];
    sendBuffer[6+(*package).length] =  (*package).End_0;
    sendBuffer[7+(*package).length] =  (*package).End_1;    

    HAL_UART_Transmit(huart_, sendBuffer, (*package).length + 8, HAL_MAX_DELAY);
    return true;
}

//接收的状态机的状态位，可以根据自己模块来编写
 enum rxState
 {
     WAITING_FOR_HEADER_0,
     WAITING_FOR_HEADER_1,
     WAITING_FOR_ID,
     WAITING_FOR_LENGTH,
     WAITING_FOR_DATA,
     WAITING_FOR_CRC_0,
     WAITING_FOR_CRC_1,
     WAITING_FOR_END_0,
     WAITING_FOR_END_1
 } state_;

//接收或者是发送的包格式，写成结构体方便管理
  typedef struct _Package
 {
     uint8_t  Head_0;
     uint8_t  Head_1;    
 	 uint8_t  End_0;
     uint8_t  End_1;
     uint8_t  frame_id;
     uint8_t  length;
     uint8_t  rxIndex=0;

     union _PK_Data
     {
         uint8_t   u8_PK_data   [Max_Package_Length];
         int32_t   int32_PK_data[Max_Package_Length/4];
         int16_t   int16_PK_data[Max_Package_Length/2];
         uint32_t  u32_PK_data  [Max_Package_Length/4];
         uint16_t  u16_PK_data  [Max_Package_Length/2];
         float     float_PK_data[Max_Package_Length/4];
     }PK_Data;//同个实例的包只有可能接收一种类型的数据，所以用union成员是共享一块空间，节省空间，同时易于类型转换
     union _check_code
     {
         uint16_t crc_code;
         uint8_t  crc_buff[2]; // CRC 校验的字节形式
     }check_code;   //虽然不知道是要干什么的，但是先复制过来
     uint16_t crc_calculated = 0;

 }Package;

//到时候在子类的定义中实例化接收帧
Package rx_frame_mat;

 //接收数据的具体流程
  void SerialDevice::handleReceiveData(uint8_t byte)
{
    if( 1 == receive_ok_flag )
    {   
        /*当receive_ok_flag=1时，说明数据已经接收完毕但是未更新完毕
        此时若是重新接收，可能导致原先的数据被覆盖，故直接返回，这么做的弊端是会丢数据
        另一个方法是启用DMA来接收数据，还没实现，有时间再来搞
			*/
        
        return;
    }
    switch (state_)
    {
    case WAITING_FOR_HEADER_0:
        if (byte == rx_frame_mat.Head_0)
        {
						rx_frame_mat.Head_0 = byte; // 存储一号帧头
            state_ = WAITING_FOR_HEADER_1;
            
        }
        break;
    case WAITING_FOR_HEADER_1:
        if (byte == rx_frame_mat.Head_1)
        {
						 rx_frame_mat.Head_1 = byte; // 存储二号帧头
            state_ = WAITING_FOR_ID;
        }
        else
        {
            state_ = WAITING_FOR_HEADER_0;
        }
        break;
    case WAITING_FOR_ID:
        rx_frame_mat.frame_id = byte; // 存储帧ID
        state_ = WAITING_FOR_LENGTH;
        break;
    case WAITING_FOR_LENGTH:
        rx_frame_mat.length = byte; // 存储数据长度
        rx_frame_mat.rxIndex = 0;
        state_ = WAITING_FOR_DATA;
        break;
    case WAITING_FOR_DATA:
    /*存储接收到的数据，存入到联合体的数据成员u8_PK_data数组中，如果接收的数据本身是其他类型的数据，
    则在函数之后再进行转换。（直接利用联合体的共享空间特性）*/
        rx_frame_mat.PK_Data.u8_PK_data[rx_frame_mat.rxIndex++] = byte;
        if (rx_frame_mat.rxIndex >= rx_frame_mat.length)
        {
            state_ = WAITING_FOR_CRC_0;
        }
        break;
    case WAITING_FOR_CRC_0:
        rx_frame_mat.check_code.crc_buff[0] = byte; // 存储 CRC 校验的高字节
        state_ = WAITING_FOR_CRC_1;
        break;
    case WAITING_FOR_CRC_1:
        rx_frame_mat.check_code.crc_buff[1] = byte; // 存储 CRC 校验的低字节
        state_ = WAITING_FOR_END_0;
        break;
    case WAITING_FOR_END_0:
        if (byte == rx_frame_mat.End_0)
        {
            state_ = WAITING_FOR_END_1;
            rx_frame_mat.End_0 = byte; // 存储帧尾
        }
        else
        {
            state_ = WAITING_FOR_HEADER_0;
        }
        break;
    case WAITING_FOR_END_1:
        if (byte == rx_frame_mat.End_1){
        
            rx_frame_mat.End_1 = byte; // 存储帧尾
            //如果开启CRC校验就就进入校验
            if (enableCrcCheck_){
            
                // 计算 CRC 并与接收到的 CRC 进行比较
                rx_frame_mat.crc_calculated = CRC16_Table(rx_frame_mat.PK_Data.u8_PK_data, 
                                                          rx_frame_mat.length);
                //如果数据校验失败，则丢弃该帧，重新等待帧头
                if (rx_frame_mat.crc_calculated != rx_frame_mat.check_code.crc_code){
                
                    state_ = WAITING_FOR_HEADER_0;
                    break;
                }
            }   //前面的都通过之后，说明接收数据完成，置标志位，并等待下一帧
                state_ = WAITING_FOR_HEADER_0;
                receive_ok_flag = 1;//接收完毕之后置标志位，同时在子类中实现更新数据的函数并调用
                //同时需要强调的是，一定要在更新数据函数的最后将该标志位重新赋值为0！！，否则无法再次接收数据
        }
        else 
        {   //帧尾2错误，则丢弃该帧，重新等待包头
            state_ = WAITING_FOR_HEADER_0;
        }
        break;
    default:
        state_ = WAITING_FOR_HEADER_0;
        break;
    }//end_switch
    
}