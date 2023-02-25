#ifndef __MOTOR__H__
#define __MOTOR__H__


#include "sys.h"



int max (int a, int b);
void Move_Forward ();
void Move_left();
void Lateral_correction (short piancha);

void Motor_Init();

#endif