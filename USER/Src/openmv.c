#include "openmv.h"

const uint8_t SLine[7] = {0x7B, 0x02, 0, 0, 0, 0, 0x7C};
const uint8_t FCircle[7] = {0x7B, 0x01, 0, 0, 0, 0, 0x7C};
const uint8_t SCode[7] = {0x7B, 0x03, 0, 0, 0, 0, 0x7C};
const uint8_t Stop[7] = {0x7B, 0x07, 0, 0, 0, 0, 0x7C};
const uint8_t Smaterial[7] = {0x7B, 0x09, 0, 0, 0, 0, 0x7C};

void openmv_Init(void)
{
    ClearBuffer(openmv1);
    ClearBuffer(openmv2);

    // __HAL_UART_ENABLE_IT(&huart7, UART_IT_IDLE);
    HAL_UART_Receive_IT(&huart2, openmv1, 7);

    // __HAL_UART_ENABLE_IT(&huart8, UART_IT_IDLE);
    HAL_UART_Receive_IT(&huart3, openmv2, 7);
    
    //重置openmv
    
    HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, RESET);
    HAL_GPIO_WritePin (GPIOD, GPIO_PIN_12, RESET);

    HAL_Delay (1000);

    HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, SET);
    HAL_GPIO_WritePin (GPIOD, GPIO_PIN_12, SET);

    // HAL_UART_Receive_IT (&huart1, openmv1, 7);
}


void MV_DataProcess1 (uint8_t* openmv)
{  
    if (MV_message_check(openmv, &huart2) == 1) return; //检验数据是否正确

    if (openmv[1] == 0x01 || openmv[1] == 0x05)                              //圆心偏移矫正
    {
        MV_filter_Process(openmv, 15);                                      //滤波
        MOVE_MV_micro (openmv[2], openmv[3], openmv[4], openmv[5]);         //矫正程序
        HAL_UART_Transmit (&huart1, openmv, 7, 0xff);
    }
    else if (openmv[1] == 0x06) //未找到圆，返回错误信息
    {
        Move_Stop();
        HAL_UART_Transmit (&huart1, openmv, 7, 0xff);
    }

    else if (openmv[1] == 0x02) //开始巡线
    {
        HAL_UART_Transmit (&huart1, openmv, 7, 0xff);
        MV_filter_Process (openmv , 8);                                       //滤波
        Lateral_correction (openmv[2], openmv[3], openmv[4], openmv[5]);      //巡线矫正
    }

    else if (openmv[1] == 0x09) //找原料区
    {
        MV_filter_Process(openmv, 15);                                      //滤波
        MOVE_MV_micro (openmv[2], openmv[3], 0, 0);         //矫正程序
        HAL_UART_Transmit (&huart1, openmv, 7, 0xff);
    }

    HAL_UART_Receive_IT (&huart2, openmv1, 7);
}


void MV_DataProcess2 (uint8_t *openmv)
{
    if (MV_message_check(openmv, &huart3) == 1) return;
    if (openmv[1] == 0x03)
    {

        ShowAssignmentCode (openmv);        //显示任务码
        procedure ++;
    }
    else if (openmv[1] == 0x04)             //发送抓取信息
    {

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

/*
    数据过滤后的处理函数，未测试
*/
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



//滤波处理
void MV_filter_Process(uint8_t* openmv, uint8_t yuzhi)
{
    openmv[3] = (uint8_t) filter (openmv[3], yuzhi);
    openmv[5] = (uint8_t) filter (openmv[5], yuzhi);
}

void MV_StopSearchLine (void)
{
    HAL_UART_Transmit (&huart2, Stop, 7, 0x00ff);
    Move_Stop ();
    IF_LINE = 0;
}
