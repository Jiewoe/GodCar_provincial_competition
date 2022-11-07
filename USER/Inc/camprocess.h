#ifndef __CAMPROCESS_H_
#define __CAMPROCESS_H_

#include "stm32h7xx_hal.h" 
#include <string.h>


//目标先
#define targetlinePer 20


typedef struct 
{
    uint8_t *BufferAddress; //相机原始数据指向的内存地址
    short CamWide;
    short CanHigh;
    uint8_t *Cam_Result; //处理后的图片地址 通常认为紧跟在buffer address之后
} CamStruct;


//更改了CannyEdgeTest 的返回值
short CannyEdgeTest( CamStruct *ov5640, uint8_t lowThr);
float Fsqrt(float x);
uint8_t Atan2(float y, float x);
void Quicksort(float array[], uint8_t maxlen, uint8_t begin, uint8_t end);
void Swap(float *a, float *b);

#endif 