#include "SerialDevice.h"
//初始化静态成员
SerialDevice* SerialDevice::instances_[MAX_INSTANCES]={nullptr}; 
int SerialDevice::instanceCount_=0;    
SerialDevice::SerialDevice(UART_HandleTypeDef *huartx,
                           bool enableCrcCheck)
                           :enableCrcCheck_(enableCrcCheck)
{
    if(instanceCount_>MAX_INSTANCES)
    {
        init_status = false;
        return;
    }
    for(uint8_t i = 0; i < MAX_INSTANCES; i++)
    {
        if(instances_[i]->huart_ == huartx)
        {
            init_status = false;
            return;
        }
    }
    huart_ = huartx;
    instances_[instanceCount_] = this;
    instanceCount_++;
    init_status = true;
}

void SerialDevice::SetRxPackage_identity(uint8_t head1,uint8_t head2,
            uint8_t end1,uint8_t end2 ,uint8_t id   ,uint8_t length)
{
    rx_frame_mat.Head_1 = head1;
    rx_frame_mat.Head_2 = head2;
    rx_frame_mat.End_1  = end1;
    rx_frame_mat.End_2  = end2;
    rx_frame_mat.frame_id = id;
    rx_frame_mat.length = length;
}

bool SerialDevice::SendByte(uint8_t data)
{
    if (huart_ == nullptr) {
        return false;  // 如果 UART 句柄为空，返回失败
    }
    // 使用 HAL 库发送单个字节
    if (HAL_UART_Transmit(huart_, &data, 1, 500) != HAL_OK) {
        return false;  // 发送失败
    }
    return true;  // 发送成功
}

bool SerialDevice::SendString(char *data)
{
    if (huart_ == nullptr || data == nullptr) {
        return false;  // 如果 UART 句柄或数据指针为空，返回失败
    }

    // 获取字符串长度
    size_t length = strlen(data);

    // 使用 HAL 库发送字符串
    if (HAL_UART_Transmit(huart_, (uint8_t *)data, length, HAL_MAX_DELAY) != HAL_OK) {
        return false;  // 发送失败
    }

    return true;  // 发送成功
}

bool SerialDevice::SendArray(uint8_t *data, uint8_t data_len)
{
    if (huart_ == nullptr || data == nullptr) {
        return false;  // 如果 UART 句柄或数据指针为空，返回失败
    }

    // 使用 HAL 库发送数组
    if (HAL_UART_Transmit(huart_, data, data_len, HAL_MAX_DELAY) != HAL_OK) {
        return false;  // 发送失败
    }

    return true;  // 发送成功
}

bool SerialDevice::SendFloat(float data)
{
    if (huart_ == nullptr) {
        return false;  // 如果 UART 句柄为空，返回失败
    }

    // 将浮点数转换为字节数组
    uint8_t buffer[4];
    memcpy(buffer, &data, sizeof(float));

    // 使用 HAL 库发送字节数组
    if (HAL_UART_Transmit(huart_, buffer, sizeof(float), HAL_MAX_DELAY) != HAL_OK) {
        return false;  // 发送失败
    }

    return true;  // 发送成功
}

void SerialDevice::startUartReceiveIT()
{
    HAL_UART_Receive_IT(huart_, rxBuffer_, RX_BUFFER_SIZE);
}

bool SerialDevice::SendInt32(int32_t data)
{
    if (huart_ == nullptr) {
        return false;  // 如果 UART 句柄为空，返回失败
    }

    // 将 int32_t 转换为字节数组
    uint8_t buffer[4];
    memcpy(buffer, &data, sizeof(int32_t));

    // 使用 HAL 库发送字节数组
    if (HAL_UART_Transmit(huart_, buffer, sizeof(int32_t), HAL_MAX_DELAY) != HAL_OK) {
        return false;  // 发送失败
    }

    return true;  // 发送成功
}

bool SerialDevice::SendInt16(int16_t data)
{
    if (huart_ == nullptr) {
        return false;  // 如果 UART 句柄为空，返回失败
    }

    // 将 int16_t 转换为字节数组
    uint8_t buffer[2];
    memcpy(buffer, &data, sizeof(int16_t));

    // 使用 HAL 库发送字节数组
    if (HAL_UART_Transmit(huart_, buffer, sizeof(int16_t), HAL_MAX_DELAY) != HAL_OK) {
        return false;  // 发送失败
    }

    return true;  // 发送成功
}
//未完成
bool SerialDevice::SendPackage(Package *package)
{
    if (huart_ == nullptr) {
        return false;  // 如果 UART 句柄为空，返回失败
    }
    uint8_t sendBuffer[Max_Package_Length];
    sendBuffer[0] = (*package).Head_1;
    sendBuffer[1] = (*package).Head_2;
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
    sendBuffer[6+(*package).length] =  (*package).End_1;
    sendBuffer[7+(*package).length] =  (*package).End_2;    

    HAL_UART_Transmit(huart_, sendBuffer, (*package).length + 8, HAL_MAX_DELAY);
    return true;
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8_t rxByte;
    if(huart == &huart6)
    {
        SerialDevice::instances_[0]->SendByte(0x22);
        HAL_UART_Receive_IT(&huart6, SerialDevice::instances_[0]->rxBuffer_, RX_BUFFER_SIZE);
    }
    // // 获取 UART 接收的字节
    // for (int i = 0; i < SerialDevice::instanceCount_; i++)
    // {
    //     if (SerialDevice::instances_[i]->huart_ == huart)
    //     {
    //         rxByte = SerialDevice::instances_[i]->rxBuffer_[0]; // 读取接收缓冲区中的字
    //         SerialDevice::instances_[i]->handleReceiveData(rxByte);
    //         HAL_UART_Receive_IT(huart, SerialDevice::instances_[i]->rxBuffer_, RX_BUFFER_SIZE);
    //     }
    // }
}

