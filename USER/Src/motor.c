
#include "motor.h"
#include "openmv.h"

uint8_t round1 = 0;
uint8_t round2 = 0;
uint8_t round3 = 0;
uint8_t round4 = 0;

// 单位厘米
uint8_t target = 0;
uint8_t IF_OUT = 0;
uint8_t left_target = 0;
uint8_t right_target = 0;

/*
    滤波队列

*/
uint8_t DataCheck_flag = 0; // 数据是否进行检验标志位，如果还在收集数据，则为0，收集完为1
uint8_t hengPian_flag = 0;  // 横向偏差是否有效标志位，决定本次数据是否进行修正，0为无效，1有效（进行修正）
uint8_t zongPian_flag = 0;  // 纵向偏差是否有效标志位
Queue circleData = {0, {{0, 0}, {0, 0}, {0, 0}}};

// 状态控制
// 这在里可以增加开始按钮
uint8_t procedure = 0;

// 是否移动
uint8_t IF_MOVE = 0;

// motor2 60000-x
//============初始化轮子============
void Motor_Init()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_Base_Start_IT(&htim4);
    HAL_TIM_Base_Start_IT(&htim8);
    round1 = round2 = round3 = round4 = 0;
    Motor1_CNT = Motor2_CNT = Motor3_CNT = Motor4_CNT = 5000;
    Motor1_Speed = Motor2_Speed = Motor3_Speed = Motor4_Speed = 0;
    HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin | Motor2_Pin | Motor3_Pin | Motor4_Pin, GPIO_PIN_RESET);
}

void Move_Stop(void)
{
    round1 = round2 = round3 = round4 = 0;
    Motor1_CNT = Motor2_CNT = Motor3_CNT = Motor4_CNT = 5000;
    Motor1_Speed = Motor2_Speed = Motor3_Speed = Motor4_Speed = 0;
    HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin | Motor2_Pin | Motor3_Pin | Motor4_Pin, GPIO_PIN_RESET);
}

void Move_Forward()
{
    int Piancha = target - (double)(((float)(round2 + round4) / 2 + (float)(Motor2_CNT + Motor4_CNT) / 120000) * 3.14 * 6);
    printf("偏差是%d\n", Piancha);
    printf("CNT:%d", Motor2_CNT);
    printf("round:%d", round2);
    //=====完成操作=======
    if (Piancha < 2)
    {
        target = 0;
        IF_MOVE = 0;
        Motor2_Speed = Motor4_Speed = 0; // 停车
        Motor2_CNT = Motor4_CNT = 0;
        round2 = round4 = 0;
        procedure++;
        return;
    }
    //======进行线性启动=======
    // 最低速度为500
    if (target - Piancha < 10)
    {
        Motor2_Speed = Motor4_Speed = max(1000, (target - Piancha) * 250); // 在这里调整最大速度
        return;
    }
    //======进行线性停止=======
    if (Piancha < 10)
    {
        Motor2_Speed = Motor4_Speed = max(1000, (Piancha)*250);
        return;
    }
}
/*
    移出启停区
*/
void Move_Out()
{
    int Piancha = target - (double)(((float)(round2 + round4) / 2 + (float)(Motor2_CNT + Motor4_CNT) / 120000) * 3.14 * 6);
    printf("偏差是%d\n", Piancha);
    printf("CNT:%d", Motor2_CNT);
    printf("round:%d", round2);
    //=====完成操作=======
    if (Piancha < 2)
    {
        target = 0;
        IF_OUT = 0;
        Motor2_Speed = Motor4_Speed = 0; // 停车
        Motor3_Speed = Motor1_Speed = 0;
        Motor1_CNT = Motor3_CNT = 0;
        Motor2_CNT = Motor4_CNT = 0;
        round2 = round4 = 0;
        procedure++;
        return;
    }
    //======进行线性启动=======
    // 最低速度为500
    if (target - Piancha < 10)
    {
        Motor1_Speed = Motor3_Speed = Motor2_Speed = Motor4_Speed = max(1000, (target - Piancha) * 250); // 在这里调整最大速度
        return;
    }
    //======进行线性停止=======
    if (Piancha < 10)
    {
        Motor1_Speed = Motor3_Speed = Motor2_Speed = Motor4_Speed = max(1000, (Piancha)*250);
        return;
    }
}

