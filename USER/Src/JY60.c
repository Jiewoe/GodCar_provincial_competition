#include "JY60.h"

uint8_t *DMARecieveBuffer_JY60 = (uint8_t *)0x24040040;

ACCE acce1;
ANGV angv1;
ANG ang1;

//Please define a buffer when you use it
void JY60Init (UART_HandleTypeDef *huartx) //Int
{
    JY60DMAInit();
    uint8_t cmd1[3];
    cmd1[0] = 0xff;
    cmd1[1] = 0xaa;
    cmd1[2] = 0x64;
    HAL_UART_Transmit(huartx, cmd1, 3, 0xffff);//9600
    cmd1[0] = 0xff;
    cmd1[1] = 0xaa;
    cmd1[2] = 0x52;
    HAL_UART_Transmit(huartx, cmd1, 3, 0xffff);//初始化z
    cmd1[0] = 0xff;
    cmd1[1] = 0xaa;
    cmd1[2] = 0x67;
    HAL_UART_Transmit(huartx, cmd1, 3, 0xffff);//初始化加速度计
    cmd1[0] = 0xff;
    cmd1[1] = 0xaa;
    cmd1[2] = 0x61;
    HAL_UART_Transmit(huartx, cmd1, 3, 0xffff);//串口
    cmd1[0] = 0xff;
    cmd1[1] = 0xaa;
    cmd1[2] = 0x65;
    HAL_UART_Transmit(huartx, cmd1, 3, 0xffff);//水平安装
}


//一条数据11位
void JY60_Message_Pross (uint8_t *buffer, ACCE *acce, ANGV *angv, ANG *ang)
{
    while (1)
    {
        while (buffer[0]!= 0x55)
        {
            buffer++;
            if (buffer>DMARecieveBuffer_JY60+38)
            {
                return;
            }
        }
        float g=9.8;
        if (buffer [1] == 0x51)
        {
            acce->Gx = (float)(((short)buffer[3]<<8)|buffer[2])/32768*16*g;
            acce->Gy = (float)(((short)buffer[5]<<8)|buffer[4])/32768*16*g;
            acce->Gz = (float)(((short)buffer[7]<<8)|buffer[6])/32768*16*g;
        }
        if (buffer [1] == 0x52)
        {
            angv->ANGVx = (float)(((short)buffer[3]<<8)|buffer[2])/32768*2000;
            angv->ANGVy = (float)(((short)buffer[5]<<8)|buffer[4])/32768*2000;
            angv->ANGVz = (float)(((short)buffer[7]<<8)|buffer[6])/32768*2000;
        }
        if (buffer [1] == 0x53)
        {
            ang->Pitch = (float)(((short)buffer[3]<<8)|buffer[2])/32768*180;
            ang->Roll  = (float)(((short)buffer[5]<<8)|buffer[4])/32768*180;
            ang->Yaw   = (float)(((short)buffer[7]<<8)|buffer[6])/32768*180;
        }
        buffer = buffer + 11;
    }
}

/*

    DMA初始化

*/
void JY60DMAInit(void)
{
    //__HAL_UART_CLEAR_IDLEFLAG(&huart5);
    HAL_UART_Receive_DMA (&huart5, DMARecieveBuffer_JY60, 38);
 
    //__HAL_UART_ENABLE_IT (&huart5, UART_IT_IDLE);
}

// void JY60Process(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == UART5)
//     {
//         if (RESET != __HAL_UART_GET_FLAG(&huart5, UART_FLAG_IDLE))
//         {
//             __HAL_UART_CLEAR_IDLEFLAG(&huart5);
//             HAL_UART_DMAStop(&huart5);

//             uint8_t temp = ((BDMA_Channel_TypeDef *)(&huart5)->Instance)->CNDTR;
//             if (JY60_MAX_SIZE-temp == 0)
//             {
//                 refineData();
//                 HAL_UART_Transmit(&huart8, Data_JY60, JY60_Data_Size, 0xffff);
//             }

//             HAL_UART_Transmit (&huart8, DMARecieveBuffer_JY60, JY60_MAX_SIZE, 0xffff);

//             HAL_UART_Receive_DMA(&huart5, DMARecieveBuffer_JY60, JY60_MAX_SIZE);
//         }
//     }
// }

// /*

//     提取数据函数，从所有接收到的数据中提取三组有用的;

// */
// void refineData(void)
// {
//     uint8_t cnt=0, i=0, j=0, flag=0;
//     for (i=0; i<JY60_MAX_SIZE; i++)
//     {
//         if (DMARecieveBuffer_JY60[i] == 0x55)
//         {
//             cnt ++;
//             if (cnt > 3)
//                 break;
//         }
//         if(cnt>0)
//         {
//             Data_JY60[j] = DMARecieveBuffer_JY60[i];
//             j ++;
//         }
//     }    
// }