#ifndef __MOTOR__H__
#define __MOTOR__H__


#include "sys.h"



int max (int a, int b);
void Move_Forward ();
void Move_left();
void Lateral_correction (uint8_t piancha, uint8_t sign);
void printCnt(void);

void Motor_Init();

#endif