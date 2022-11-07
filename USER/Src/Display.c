#include "Display.h"

// dma配置与空闲中断在主函数里面

void DisPlay_Porcess(uint8_t *buffer, uint8_t size)
{
    if (buffer[0]!=0xEE)
    {
        return;
    }
    if ()

}