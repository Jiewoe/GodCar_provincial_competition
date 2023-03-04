#include "openmv.h"

uint8_t SLine[7] = {0x7B, 0x02, 0, 0, 0, 0, 0x7C};
uint8_t FCircle[7] = {0x7B, 0x01, 0, 0, 0, 0, 0x7C};

void openmv_Init(void)
{
    // __HAL_UART_ENABLE_IT(&huart7, UART_IT_IDLE);
    HAL_UART_Receive_IT(&huart2, openmv1, 7);

    // __HAL_UART_ENABLE_IT(&huart8, UART_IT_IDLE);
    HAL_UART_Receive_IT(&huart3, openmv2, 7);
    

    // HAL_UART_Receive_IT (&huart1, openmv1, 7);
}


void MV_DataProcess1 (uint8_t* openmv)
{  
    if (MV_message_check(openmv, &huart2) == 1) return; //检验数据是否正确

    if (openmv[1] == 0x01)                              //圆心偏移矫正
    {
        MOVE_MV_micro (openmv[2], openmv[3], openmv[4], openmv[5]);
        HAL_UART_Transmit (&huart1, openmv, 7, 0xff);
    }
    else if (openmv[1] == 0x02)
    {
        Lateral_correction (openmv[2], openmv[3], openmv[4], openmv[5]);      //巡线矫正
    }

    HAL_UART_Receive_IT (&huart2, openmv1, 7);
}

void MV_DataProcess1_new (uint8_t* openmv)
{  
    if (MV_message_check(openmv, &huart2) == 1) return; //检验数据是否正确

    if (openmv[1] == 0x01)                              //圆心偏移矫正
    {
        DataCheck (openmv);
        if (hengPian_flag==1 && zongPian_flag==1)
        {
            MOVE_MV_micro (openmv[2], circleData.hengPian[1], openmv[4], circleData.zongPian[1]);
        }
        else if (hengPian_flag==1 && zongPian_flag==0)
        {
            MOVE_MV_micro (openmv[2], circleData.hengPian[1], openmv[4], 0);
        }
        else if (hengPian_flag==0 && zongPian_flag==1)
        {
            MOVE_MV_micro (openmv[2], 0, openmv[4], circleData.zongPian[1]);
        }

        HAL_UART_Transmit (&huart1, openmv, 7, 0xff);
    }
    else if (openmv[1] == 0x02)
    {
        Lateral_correction (openmv[2], openmv[3], openmv[4], openmv[5]);      //巡线矫正
    }

    HAL_UART_Receive_IT (&huart2, openmv1, 7);
}


void MV_DataProcess2 (uint8_t *openmv)
{
    if (MV_message_check(openmv, &huart3) == 1) return;
    if (openmv[1] == 0x03)
    {
        ShowAssignmentCode (openmv);
    }
    
    HAL_UART_Receive_IT (&huart3, openmv2, 7);
}

/*
    数组清理函数

*/
void ClearBuffer (uint8_t *buffer)
{
    for (int i=0; i<7; i++)
    {
        *(buffer+i) = 0x00;
    }
}


/*
    信息检查函数
*/
uint8_t MV_message_check(uint8_t* openmv, UART_HandleTypeDef *huart)
{
    if (openmv[0] == 0x7B && openmv[6] == 0x7C) //帧头、帧尾符合 返回0
    {
        
        return 0;
    }
    else                                        //帧头、帧尾不符合 返回1
    {
        //这里要重新写
        // uint8_t error[7] =  {0x7B, 0x01, 0x01, 0x01, 0x01, 0x01 ,0x7C};
        // uint8_t resend[7] = {0x7B, 0x02, 0x02, 0x02, 0x02, 0x02, 0x7C};

        // for (int i=0; i<3; i++)
        // {
        //     HAL_UART_Transmit (huart, error, 7, 0x00ff);    //发送三遍错误信息（说是openmv一遍收不到），让其停止发送数据
        // }

        ClearBuffer (openmv);

        // MX_USART1_UART_Init();
        MX_USART2_UART_Init();
        MX_USART3_UART_Init();

        HAL_UART_Receive_IT (huart, openmv, 7);             //重开接收中断

        // HAL_UART_Transmit (huart, resend , 7, 0x00ff);      //发送重发信息
        
        //在这里写出问题后之后的处理
        return 1;
    }
}