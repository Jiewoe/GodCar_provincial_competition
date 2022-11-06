/***
	************************************************************************************************
	*	@file  	usart_dma.c
	*	@version V1.0
	*  @date    2022-4-6
	*	@author  反客科技
	*	@brief   usart DMA 相关函数
   ************************************************************************************************
   *  @description
	*
	*	实验平台：反客STM32H750VBT6核心板 （型号：FK750M1-VBT6）
	*	淘宝地址：https://shop212360197.taobao.com
	*	QQ交流群：536665479
	*
>>>>> 文件说明：
	*
	*  1. 初始化usart引脚，配置波特率等参数
	*	2. 配置 DMA 模式，并开启对应的中断
	*
	************************************************************************************************
***/


#include "usart_dma.h"

extern UART_HandleTypeDef huart1;  // UART_HandleTypeDef 结构体变量

extern DMA_HandleTypeDef   DMA_Handle_Uart1_TX;  // DMA句柄
extern DMA_HandleTypeDef   DMA_Handle_Uart1_TX;  // DMA句柄

volatile	uint8_t USART_FPS = 0;			// 串口发送速度（每秒发送照片的帧数）
volatile uint8_t USART1_TX_State = 1;  // DCMI状态标志，当数据帧传输完成时，会被中断回调函数置 1     

//这个中段函数要改

/***************************************************************************************************************************************
*	函 数 名: HAL_UART_TxCpltCallback
*
*	函数功能: 发出完毕回调函数，每当发送数据完毕，会进入该中断服务函数
*
*****************************************************************************************************************************************/

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
// 	static uint32_t 	USART_Tick = 0;         // 用于保存当前的时间计数值
//    static uint8_t 	USART_Frame_Count = 0;   // 帧数计数   

//  	if(HAL_GetTick() - USART_Tick >= 1000)    // 每隔 1s 计算一次帧率
// 	{
// 		USART_Tick = HAL_GetTick();       // 重新获取当前时间计数值

// 		USART_FPS = USART_Frame_Count;		// 串口发送速度（每秒发送照片的帧数）

// 		USART_Frame_Count = 0;            // 计数清0
// 	}
// 	USART_Frame_Count ++;    // 每进入一次中断（每次传输完一帧数据），计数值+1

// 	USART1_TX_State = 1;
}


/*************************************************************************************************
*	在有些场合，例如LVGL因为需要用__aeabi_assert或者TouchGFX，不能勾选 microLib 以使用printf
*	添加以下代码，让标准C库支持重定向fput
*  根据编译器，选择对应的代码即可
*************************************************************************************************/


//// AC5编译器使用这段代码
//#pragma import(__use_no_semihosting)  
//int _ttywrch(int ch)    
//{
//    ch=ch;
//	return ch;
//}         
//struct __FILE 
//{ 
//	int handle; 

//}; 
//FILE __stdout;       

//void _sys_exit(int x) 
//{ 
//	x = x; 
//} 



//// AC6编译器使用这段代码
//__asm (".global __use_no_semihosting\n\t");
//void _sys_exit(int x) 
//{
//  x = x;
//}
////__use_no_semihosting was requested, but _ttywrch was 
//void _ttywrch(int ch)
//{
//    ch = ch;
//}

//FILE __stdout;



/*************************************************************************************************
*	函 数 名:	fputc
*	入口参数:	ch - 要输出的字符 ，  f - 文件指针（这里用不到）
*	返 回 值:	正常时返回字符，出错时返回 EOF（-1）
*	函数功能:	重定向 fputc 函数，目的是使用 printf 函数
*	说    明:	无		
*************************************************************************************************/

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100);	// 发送单字节数据
	return (ch);
}

