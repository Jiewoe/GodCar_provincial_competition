#ifndef __OPENMV_H__
#define __OPENMV_H__

#include "sys.h"
#include "motor.h"

void openmv_Init(void);
void MV_DataProcess1 (uint8_t* openmv);
void MV_DataProcess2 (uint8_t *openmv);
void MV_DataProcess1_new (uint8_t* openmv);

uint8_t MV_message_check(uint8_t* openmv, UART_HandleTypeDef *huart);
void ShowAssignmentCode(uint8_t *openmv);

extern uint8_t SLine[7];
extern uint8_t FCircle[7];





#endif