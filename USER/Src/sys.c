
#include "sys.h"
#include "servoDriver.h"
#include "openmv.h"

uint8_t Assignment[6];
uint8_t IF_LINE = 0;   // 在巡线矫正完成后置1，在发送停止巡线信号后置0
uint8_t IF_CIRCLE = 0; // 是否进行找圆  置1时找圆，置0时停止找圆
uint8_t IF_FINISHLINE = 0;

uint8_t leftDistance = 5;       // 从启停区左移距离
uint8_t codeDistance = 60;      // 到二维码板子距离
uint8_t materialDistance = 110; // 到原料区距离

/*
    每次执行完前进或者转弯指令后，procedure++（在前进和转弯函数内）

*/
void Procedure_Setting(uint8_t now)
{
    static uint8_t setted = 0;
    // 和之前的一样就不执行
    if (now == setted)
        return;
    setted = now;
    showCase();
    switch (now)
    {
    // //移出启停区域
    // case 0:
    //     IF_OUT = 1;
    //     target = 20;
    //     //修改中
    //     break;

    // 第二步：移动到二维码前面
    case 1:
        HAL_Delay(2500);
        ActionFunc(lineAngle); // 调整到巡线位置

        // HAL_Delay (7000);
        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 发消息给openmv，开启巡线

        HAL_Delay(1200);

        IF_LINE = 0;

        target = codeDistance;
        IF_MOVE = 1;

        HAL_Delay(500);

        break;

    // 扫码
    case 2:
        HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff); // 停止巡线
        Move_Stop();
        HAL_Delay(600);

        // ActionFunc (scanCodeAngle);   //扫码角度，未设置

        // 提示2号openmv开始扫码
        HAL_Delay(600);
        PawControl (180);
        HAL_UART_Transmit (&huart3, SCode, 7, 0x00ff);
        
        // 测试代码
        // 实际：显示任务码后自动procedure++

        break;

    // 移动到圆盘前20cm
    case 3:
        HAL_UART_Transmit(&huart3, Stop, 7, 0x00ff);
        HAL_Delay(1000);
        ActionFunc (specialLineAngle);
        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线

        // HAL_Delay (1200);
        target = materialDistance - 20;
        IF_MOVE = 1;

        break;

    // 停止找线，向前移动20cm
    case 4:
        HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff);
        Move_Stop();
        target = 20;
        IF_MOVE = 1;
        break;

    // 寻找原料区域
    case 5:
        ActionFunc(stageangle);
        HAL_UART_Transmit(&huart2, Smaterial, 7, 0xff);
        IF_CIRCLE = 1;
        break;
    //停好车
    case 6:
        HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff);
        Move_Stop();

        break;
    case 7:

        // 原料区装货

    case 8:
        // //HAL_Delay(2000); // 用找线的方式控制车的横向位置，这里时间长一点
        //
        // // MV_StopSearchLine ();       //停止巡线
        // // openmv2操作

        // HAL_UART_Transmit(&huart2, Smaterial, 7, 0xff);
        // IF_CIRCLE = 1; // 执行定位原料区

        // // ActionFunc (lineAngle);
        // // HAL_Delay (800);
        // // HAL_UART_Transmit (&huart2, SLine, 7, 0x00ff);  //开启巡线
        // // HAL_Delay (1200);

        // // 在movemrico里给了进行下一步的指令
        // // procedure++;
        break;

    // 移动到第一个拐角
    case 9:
        // 停止找原料区
        HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff);
        ActionFunc(lineAngle);

        HAL_Delay(500);
        target = 36; // 到拐角（这里距离太短，不开循迹）

        IF_LINE = 1; // 这里手动改变标志位
        IF_MOVE = 1;

        // 用陀螺仪定位
        // 等待完成

        break;

    // 转弯
    case 10:
        IF_LINE = 0; // 手动关闭标志位

        Move_Turnleft(); // 左转
        HAL_Delay(1200);

        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线

        break;

    // 移动到粗加工区
    case 11:
        HAL_Delay(1200);
        target = 100; // 拐角到粗加工区的距离
        IF_MOVE = 1;
        break;

    // 粗加工区，调整位置
    case 12:
        // 机械臂动作
        MV_StopSearchLine(); // 停止巡线
        IF_FINISHLINE = 1;

        IF_CIRCLE = 1;

        ActionFunc(circleAngle); // 机械臂调整到找圆位置
        HAL_Delay(800);
        HAL_UART_Transmit(&huart2, FCircle, 7, 0x00ff); // 提示openmv找圆，不知道找到的是啥颜色的圆

        break;

    // 放置物料
    case 13:
        HAL_UART_Transmit(&huart2, Stop, 7, 0xff);
        // DisposeCargo_Logic ();

        // HAL_Delay (2000);

        // PickCargo_Logic ();

        HAL_Delay(1000);

        ActionFunc(lineAngle);

        procedure++;
        break;

    // 移动到第二个拐角
    case 14:
        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线
        HAL_Delay(1200);
        target = 103;
        IF_MOVE = 1;
        break;

    // 转弯
    case 15:
        HAL_Delay(500);

        MV_StopSearchLine(); // 停止巡线

        HAL_Delay(600);
        Move_Turnleft(); // 左转
        break;

    // 到达精加工区
    case 16:
        HAL_Delay(500);

        ActionFunc(lineAngle);

        HAL_Delay(1000);

        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线
        HAL_Delay(1200);

        target = 97; // 到达精加工区
        IF_MOVE = 1;

        break;

    // 精加工区,调整位置
    case 17:
        HAL_Delay(600);
        MV_StopSearchLine(); // 停止巡线
        IF_FINISHLINE = 1;
        IF_CIRCLE = 1;
        ActionFunc(circleAngle);

        HAL_Delay(800);

        HAL_UART_Transmit(&huart2, FCircle, 7, 0x00ff); // 开始找圆

        break;

    // 精加工区，放置物品
    case 18:
        HAL_Delay(1000);
        HAL_Delay(500);
        // DisposeCargo_Logic ();  //放置物块
        HAL_UART_Transmit(&huart2, Stop, 7, 0xff);
        HAL_Delay(1000);

        ActionFunc(lineAngle);

        HAL_Delay(600);
        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线
        HAL_Delay(1500);

        procedure++;
        break;

    // 到第三个拐角
    case 19:
        HAL_Delay(500);
        target = 100;
        IF_MOVE = 1;
        break;

    // 转弯
    case 20:
        HAL_Delay(500);
        MV_StopSearchLine(); // 停止巡线

        HAL_Delay(600);
        Move_Turnleft(); // 左转
        break;

    // 回到出发点
    case 21:
        HAL_Delay(2000);

        ActionFunc(lineAngle);

        HAL_Delay(600);

        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开始巡线
        HAL_Delay(1200);

        target = 200;
        IF_MOVE = 1;
        IF_FINISHLINE = 1;

        Move_Stop();

        break;
    }
}

int filter(int a, int YuZhi)
{
    static int data1, data2;
    static char flag = 0;
    if (flag < 2)
    {
        if (flag == 0)
            data1 = a;
        if (flag == 1)
            data2 = a;
        return a;
    }
    flag++;
    if (flag > 30)
        flag = 0;
    int agv = (data2 + data1) / 2;
    if (fabs(agv - a) > YuZhi)
    {
        if (fabs(a - data1) > fabs(a - data2))
        {
            data1 = data2;
        }
        else
        {
            data2 = data1;
        }
        return data1;
    }
    else
    {
        if (fabs(a - data1) > fabs(a - data2))
        {
            data1 = a;
        }
        else
        {
            data2 = a;
        }
        return a;
    }
}
