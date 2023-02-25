#include "Display.h"

// dma配置与空闲中断在主函数里面


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
    if (buffer[1]==0xB1&&buffer[2]==0x11)//滑块指令与按钮指令
    {
        if (buffer[4]==0x00)//0号屏幕，大臂舵机和云台舵机（0x01）控制
        {
            if (buffer[6]==0x01)//1号滑块
            {
                HolderControl((uint16_t)(buffer[10]*256+buffer[11]));
            }
            if (buffer[6] == 0x02)
            {
                LeftArmControl((uint16_t)(buffer[10]*256+buffer[11]));
            }
            if (buffer[6] == 0x03)
            {
                RightArmControl((uint16_t)(buffer[10]*256+buffer[11]));
            }

        }

        // 2号屏幕 货架控制
        if (buffer[4]==0x01)
        {
            uint8_t set = 1;
            if (buffer[6]>3)
            {
                set = 0;
                buffer[6]=buffer[6]-3;
            }
            CargoSet(buffer[6], set);
        }

        // 3号屏幕 WiFi按钮
        if (buffer[4] == 0x02)
        {

        }
    }

}

