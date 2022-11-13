#ifndef __JY60_H__
#define __JY60_H__

#include "usart.h"

extern uint8_t *DMARecieveBuffer_JY60;

typedef struct
{
    float Gx;
    float Gy;
    float Gz;
} ACCE;

typedef struct
{
    float ANGVx;
    float ANGVy;
    float ANGVz;
} ANGV;

typedef struct
{
    float Pitch;
    float Roll;
    float Yaw;
} ANG;

extern  ACCE acce1;
extern  ANGV angv1;
extern  ANG ang1;

void JY60Init (UART_HandleTypeDef *huartx);
void  JY60_Message_Pross (uint8_t *buffer, ACCE *acce, ANGV *angv, ANG *ang);
void JY60DMAInit(void);

#endif