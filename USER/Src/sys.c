#include "sys.h"
#include "servoDriver.h"
#include "openmv.h"

uint8_t Assignment[6];

/*
    用来记录圆环的颜色，然后控制车少少计算
    用空间换时间
*/
uint8_t boardcolor[6];
uint8_t colorflag = 0;

volatile uint8_t IF_LINE = 0;   // 在巡线矫正完成后置1，在发送停止巡线信号后置0
volatile uint8_t IF_CIRCLE = 0; // 是否进行找圆  置1时找圆，置0时停止找圆
volatile uint8_t CLEARFLAG = 0;

uint8_t leftDistance = 5;      // 从启停区左移距离
uint8_t codeDistance = 60;     // 到二维码板子距离
uint8_t materialDistance = 90; // 到原料区距离

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
    // showCase();
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
        HAL_Delay(1000);
        Move_Out();
        HAL_Delay(200);
        CLEARFLAG = 1;
        target = 30;
        IF_MOVE = 1;
        IF_LINE = 1;
        break;

    case 2:
        HAL_Delay(1000);
        ActionFunc(lineAngle); // 调整到巡线位置
        IF_MOVE = 0;

        // HAL_Delay (7000);
        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 发消息给openmv，开启巡线

        HAL_Delay(1000);

        target = codeDistance;
        CLEARFLAG = 1;
        IF_MOVE = 1;

        break;

    // 扫码
    case 3:
        HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff);
        IF_LINE = 0; // 停止巡线
        HAL_Delay(300);
        Move_Stop();

        // ActionFunc (scanCodeAngle);   //扫码角度，未设置

        // 提示2号openmv开始扫码
        PawControl(180);
        HAL_UART_Transmit(&huart3, SCode, 7, 0x00ff);

        // 测试代码
        // 实际：显示任务码后自动procedure++

        break;

    // 移动到圆盘前20cm
    case 4:
        HAL_UART_Transmit(&huart3, Stop, 7, 0x00ff);
        // ActionFunc (specialLineAngle);
        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线

        HAL_Delay(600);
        CLEARFLAG = 1;
        target = materialDistance - 25;
        IF_MOVE = 1;

        break;

    // 停止找线，向前移动20cm
    case 5:
        ActionFunc(specialLineAngle);
        HAL_Delay(2000);
        HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff);
        IF_LINE = 1;
        HAL_Delay(300);
        CLEARFLAG = 1;
        // IF_LINE = 1;
        target = 20;
        IF_MOVE = 1;
        break;

    // 寻找原料区域
    case 6:
        Move_Stop();
        // IF_LINE = 0;
        ActionFunc(stageangle);
        HAL_UART_Transmit(&huart2, Smaterial, 7, 0xff);
        IF_CIRCLE = 1;
        break;
    // 停好车
    case 7:
        HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff);
        IF_LINE = 0;
        HAL_Delay(300);
        Move_Stop();
        ActionFunc(waitAngle);
        HAL_Delay(600);
        Swuliao[2] = Assignment[0];
        HAL_UART_Transmit(&huart3, Swuliao, 7, 0xff);
        break;
    case 8:
        ActionFunc(waitAngle);
        HAL_Delay(600);
        Swuliao[2] = Assignment[1];
        HAL_UART_Transmit(&huart3, Swuliao, 7, 0xff);
        break;
    case 9:
        ActionFunc(waitAngle);
        HAL_Delay(600);
        Swuliao[2] = Assignment[2];
        HAL_UART_Transmit(&huart3, Swuliao, 7, 0xff);
        break;

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

    // 移动到第一个拐角
    case 10:
        // 停止找原料区
        // HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff);
        Move_Stop();
        HAL_Delay(1000);
        ActionFunc(lineAngle);

        HAL_Delay(500);
        CLEARFLAG = 1;
        target = 36; // 到拐角（这里距离太短，不开循迹）

        IF_LINE = 1; // 这里手动改变标志位
        IF_MOVE = 1;

        // 用陀螺仪定位
        // 等待完成

        break;

    // 转弯
    case 11:
        IF_LINE = 0; // 手动关闭标志位

        Move_Turnleft(); // 左转
        HAL_Delay(1200);
        break;

    // 移动到粗加工区
    case 12:

        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线
        CLEARFLAG = 1;
        HAL_Delay(1200);
        target = 100; // 拐角到粗加工区的距离
        IF_MOVE = 1;
        break;

    // 粗加工区，调整位置
    case 13:
        // 机械臂动作
        MV_StopSearchLine(); // 停止巡线

        IF_CIRCLE = 1;

        ActionFunc(circleAngle); // 机械臂调整到找圆位置
        HAL_Delay(800);
        HAL_UART_Transmit(&huart2, FCircle, 7, 0x00ff); // 提示openmv找圆，不知道找到的是啥颜色的圆

        break;

    // 到加工区停车
    case 14:
        HAL_UART_Transmit(&huart2, Stop, 7, 0xff);
        Move_Stop();
        break;


    //开始识别颜色
    //要找三次
    case 15:
        ActionFunc (); //打到左边没有回程差的位置
        //这个执行一次后会暂停
        HAL_UART_Transmit (&huart2, Tellcolor, 7, 0xff);
        break;
    
    case 16:
        ActionFunc (); //打到左边没有回程差的位置
        //这个执行一次后会暂停
        HAL_UART_Transmit (&huart2, Tellcolor, 7, 0xff);
        break;
    case 17:
        ActionFunc (); //打到左边没有回程差的位置
        //这个执行一次后会暂停
        HAL_UART_Transmit (&huart2, Tellcolor, 7, 0xff);

        HAL_UART_Transmit (&huart2, Stop, 7, 0xff);
        HAL_Delay (500);
        break;
    case 18:
        ActionFunc (); //最打到左边没有回程差的位置
        colorflag = 0;
        HAL_UART_Transmit (&huart2, Flocation, 7, 0xff);
        

        


























        
        DisposeCargo_Logic();

        HAL_Delay(2000);

        PickCargo_Logic();

        HAL_Delay(1000);

        ActionFunc(lineAngle);

        procedure++;
        break;

    // 移动到第二个拐角
    case 15:
        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线
        HAL_Delay(1200);
        CLEARFLAG = 1;
        target = 103;
        IF_MOVE = 1;
        break;

    // 转弯
    case 16:
        HAL_UART_Transmit(&huart2, Stop, 7, 0xff);
        HAL_Delay(500);
        Move_Stop();
        // MV_StopSearchLine(); // 停止巡线

        HAL_Delay(600);
        Move_Turnleft(); // 左转
        break;

    // 到达精加工区
    case 17:
        HAL_Delay(800);
        Move_Stop();

        HAL_Delay(200);
        ActionFunc(lineAngle);

        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线
        HAL_Delay(1200);
        CLEARFLAG = 1;

        target = 102; // 到达精加工区
        IF_MOVE = 1;

        break;

    // 精加工区,调整位置
    case 18:
        HAL_Delay(600);
        MV_StopSearchLine(); // 停止巡线

        IF_CIRCLE = 1;
        ActionFunc(circleAngle);

        HAL_Delay(800);

        HAL_UART_Transmit(&huart2, FCircle, 7, 0x00ff); // 开始找圆

        break;

    // 精加工区，放置物品
    case 19:
        HAL_Delay(1000);
        HAL_UART_Transmit(&huart2, Stop, 7, 0xff);
        HAL_Delay(500);

        DisposeCargo_Logic(); // 放置物块

        HAL_Delay(800);

        ActionFunc(lineAngle);

        HAL_Delay(600);
        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线
        HAL_Delay(1500);

        procedure++;
        break;

    // 到第三个拐角
    case 20:
        // HAL_Delay(500);
        Move_Stop();
        CLEARFLAG = 1;
        target = 100;
        IF_MOVE = 1;
        break;

    // 转弯
    case 21:
        HAL_Delay(500);
        MV_StopSearchLine(); // 停止巡线

        HAL_Delay(600);
        Move_Turnleft(); // 左转
        break;

    // 回到出发点
    case 22:
        HAL_Delay(2000);

        ActionFunc(lineAngle);

        HAL_Delay(600);

        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开始巡线
        HAL_Delay(1200);
        CLEARFLAG = 1;

        target = 200;
        IF_MOVE = 1;

        // Move_Stop();

        break;

    case 23:
        HAL_UART_Transmit(&huart2, Stop, 7, 0xff);
        HAL_Delay(1000);
        // HAL_UART_Transmit(&huart2, Stop, 7, 0xff);
        // MV_StopSearchLine ();
        // 这里没有线，要改
        Move_Turnleft();
        CLEARFLAG = 1;
        // target = 30;
        // IF_MOVE = 1;
        break;

    case 24:
        Move_Stop();
        HAL_Delay(1000);
        ActionFunc(lineAngle); // 调整到巡线位置

        // HAL_Delay (7000);
        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 发消息给openmv，开启巡线

        HAL_Delay(500);
        CLEARFLAG = 1;

        target = 30 + codeDistance + materialDistance - 30;
        IF_MOVE = 1;

        break;

    // 扫码
    case 25:
        procedure++;

        // HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff);
        // IF_LINE = 0; // 停止巡线
        // HAL_Delay(300);
        // Move_Stop();

        // // ActionFunc (scanCodeAngle);   //扫码角度，未设置

        // // 提示2号openmv开始扫码
        // PawControl (180);
        // HAL_UART_Transmit (&huart3, SCode, 7, 0x00ff);

        // 测试代码
        // 实际：显示任务码后自动procedure++

        break;

    // 移动到圆盘前20cm
    case 26:
        // HAL_UART_Transmit(&huart3, Stop, 7, 0x00ff);
        // // ActionFunc (specialLineAngle);
        // HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线

        // HAL_Delay (600);
        // target = materialDistance - 20;
        // IF_MOVE = 1;

        MV_StopSearchLine();
        procedure++;

        break;

    // 停止找线，向前移动20cm
    case 27:
        ActionFunc(specialLineAngle);
        HAL_Delay(1000);
        // HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff);
        // IF_LINE =0;
        // HAL_Delay(300);

        // IF_LINE = 1;
        CLEARFLAG = 1;
        target = 20;
        IF_MOVE = 1;
        break;

    // 寻找原料区域
    case 28:
        Move_Stop();
        // IF_LINE = 0;
        ActionFunc(stageangle);
        HAL_UART_Transmit(&huart2, Smaterial, 7, 0xff);
        IF_CIRCLE = 1;
        break;
    // 停好车
    case 29:
        HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff);
        // IF_LINE= 0;
        HAL_Delay(300);
        Move_Stop();
        ActionFunc(waitAngle);
        HAL_Delay(600);
        Swuliao[2] = Assignment[3];
        HAL_UART_Transmit(&huart3, Swuliao, 7, 0xff);
        break;
    case 30:
        ActionFunc(waitAngle);
        HAL_Delay(600);
        Swuliao[2] = Assignment[4];
        HAL_UART_Transmit(&huart3, Swuliao, 7, 0xff);
        break;
    case 31:
        ActionFunc(waitAngle);
        HAL_Delay(600);
        Swuliao[2] = Assignment[5];
        HAL_UART_Transmit(&huart3, Swuliao, 7, 0xff);
        break;

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

    // 移动到第一个拐角
    case 32:
        // 停止找原料区
        // HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff);
        HAL_Delay(1000);
        ActionFunc(lineAngle);

        HAL_Delay(500);
        target = 36; // 到拐角（这里距离太短，不开循迹）

        IF_LINE = 1; // 这里手动改变标志位
        IF_MOVE = 1;

        // 用陀螺仪定位
        // 等待完成

        break;

    // 转弯
    case 33:
        IF_LINE = 0; // 手动关闭标志位

        Move_Turnleft(); // 左转
        HAL_Delay(1200);
        break;

    // 移动到粗加工区
    case 34:
        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线

        HAL_Delay(1200);
        target = 100; // 拐角到粗加工区的距离
        IF_MOVE = 1;
        break;

    // 粗加工区，调整位置
    case 35:
        // 机械臂动作
        MV_StopSearchLine(); // 停止巡线

        IF_CIRCLE = 1;

        ActionFunc(circleAngle); // 机械臂调整到找圆位置
        HAL_Delay(800);
        HAL_UART_Transmit(&huart2, FCircle, 7, 0x00ff); // 提示openmv找圆，不知道找到的是啥颜色的圆

        break;

    // 放置物料
    case 36:
        HAL_UART_Transmit(&huart2, Stop, 7, 0xff);
        DisposeCargo_Logic();

        HAL_Delay(2000);

        PickCargo_Logic();

        HAL_Delay(1000);

        ActionFunc(lineAngle);

        procedure++;
        break;

    // 移动到第二个拐角
    case 37:
        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线
        HAL_Delay(1200);
        target = 103;
        IF_MOVE = 1;
        break;

    // 转弯
    case 38:
        HAL_UART_Transmit(&huart2, Stop, 7, 0xff);
        HAL_Delay(500);
        Move_Stop();
        // MV_StopSearchLine(); // 停止巡线

        HAL_Delay(600);
        Move_Turnleft(); // 左转
        break;

    // 到达精加工区
    case 39:
        HAL_Delay(800);
        Move_Stop();

        HAL_Delay(200);
        ActionFunc(lineAngle);

        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线
        HAL_Delay(1200);

        target = 102; // 到达精加工区
        IF_MOVE = 1;

        break;

    // 精加工区,调整位置
    case 40:
        HAL_Delay(600);
        MV_StopSearchLine(); // 停止巡线

        IF_CIRCLE = 1;
        ActionFunc(circleAngle);

        HAL_Delay(800);

        HAL_UART_Transmit(&huart2, FCircle, 7, 0x00ff); // 开始找圆

        break;

    // 精加工区，放置物品
    case 41:
        HAL_Delay(1000);
        // HAL_Delay(500);
        HAL_UART_Transmit(&huart2, Stop, 7, 0xff);

        HAL_Delay(500);
        DisposeCargo_Logic(); // 放置物块
        HAL_Delay(400);

        ActionFunc(lineAngle);

        HAL_Delay(600);
        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开启巡线
        HAL_Delay(1500);

        procedure++;
        break;

    // 到第三个拐角
    case 42:
        HAL_Delay(500);
        target = 100;
        IF_MOVE = 1;
        break;

    // 转弯
    case 43:
        HAL_Delay(500);
        MV_StopSearchLine(); // 停止巡线

        HAL_Delay(600);
        Move_Turnleft(); // 左转
        break;

    // 回到出发点
    case 44:
        HAL_Delay(2000);

        ActionFunc(lineAngle);

        HAL_Delay(600);

        HAL_UART_Transmit(&huart2, SLine, 7, 0x00ff); // 开始巡线
        HAL_Delay(1200);

        target = 200;
        IF_MOVE = 1;

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
    if (flag > 12)
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
