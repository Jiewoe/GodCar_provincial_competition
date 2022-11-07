#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "stm32h7xx_hal.h"
#include "servoDriver.h"

extern uint8_t Display_Buffer[64];
extern UART_HandleTypeDef huart4;

void DisPlay_Porcess(uint8_t *buffer);
void DisPlay_Init();


#endif