/*
    偏差修正

    偏差为角度值
    即车辆法线与赛道方向的夹角

    piancha 偏差绝对值(角度)
    sign    符号标识 为1时偏差大于0（左偏） 为0时偏差小于0（右偏）

    sign2   符号标识
    hengPia 横向偏差

    编码器计数方案
*/
void Lateral_correction(uint8_t sign, uint8_t piancha, uint8_t sign2, uint8_t hengPia)
{
    if(IF_FINISHLINE == 1) return;    //巡线未开启（或被关闭）直接退出, 只有在开启找圆前才用这个标志位，为了防止从巡线角度打到找圆角度时，巡线剩余消息进入导致车辆乱动

    
    uint16_t move_speed = 800; 
    uint16_t rotate_speed = 1700;
    if (hengPia > 120)
    {
        sign2 = 0;
        hengPia = hengPia - 120;
    }
    else
    {
        sign2 = 1;
        hengPia = 120 - hengPia;
    }
    if (piancha > 3)
    {
        
        if (sign == 0) // 整车左偏
        {
            HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(Motor_GPIO, Motor3_Pin, GPIO_PIN_RESET);
            Motor1_Speed = 6000 - rotate_speed;
            Motor3_Speed = rotate_speed;
        }
        else if (sign == 1) // 整车右偏
        {
            HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(Motor_GPIO, Motor3_Pin, GPIO_PIN_SET);
            Motor1_Speed = rotate_speed;
            Motor3_Speed = 6000 - rotate_speed;
        }
    }
    if (piancha < 3)
    {
        HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Motor_GPIO, Motor3_Pin, GPIO_PIN_RESET);
        Motor1_Speed = 6000;
        Motor3_Speed = 0;
        if (hengPia > 20)
        {
            if (sign2 == 0)
            {
                HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(Motor_GPIO, Motor3_Pin, GPIO_PIN_SET);
                Motor1_Speed = 6000 - move_speed;
                Motor3_Speed = 6000 - move_speed;
            }
            else
            {
                HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(Motor_GPIO, Motor3_Pin, GPIO_PIN_RESET);
                Motor1_Speed = move_speed;
                Motor3_Speed = move_speed;
            }
        }
        else
        {
            HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(Motor_GPIO, Motor3_Pin, GPIO_PIN_RESET);
            Motor1_Speed = 6000;
            Motor3_Speed = 0;

            IF_LINE = 1;
        }
    }
}

/*
    根据圆心位置调整

    symheng 横向标志位 0 为正
    heng 横向偏差
    symzong 纵向标志位 1 为负
    zong 纵向偏差



*/
void MOVE_MV_micro(uint8_t symheng, uint8_t heng, uint8_t symzong, uint8_t zong)
{
    uint16_t speed = 780;
    if (IF_CIRCLE != 1) //置1是执行
        return;
    if (heng > 12)
    {
        if (symheng == 0)
        {
            HAL_GPIO_WritePin(Motor_GPIO, Motor2_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(Motor_GPIO, Motor4_Pin, GPIO_PIN_SET);
            Motor2_Speed = 6000 - speed;
            Motor4_Speed = 6000 - speed;
        }
        else if (symheng == 1)
        {
            HAL_GPIO_WritePin(Motor_GPIO, Motor2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(Motor_GPIO, Motor4_Pin, GPIO_PIN_RESET);
            Motor2_Speed = speed;
            Motor4_Speed = speed;
        }
    }
    else
    {
        HAL_GPIO_WritePin(Motor_GPIO, Motor2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Motor_GPIO, Motor4_Pin, GPIO_PIN_RESET);
        Motor2_Speed = 0;
        Motor4_Speed = 0;
    }
    if (zong > 12) // 阈值调整
    {
        if (symzong == 1)
        {
            HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(Motor_GPIO, Motor3_Pin, GPIO_PIN_SET);
            Motor1_Speed = 6000 - speed;
            Motor3_Speed = 6000 - speed;
        }
        else if (symzong == 0)
        {
            HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(Motor_GPIO, Motor3_Pin, GPIO_PIN_RESET);
            Motor1_Speed = speed;
            Motor3_Speed = speed;
        }
    }
    else
    {
        HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Motor_GPIO, Motor3_Pin, GPIO_PIN_RESET);
        Motor1_Speed = 0;
        Motor3_Speed = 0;
    }
    if (heng < 12 && zong < 12)
    {
        Move_Stop();
        IF_CIRCLE = 0;

        IF_FINISHLINE = 0;
        procedure ++;
    }
}

// void Move_Stop(void)
// {
//     HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin, GPIO_PIN_RESET);
//     HAL_GPIO_WritePin(Motor_GPIO, Motor3_Pin, GPIO_PIN_RESET);
//     Motor1_Speed = 0;
//     Motor3_Speed = 0;

//     HAL_GPIO_WritePin(Motor_GPIO, Motor2_Pin, GPIO_PIN_RESET);
//     HAL_GPIO_WritePin(Motor_GPIO, Motor4_Pin, GPIO_PIN_RESET);
//     Motor2_Speed = 0;
//     Motor4_Speed = 0;

// }

// 向左转弯
void Move_Turnleft()
{
    HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin | Motor2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_GPIO, Motor3_Pin | Motor4_Pin, GPIO_PIN_SET);
    Motor2_Speed = Motor1_Speed = 1500;
    Motor4_Speed = Motor3_Speed = 6000 - 1500;
    HAL_Delay(1100);

    // 这里要改
    procedure++;
    Motor_Init();
}

