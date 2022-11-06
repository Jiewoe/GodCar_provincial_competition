#include "esp8266.h"

uint32_t WifiDataLen;                       //wifi传送数据长度
uint8_t  WifiState;                         //wifi整体状态标志位
uint8_t  InitState;                         //连接状态标志位

uint8_t  Testdatalen;                           //串口数据长度

uint8_t DMARecieveBuffer_Test[BUFFERSIZE/8];
uint8_t DMARecieveBuffer_Wifi[BUFFERSIZE];

/*

    DMA空闲中断开启

*/
void WifiDMAInit(void)
{
    HAL_UART_Receive_DMA (&huart8, DMARecieveBuffer_Test, BUFFERSIZE/8);
    __HAL_UART_ENABLE_IT (&huart8, UART_IT_IDLE);

    HAL_UART_Receive_DMA(&huart6, DMARecieveBuffer_Wifi, BUFFERSIZE);
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
}

/*

    设置WiFimode 默认为STA

*/
void setWIFIMode(uint8_t mode)
{
    switch (mode)
    {
        case STA:
            HAL_UART_Transmit(&huart6, "AT+CWMODE=1\r\n",14, 0xffff);
            break;
    }
}

/*

    重启WiFi模块，一般重启后会自动连接已经连接过的wifi

*/
void resetMethed(void)
{
    HAL_UART_Transmit(&huart6, "AT+RST\r\n", 9, 0xffff);
}

/*

    连接wifi函数

*/
void connectJAP(char *ssid, char *password) //连接WiFi，ssid为WiFi名称， pwd为密码
{
    uint8_t id_pwd[50] = {'\0'};
    sprintf (id_pwd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);

    HAL_UART_Transmit(&huart6, id_pwd, strlen(id_pwd), 0xffff);

}

/*

    获取ip地址

*/
void getMethedIP(void)
{
    HAL_UART_Transmit(&huart6, "AT+CIFSR\r\n", 11, 0xffff);
}

/*

    连接主机（手机或电脑端网络调试助手模拟主机）

*/
void connectServer(char *mode, char* ip, uint32_t port)
{
    uint8_t instruction[128] = {'\0'};
    sprintf (instruction, "AT+CIPSTART=\"%s\",\"%s\",%d\r\n", mode, ip, port);

    HAL_UART_Transmit (&huart6, instruction, strlen(instruction), 0xffff);
}

/*

    选择传输模式，默认模式就行

*/
void selectCIPMode(void)
{
    HAL_UART_Transmit (&huart6, "AT+CIPMODE=1\r\n", 15, 0xffff);

}

/*

    开启传输，AT指令失效，理论上主机发送+++到WiFi上会退出，但测试没有效果，建议断电重启退出

*/
void startSend(void)
{
    HAL_UART_Transmit (&huart6, "AT+CIPSEND\r\n", 13, 0xffff);
    WifiState = READY;
}

/*

    完成自动连接WiFi后的步骤

*/
void InitWifiMethod(char *Server_Mode, char *Server_IP, uint32_t port_num)
{

    connectServer (Server_Mode, Server_IP, port_num);
    selectCIPMode ();
    startSend     ();
}

/*

    进入数据传输阶段后处理数据函数

*/
void Dataprocess(void)
{
    
}

/*

    对调试串口发来的指令进行处理

*/
void WifiModeControl(void)
{
    switch (DMARecieveBuffer_Test[0])
    {
   case 0xEE:
       resetMethed();
       break;
   case 0xEF:
       getMethedIP();
       break;
   case 0xFF:
       connectServer("TCP", "10.234.164.107", 8080);
       break;
   case 0xFE:
       selectCIPMode ();
       break;
   case 0xDE:
       startSend ();
       break;
    default:
        HAL_UART_Transmit (&huart6, DMARecieveBuffer_Test, Testdatalen, 0xffff);
        break;
    }
}

/*

    空闲中断回调处理函数 wifi部分

*/
void WifiProcess(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART6)
    {
        if (RESET != __HAL_UART_GET_FLAG(&huart6, UART_FLAG_IDLE))
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart6);
            HAL_UART_DMAStop(&huart6);

            uint32_t temp = __HAL_DMA_GET_COUNTER(&huart6);
            WifiDataLen = BUFFERSIZE - temp;

            if (WifiState == UNREADY)
            {
                HAL_UART_Transmit(&huart6, DMARecieveBuffer_Wifi, WifiDataLen, 0xffff);
            }
            else
            {
                Dataprocess();
            }

            WifiDataLen = 0;
            HAL_UART_Receive_DMA(&huart6, DMARecieveBuffer_Wifi, BUFFERSIZE);
        }
    }
    if (huart->Instance == UART8)
    {
        if (__HAL_UART_GET_FLAG(&huart8, UART_FLAG_IDLE) != RESET)
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart8);
            HAL_UART_DMAStop(&huart8);

            uint32_t temp = __HAL_DMA_GET_COUNTER(&huart8);
            Testdatalen = BUFFERSIZE/8 - temp;

            WifiModeControl ();
            Testdatalen = 0;

            HAL_UART_Receive_DMA(&huart8, DMARecieveBuffer_Test, BUFFERSIZE/8);
        }
    }
}




//信息处理函数，效果不佳，不用
void messageProcess(void)
{
    if(strstr(DMARecieveBuffer_Wifi, "WIFI GOT IP"))
    {
        InitWifiMethod("TCP", "10.234.164.107", 8080);
        // InitState == READY;
    }
}

//清空缓存区函数，暂时没用
void InitBuffer(char *buffer, uint32_t len)
{
    for (uint32_t i=0; i<len; i++)
    {
        *(buffer+i) = '\0';
    }
}
