
#include "sys.h"


// uint8_t OpenMV1[7];
// uint8_t OpenMV2[7];

//在这里设置每一步做什么
void Procedure_Setting(uint8_t now)
{
    static uint8_t setted = 0;
    //和之前的一样就不执行
    if (now==setted) return;
    setted = now;
    switch (now)
    {
        //第一步：移动到二维码前面
        case 1:
            HAL_Delay (500);
            target = 80; //到板子距离
            IF_MOVE = 1;
            //命令接受二维码 
            //这里要封装机械臂初始化
            // uint8_t a[3] = {0x55, 0x30, 0x66};
            // HAL_UART_Transmit (&huart3, a, 3, 0xfff);
            // HAL_UART_Receive_IT (&huart3,OpenMV1,7);
            break;
        case 2:
            HAL_Delay (500);
            target = 100; //到原料区距离
            IF_MOVE = 1;
            //慢慢移动
            // HAL_UART_Receive_IT (&huart2, OpenMV2, 7);//接收回调没写//停车指令
            break;
        case 3:
            HAL_Delay (500);
            //停车重复接收机械臂指令
            //等待完成指令的发送
            procedure++;
            break;
        case 4:
            HAL_Delay (500);
            target = 20; //到拐角
            IF_MOVE = 1;
            //用陀螺仪定位
            //等待完成
            break;
        case 5://到达拐角
            HAL_Delay (500);
            Move_left();
            break;
        case 6: //到达中点
            HAL_Delay (500);
            target = 97;
            IF_MOVE = 1;
            break;  
        case 7:
        //机械臂动作
            HAL_Delay (500);
            procedure++;
            break;
        case 8: //到达拐角
            HAL_Delay (500);
            target = 97;
            IF_MOVE = 1;
            break;
        case 9: 
            HAL_Delay (500);
            Move_left ();
            break;
        case 10:
            HAL_Delay (500);
            target = 97;
            IF_MOVE = 1;
            break;
            
        
            

    }
}
