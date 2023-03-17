#ifndef __OPENMV_H__
#define __OPENMV_H__

#include "sys.h"
#include "motor.h"
#include "servoDriver.h"

void openmv_Init(void);
void MV_DataProcess1 (uint8_t* openmv);
void MV_DataProcess2 (uint8_t *openmv);
void MV_DataProcess1_new (uint8_t* openmv);

uint8_t MV_message_check(uint8_t* openmv, UART_HandleTypeDef *huart);
void ShowAssignmentCode(uint8_t *openmv);
void MV_filter_Process(uint8_t* openmv, uint8_t yuzhi);
void ClearBuffer (uint8_t *buffer);
void MV_StopSearchLine (void);
void MV_Tellcolor(uint8_t *openmv);


extern const uint8_t SLine[7];
extern const uint8_t FCircle[7];
extern const uint8_t Stop[7];
extern const uint8_t SCode[7];
extern const uint8_t Smaterial[7];
extern uint8_t Swuliao[7];
extern uint8_t Pick_ins;
extern const uint8_t Flocation[7];
extern const uint8_t Tellcolor[7];





#endif