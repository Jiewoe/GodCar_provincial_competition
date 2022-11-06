#ifndef __USART_H
#define __USART_H

#include "stm32h7xx_hal.h"
#include "stdio.h"


extern volatile uint8_t USART_FPS;				// 串口发送速度（每秒发送照片的帧数）
extern volatile uint8_t USART1_TX_State ; 	// DCMI状态标志，当数据帧传输完成时，会被中断回调函数置 1   



#endif //__USART_H





