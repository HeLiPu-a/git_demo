
bool SendPackage(Package    *package);
//�������ݰ������Ը����Լ���ģ����ж��޸ġ�����
bool SerialDevice::SendPackage(Package *package)
{
    if (huart_ == nullptr) {
        return false;  // ��� UART ���Ϊ�գ�����ʧ��
    }
    uint8_t sendBuffer[Max_Package_Length];
    sendBuffer[0] = (*package).Head_0;
    sendBuffer[1] = (*package).Head_1;
    sendBuffer[2] = (*package).frame_id;
    sendBuffer[3] = (*package).length;
    for(uint8_t i = 0;i<(*package).length;i++)//���������ݵ������������
    {
        sendBuffer[4+i] = (*package).PK_Data.u8_PK_data[i];
    }
    /*����CRCУ��ֵ,���뵽������check_code�У������������ǹ����ڴ棬
    ����crc_buff[0]�պ���crc_code�ĵ�8λ��[1]Ϊ�߰�λ*/
    (*package).check_code.crc_code = CRC16_Table((*package).PK_Data.u8_PK_data, 
                                                 (*package).length);
    sendBuffer[4+(*package).length] =  (*package).check_code.crc_buff[0];
    sendBuffer[5+(*package).length] =  (*package).check_code.crc_buff[1];
    sendBuffer[6+(*package).length] =  (*package).End_0;
    sendBuffer[7+(*package).length] =  (*package).End_1;    

    HAL_UART_Transmit(huart_, sendBuffer, (*package).length + 8, HAL_MAX_DELAY);
    return true;
}

//���յ�״̬����״̬λ�����Ը����Լ�ģ������д
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

//���ջ����Ƿ��͵İ���ʽ��д�ɽṹ�巽�����
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
     }PK_Data;//ͬ��ʵ���İ�ֻ�п��ܽ���һ�����͵����ݣ�������union��Ա�ǹ���һ��ռ䣬��ʡ�ռ䣬ͬʱ��������ת��
     union _check_code
     {
         uint16_t crc_code;
         uint8_t  crc_buff[2]; // CRC У����ֽ���ʽ
     }check_code;   //��Ȼ��֪����Ҫ��ʲô�ģ������ȸ��ƹ���
     uint16_t crc_calculated = 0;

 }Package;

//��ʱ��������Ķ�����ʵ��������֡
Package rx_frame_mat;

 //�������ݵľ�������
  void SerialDevice::handleReceiveData(uint8_t byte)
{
    if( 1 == receive_ok_flag )
    {   
        /*��receive_ok_flag=1ʱ��˵�������Ѿ�������ϵ���δ�������
        ��ʱ�������½��գ����ܵ���ԭ�ȵ����ݱ����ǣ���ֱ�ӷ��أ���ô���ı׶��ǻᶪ����
        ��һ������������DMA���������ݣ���ûʵ�֣���ʱ��������
			*/
        
        return;
    }
    switch (state_)
    {
    case WAITING_FOR_HEADER_0:
        if (byte == rx_frame_mat.Head_0)
        {
						rx_frame_mat.Head_0 = byte; // �洢һ��֡ͷ
            state_ = WAITING_FOR_HEADER_1;
            
        }
        break;
    case WAITING_FOR_HEADER_1:
        if (byte == rx_frame_mat.Head_1)
        {
						 rx_frame_mat.Head_1 = byte; // �洢����֡ͷ
            state_ = WAITING_FOR_ID;
        }
        else
        {
            state_ = WAITING_FOR_HEADER_0;
        }
        break;
    case WAITING_FOR_ID:
        rx_frame_mat.frame_id = byte; // �洢֡ID
        state_ = WAITING_FOR_LENGTH;
        break;
    case WAITING_FOR_LENGTH:
        rx_frame_mat.length = byte; // �洢���ݳ���
        rx_frame_mat.rxIndex = 0;
        state_ = WAITING_FOR_DATA;
        break;
    case WAITING_FOR_DATA:
    /*�洢���յ������ݣ����뵽����������ݳ�Աu8_PK_data�����У�������յ����ݱ������������͵����ݣ�
    ���ں���֮���ٽ���ת������ֱ������������Ĺ���ռ����ԣ�*/
        rx_frame_mat.PK_Data.u8_PK_data[rx_frame_mat.rxIndex++] = byte;
        if (rx_frame_mat.rxIndex >= rx_frame_mat.length)
        {
            state_ = WAITING_FOR_CRC_0;
        }
        break;
    case WAITING_FOR_CRC_0:
        rx_frame_mat.check_code.crc_buff[0] = byte; // �洢 CRC У��ĸ��ֽ�
        state_ = WAITING_FOR_CRC_1;
        break;
    case WAITING_FOR_CRC_1:
        rx_frame_mat.check_code.crc_buff[1] = byte; // �洢 CRC У��ĵ��ֽ�
        state_ = WAITING_FOR_END_0;
        break;
    case WAITING_FOR_END_0:
        if (byte == rx_frame_mat.End_0)
        {
            state_ = WAITING_FOR_END_1;
            rx_frame_mat.End_0 = byte; // �洢֡β
        }
        else
        {
            state_ = WAITING_FOR_HEADER_0;
        }
        break;
    case WAITING_FOR_END_1:
        if (byte == rx_frame_mat.End_1){
        
            rx_frame_mat.End_1 = byte; // �洢֡β
            //�������CRCУ��;ͽ���У��
            if (enableCrcCheck_){
            
                // ���� CRC ������յ��� CRC ���бȽ�
                rx_frame_mat.crc_calculated = CRC16_Table(rx_frame_mat.PK_Data.u8_PK_data, 
                                                          rx_frame_mat.length);
                //�������У��ʧ�ܣ�������֡�����µȴ�֡ͷ
                if (rx_frame_mat.crc_calculated != rx_frame_mat.check_code.crc_code){
                
                    state_ = WAITING_FOR_HEADER_0;
                    break;
                }
            }   //ǰ��Ķ�ͨ��֮��˵������������ɣ��ñ�־λ�����ȴ���һ֡
                state_ = WAITING_FOR_HEADER_0;
                receive_ok_flag = 1;//�������֮���ñ�־λ��ͬʱ��������ʵ�ָ������ݵĺ���������
                //ͬʱ��Ҫǿ�����ǣ�һ��Ҫ�ڸ������ݺ�������󽫸ñ�־λ���¸�ֵΪ0�����������޷��ٴν�������
        }
        else 
        {   //֡β2����������֡�����µȴ���ͷ
            state_ = WAITING_FOR_HEADER_0;
        }
        break;
    default:
        state_ = WAITING_FOR_HEADER_0;
        break;
    }//end_switch
    
}