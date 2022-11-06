
#include "sys.h"


uint8_t OpenMV1[7];
uint8_t OpenMV2[7];

//在这里设置每一步做什么
void Procedure_Setting(uint8_t now)
{
    switch (now)
    {
        //第一步：移动到二维码前面
        case 1:
            target = 10; //到板子距离
            IF_MOVE = 1;
            //命令接受二维码 
            //这里要封装机械臂初始化
            uint8_t a[3] = {0x55, 0x30, 0x66};
            HAL_UART_Transmit (&huart3, a, 3, 0xfff);
            HAL_UART_Receive_IT (&huart3,OpenMV1,7);
            break;
        case 2:
            target = 80; //到原料区距离
            //慢慢移动
            HAL_UART_Receive_IT (&huart2, OpenMV2, 7);//接收回调没写//停车指令
            break;
        case 3:
            //停车重复接收机械臂指令
            //等待完成指令的发送
            break;
        case 4:
            target = 20; //到拐角
            //用陀螺仪定位
            //等待完成
            break;
        case 5:
            break;
            
        
            

    }
}
