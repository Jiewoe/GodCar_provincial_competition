#ifndef __OV5640TEST_H__
#define __OV5640TEST_H__

#include "dcmi_ov5640.h"
#include "camprocess.h"
#include "sys.h"
#include "usart_dma.h"

extern double test_k;
extern double test_b;

void testInit(void);
void testFunc(void);

#endif