/*

    通过编码器计数改变方向

    angle 转向角度 （0~360）

    逆时针转向为正

    motor2_cnt ++
    motor4_cnt --

    (angle/360) * (pai * d) / 6 = round2
    ((angle/360) * (pai * d) % 6) * 60000 = cnt

    废弃………………

*/
void moveDirection(uint16_t angle)
{

    HAL_GPIO_WritePin(Motor_GPIO, Motor1_Pin | Motor2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_GPIO, Motor3_Pin | Motor4_Pin, GPIO_PIN_SET);
}

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

void printCnt(void)
{
    // printf("\nround1 and round2: %d %d\n", round1, round2);
    // printf("round3 and round4: %d %d\n", round3, round4);
    printf("cnt1 and cnt2: %d %d\n", Motor1_CNT, Motor2_CNT);
    printf("cnt3 and cnt4: %d %d\n", Motor3_CNT, Motor4_CNT);
}



void DataCheck(uint8_t *openmv)
{
    uint8_t range = 5; // 检测范围

    if (DataCheck_flag == 1)
    {
        if (fabs(openmv[3] - (circleData.hengPian[0] + circleData.hengPian[1]) / 2) < range) // 新加入数据在允许浮动范围之内，则新数据存入队列，删除一个老数据
        {
            circleData.hengPian[0] = circleData.hengPian[1]; // 数据前移，删除老数据
            circleData.hengPian[1] = openmv[3];              // 更新本组数据
            hengPian_flag = 1;
        }
        else
        {
            hengPian_flag = 0;
        }

        if (fabs(openmv[5] - (circleData.zongPian[0] + circleData.zongPian[1]) / 2) < range)
        {
            circleData.zongPian[0] = circleData.zongPian[1];
            circleData.zongPian[1] = openmv[3];
            zongPian_flag = 1;
        }
        else
        {
            zongPian_flag = 0;
        }
    }
    else // 初始情况收集三组数据
    {
        circleData.hengPian[circleData.length] = openmv[3]; // 将openmv的有效数据存入队列中，下同
        circleData.zongPian[circleData.length] = openmv[5];

        circleData.length++; // 存入一组数据后，队列长度+1

        if (circleData.length == 3) // 如果收集到三组数据，进行筛选操作
        {
            DataCheck_flag = 1; // 标志位计1，不再进入收集数据

            insertSort(circleData.hengPian);
            insertSort(circleData.zongPian);

            comparePiancha(circleData.hengPian);
            comparePiancha(circleData.zongPian);

            circleData.length--; // 剔除数据后，队列长度减一
        }
    }
}

/*
    初始三组数据比较
*/
void comparePiancha(uint8_t *piancha)
{
    if (fabs(piancha[0] - piancha[1]) > fabs(piancha[1] - piancha[2]))
    // 比较第一个和第三个数据与第二个数据的差距，去除大的那个，并填充队列，队列第三个数据置0
    {
        piancha[0] = piancha[1];
        piancha[1] = piancha[2];
        piancha[2] = 0;
    }
    else
    {
        piancha[2] = 0;
    }
}

/*
    插入排序
*/
void insertSort(uint8_t *arr)
{
    for (uint8_t i = 1; i < 3; i++)
    {
        for (uint8_t j = i - 1; j >= 0; j--)
        {
            if (arr[j] > arr[j + 1])
            {
                uint8_t temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
            else
                break;
        }
    }
}
