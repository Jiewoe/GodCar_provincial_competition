#include "Display.h"

// dma配置与空闲中断在主函数里面


uint8_t *Display_Buffer = 0x24000000+0x40000;

void DisPlay_Init()
{
    __HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE); //使能IDLE中断
   HAL_UART_Receive_DMA(&huart4,Display_Buffer,64);//开启串口DMA接收

}

void DisPlay_Porcess(uint8_t *buffer)
{
    if (buffer[0]!=0xEE)
    {
        return;
    }
    if (buffer[1]==0xB1&&buffer[2]==0x11)
    {
        if (buffer[4]==0x00)
        {
            if (buffer[6]==0x01)
            {
                HolderControl((uint16_t)(buffer[10]*256+buffer[11]));
            }
        }
    }

}