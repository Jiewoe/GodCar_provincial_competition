#ifndef __SYS_H__
#define __SYS_H__

#include "stm32h7xx_hal.h"

//openmv1 下面的 usart 2
//opemmv2 前面的 usart 3


//声明变量
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim14;

extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart8;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_tx;

extern uint8_t target;

//内存控制
/*
    Camera_Buffer 0x24000000
    Usart_Buffer 0x24000000 + Display_Width*Display_Height
    Display_Buffer = 0x24000000+0x40000; 宽度64
    uint8_t *DMARecieveBuffer_JY60 = 0x24040040; 宽度11

    

*/


#define Camera_Buffer 0x24000000 // 原始数据dma位置
#define Usart_Buffer 0x24000000 + Display_Width*Display_Height //处理过程中数据位置

extern uint8_t OpenMV1[7];
extern uint8_t OpenMV2[7];


//状态控制
extern uint8_t procedure;
extern uint8_t IF_MOVE;


//定时器改名字
//轮子pwm

//一共6000
#define Motor1_Speed (TIM1->CCR1)
#define Motor2_Speed (TIM1->CCR2)
#define Motor3_Speed (TIM1->CCR3)
#define Motor4_Speed (TIM1->CCR4)

#define Motor_GPIO GPIOG





#define Motor1_CNT ((&htim2)->Instance->CNT)
#define Motor2_CNT ((&htim3)->Instance->CNT)
#define Motor3_CNT ((&htim4)->Instance->CNT)
#define Motor4_CNT ((&htim8)->Instance->CNT)





#endif