void SerialDevice::handleReceiveData(uint8_t byte)
{
    if( 1 == receive_ok_flag)
    {   
        /*当receive_ok_flag=1时，说明数据已经接收完毕但是未更新完毕
        此时若是重新接收，可能导致原先的数据被覆盖，故直接返回，这么做的弊端是会丢数据
        另一个方法是启用DMA来接收数据，还没实现，有时间再来搞*/
        
        return;
    }
    switch (state_)
    {
    case WAITING_FOR_HEADER_0:
        if (byte == rx_frame_mat.Head_1)
        {
            state_ = WAITING_FOR_HEADER_1;
            rx_frame_mat.Head_1 = byte; // 存储一号帧头
        }
        break;
    case WAITING_FOR_HEADER_1:
        if (byte == rx_frame_mat.Head_2)
        {
            state_ = WAITING_FOR_ID;
            rx_frame_mat.Head_2 = byte; // 存储二号帧头
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
        if (byte == rx_frame_mat.End_1)
        {
            state_ = WAITING_FOR_END_1;
            rx_frame_mat.End_1 = byte; // 存储帧尾
        }
        else
        {
            state_ = WAITING_FOR_HEADER_0;
        }
        break;
    case WAITING_FOR_END_1:
        if (byte == rx_frame_mat.End_2){
        
            rx_frame_mat.End_2 = byte; // 存储帧尾
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


// CRC16 查表
static const uint16_t CRC16Table[256] = {
    // CRC16 table as defined previously
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0};

// 计算 CRC16 校验码
uint16_t CRC16_Table(uint8_t *p, uint8_t counter)
{
    uint16_t crc = 0;
    for (uint8_t i = 0; i < counter; i++)
    {
        crc = CRC16Table[((crc >> 8) ^ p[i]) & 0xFF] ^ (crc << 8);
    }
    return crc;
}

static const uint8_t CRC8Table[] = {
    0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83, 0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
    0x9D, 0xC3, 0x21, 0x7F, 0xFC, 0xA2, 0x40, 0x1E, 0x5F, 0x01, 0xE3, 0xBD, 0x3E, 0x60, 0x82, 0xDC,
    0x23, 0x7D, 0x9F, 0xC1, 0x42, 0x1C, 0xFE, 0xA0, 0xE1, 0xBF, 0x5D, 0x03, 0x80, 0xDE, 0x3C, 0x62,
    0xBE, 0xE0, 0x02, 0x5C, 0xDF, 0x81, 0x63, 0x3D, 0x7C, 0x22, 0xC0, 0x9E, 0x1D, 0x43, 0xA1, 0xFF,
    0x46, 0x18, 0xFA, 0xA4, 0x27, 0x79, 0x9B, 0xC5, 0x84, 0xDA, 0x38, 0x66, 0xE5, 0xBB, 0x59, 0x07,
    0xDB, 0x85, 0x67, 0x39, 0xBA, 0xE4, 0x06, 0x58, 0x19, 0x47, 0xA5, 0xFB, 0x78, 0x26, 0xC4, 0x9A,
    0x65, 0x3B, 0xD9, 0x87, 0x04, 0x5A, 0xB8, 0xE6, 0xA7, 0xF9, 0x1B, 0x45, 0xC6, 0x98, 0x7A, 0x24,
    0xF8, 0xA6, 0x44, 0x1A, 0x99, 0xC7, 0x25, 0x7B, 0x3A, 0x64, 0x86, 0xD8, 0x5B, 0x05, 0xE7, 0xB9,
    0x8C, 0xD2, 0x30, 0x6E, 0xED, 0xB3, 0x51, 0x0F, 0x4E, 0x10, 0xF2, 0xAC, 0x2F, 0x71, 0x93, 0xCD,
    0x11, 0x4F, 0xAD, 0xF3, 0x70, 0x2E, 0xCC, 0x92, 0xD3, 0x8D, 0x6F, 0x31, 0xB2, 0xEC, 0x0E, 0x50,
    0xAF, 0xF1, 0x13, 0x4D, 0xCE, 0x90, 0x72, 0x2C, 0x6D, 0x33, 0xD1, 0x8F, 0x0C, 0x52, 0xB0, 0xEE,
    0x32, 0x6C, 0x8E, 0xD0, 0x53, 0x0D, 0xEF, 0xB1, 0xF0, 0xAE, 0x4C, 0x12, 0x91, 0xCF, 0x2D, 0x73,
    0xCA, 0x94, 0x76, 0x28, 0xAB, 0xF5, 0x17, 0x49, 0x08, 0x56, 0xB4, 0xEA, 0x69, 0x37, 0xD5, 0x8B,
    0x57, 0x09, 0xEB, 0xB5, 0x36, 0x68, 0x8A, 0xD4, 0x95, 0xCB, 0x29, 0x77, 0xF4, 0xAA, 0x48, 0x16,
    0xE9, 0xB7, 0x55, 0x0B, 0x88, 0xD6, 0x34, 0x6A, 0x2B, 0x75, 0x97, 0xC9, 0x4A, 0x14, 0xF6, 0xA8,
    0x74, 0x2A, 0xC8, 0x96, 0x15, 0x4B, 0xA9, 0xF7, 0xB6, 0xE8, 0x0A, 0x54, 0xD7, 0x89, 0x6B, 0x35};

uint8_t CRC8_Table(uint8_t *p, uint8_t counter)
{
    uint8_t crc8 = 0;
    for (uint8_t i = 0; i < counter; i++)
    {
        crc8 = CRC8Table[crc8 ^ p[i]];
    }
    return crc8;
}

