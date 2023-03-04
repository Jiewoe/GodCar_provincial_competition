#ifndef __SYS_H__
#define __SYS_H__

#include "stm32h7xx_hal.h"

//openmv1 下面的 usart 2
//opemmv2 前面的 usart 3
void Procedure_Setting(uint8_t now);

extern uint8_t round1;
extern uint8_t round2;
extern uint8_t round3;
extern uint8_t round4;

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
extern uint8_t left_target;
extern uint8_t right_target;

typedef struct 
{
    uint8_t length;
    uint8_t hengPian[3];
    uint8_t zongPian[3];
} Queue;

extern uint8_t DataCheck_fla; //数据是否进行检验标志位，如果还在收集数据，则为0，收集完为1
extern uint8_t hengPian_flag;  //横向偏差是否有效标志位，决定本次数据是否进行修正，0为无效，1有效（进行修正）
extern uint8_t zongPian_flag;  //纵向偏差是否有效标志位
extern Queue circleData;

//内存控制
/*
    Camera_Buffer 0x24000000
    Usart_Buffer 0x24000000 + Display_Width*Display_Height
    Display_Buffer = 0x24000000+0x40000; 宽度64
    uint8_t *DMARecieveBuffer_JY60 = 0x24040040; 宽度11
    openmv1 0x24040080
    openmv2 0x240400C0

    

*/


#define Camera_Buffer 0x24000000 // 原始数据dma位置
#define Usart_Buffer 0x24000000 + Display_Width*Display_Height //处理过程中数据位置
#define openmv1 0x24000080
#define openmv2 0x240000C0

// extern uint8_t OpenMV1[7];
// extern uint8_t OpenMV2[7];


//状态控制
extern uint8_t procedure;
extern uint8_t IF_MOVE;
extern uint8_t IF_LINE;
extern uint8_t IF_CIRCLE;
extern uint8_t Assignment[6];


//定时器改名字
//轮子pwm

/*
    speed 0~6000

*/
#define Motor1_Speed (TIM1->CCR1)
#define Motor2_Speed (TIM1->CCR2)
#define Motor3_Speed (TIM1->CCR3)
#define Motor4_Speed (TIM1->CCR4)

#define Motor_GPIO GPIOG
#define Motor1_Pin GPIO_PIN_2
#define Motor2_Pin GPIO_PIN_3
#define Motor3_Pin GPIO_PIN_4
#define Motor4_Pin GPIO_PIN_5




#define Motor1_CNT ((&htim2)->Instance->CNT)
#define Motor2_CNT ((&htim3)->Instance->CNT)
#define Motor3_CNT ((&htim4)->Instance->CNT)
#define Motor4_CNT ((&htim8)->Instance->CNT)





#endif