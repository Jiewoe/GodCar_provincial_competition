
#include "sys.h"
#include "servoDriver.h"
#include "openmv.h"

uint8_t Assignment[6] = {};
uint8_t IF_LINE = 0;
uint8_t IF_CIRCLE = 0;

uint8_t leftDistance = 5;       //从启停区左移距离
uint8_t codeDistance = 80;      //到二维码板子距离
uint8_t materialDistance = 100; //到原料区距离

//在这里设置每一步做什么
void Procedure_Setting(uint8_t now)
{
    static uint8_t setted = -1;
    //和之前的一样就不执行
    if (now==setted) return;
    setted = now;
    switch (now)
    {
        case 0: //移出启停区域
            HAL_Delay (500);
            left_target = leftDistance;
            IF_MOVE = 2;    //左移标识

            break;

        //第二步：移动到二维码前面
        case 1:
            HAL_Delay (500);
            ActionFunc (lineAngle); //调整到巡线位置
            HAL_UART_Transmit (&huart2, SLine, 7, 0x00ff);
            
            target = codeDistance;

            HAL_Delay (500);
            IF_MOVE = 1;
            
            break;
        
        //移动到原料区
        case 2:
            HAL_Delay (500);
            target = materialDistance; 
            IF_MOVE = 1;

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
            break;
        case 10:
            HAL_Delay (500);
            target = 97;
            IF_MOVE = 1;
            break;
            
        
            

    }
}
