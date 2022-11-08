#ifndef __JY60_H__
#define __JY60_H__

#include "usart.h"

#define JY60_MAX_SIZE 53
#define JY60_Data_Size 33



extern uint8_t *DMARecieveBuffer_JY60;
extern uint8_t *Data_JY60;

void JY60Init (UART_HandleTypeDef *huartx);
// int  JY60_Message_Pross (uint8_t *buffer, ACCE *acce, ANGV *angv, ANG *ang);
void JY60DMAInit(void);
void refineData(void);
void JY60Process(UART_HandleTypeDef *huart);

#endif