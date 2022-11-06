#ifndef __ESP8266_H
#define __ESP8266_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "usart.h"

#define STA 1
#define DEFAULT 1
#define UNREADY 0
#define READY 1
#define BUFFERSIZE 1024
#define WIFIbuffer 0x24000000U

void WifiDMAInit(void);

void setWIFIMode(uint8_t mode);
void resetMethed(void);
void connectJAP(char *ssid, char *password);
void getMethedIP(void);
void connectServer(char *mode, char* ip, uint32_t port);
void selectCIPMode(void);
void startSend(void);
void messageProcess(void);
void InitBuffer(char *buffer, uint32_t len);

void InitWifiMethodTest (void);
void InitWifiMethod(char *Server_Mode, char *Server_IP, uint32_t port_num);
void WifiModeControl(void);
void WifiProcess(UART_HandleTypeDef *huart);

extern uint32_t WifiDataLen;
extern uint8_t  WifiState;
extern uint8_t  InitState;
extern uint8_t  Testdatalen;

extern uint8_t DMARecieveBuffer_Test[];     // DMA 串口数据缓冲区
extern uint8_t DMARecieveBuffer_Wifi[];     // DMA wifi数据接收缓冲

#endif