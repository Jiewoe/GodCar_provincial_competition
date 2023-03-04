#ifndef __MOTOR__H__
#define __MOTOR__H__


#include "sys.h"


int max (int a, int b);
void Move_Forward ();
void Move_Left();
void Move_Turnleft();
void Lateral_correction (uint8_t sign, uint8_t piancha, uint8_t sign2, uint8_t hengPia);
void printCnt(void);
void MOVE_MV_micro(uint8_t symheng, uint8_t heng, uint8_t symzong, uint8_t zong);
void Motor_Init();
void DataCheck(uint8_t *openmv);
void comparePiancha(uint8_t *piancha);
void insertSort(uint8_t *arr);

#endif