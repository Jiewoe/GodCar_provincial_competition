#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "stm32h7xx_hal.h"
#include "servoDriver.h"

#define Display_Buffer 0x24040000

extern UART_HandleTypeDef huart4;

void DisPlay_Porcess(uint8_t *buffer);
void DisPlay_Init();


#endif