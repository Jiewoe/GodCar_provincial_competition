
#include "motor.h"

uint8_t round1 = 0; 
uint8_t round2 = 0;
uint8_t round3 = 0;
uint8_t round4 = 0;

//单位厘米
uint8_t target = 0;

//状态控制
//这在里可以增加开始按钮
uint8_t procedure = 1;

//是否移动
uint8_t IF_MOVE = 0;

//motor2 60000-x
//============初始化轮子============
void Motor_Init()
{
    HAL_TIM_PWM_Start (&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start (&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start (&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start (&htim1, TIM_CHANNEL_4);
    HAL_TIM_Encoder_Start (&htim8, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start (&htim2, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start (&htim3, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start (&htim4, TIM_CHANNEL_ALL);
    HAL_TIM_Base_Start_IT (&htim2);
    HAL_TIM_Base_Start_IT (&htim3);
    HAL_TIM_Base_Start_IT (&htim4);
    HAL_TIM_Base_Start_IT (&htim8);
    round1 = round2 = round3 = round4 = 0;
    Motor1_CNT = Motor2_CNT = Motor3_CNT = Motor4_CNT = 5000;
    Motor1_Speed = Motor2_Speed = Motor3_Speed = Motor4_Speed = 0;
    HAL_GPIO_WritePin (Motor_GPIO, Motor1_Pin|Motor2_Pin|Motor3_Pin|Motor4_Pin, GPIO_PIN_RESET);
}




void Move_Forward ()
{
    int Piancha = target - (double)(((float)(round2 + round4)/2 + (float)(Motor2_CNT + Motor4_CNT)/120000)*3.14*6);
    printf("偏差是%d\n", Piancha);
    printf("CNT:%d", Motor2_CNT);
    printf("round:%d", round2);
    //=====完成操作=======
    if (Piancha < 2)
    {
        target = 0;
        IF_MOVE = 0;
        Motor2_Speed = Motor4_Speed = 0; //停车
        Motor2_CNT = Motor4_CNT = 0;
        round2 = round4 = 0;
        procedure ++;
        return;
    }
    //======进行线性启动=======
    //最低速度为500 
    if (target-Piancha < 10)
    {
        Motor2_Speed = Motor4_Speed = max (1000, (target - Piancha)*250);//在这里调整最大速度
        return;
    }
    //======进行线性停止=======
    if (Piancha < 10)
    {
        Motor2_Speed = Motor4_Speed = max (1000, (Piancha)*250);
        return;
    }
}
    
    //else if (Piancha < 30)
    // {
    //     if (Piancha < 5)
    //     {
    //         //完成移动
    //         Motor1_Speed = Motor3_Speed = 0;
    //         target = 0;
    //         IF_MOVE = 0;
    //     }
    //     Motor3_Speed = Motor1_Speed = Piancha*100;
    //     return;
    // }else
    // {
    //     Motor1_Speed = Motor3_Speed = 3000;
    //     return;
    // }


/*
    偏差修正

    偏差为角度值
    即车辆法线与赛道方向的夹角
    
    piancha 偏差绝对值
    sign    符号标识 为1时偏差大于0（左偏） 为0时偏差小于0（右偏）

    编码器计数方案
*/
void Lateral_correction (uint8_t  piancha, uint8_t sign)
{
    if (piancha > 4)
    {
        if (sign == 1)    //整车左偏
        {
            HAL_GPIO_WritePin (Motor_GPIO, Motor1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin (Motor_GPIO, Motor3_Pin, GPIO_PIN_RESET);
            Motor1_Speed = 6000-piancha * 100;
            Motor3_Speed = piancha *100;
        }
        else if (sign == 0)   //整车右偏
        {
            HAL_GPIO_WritePin (Motor_GPIO, Motor1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin (Motor_GPIO, Motor3_Pin, GPIO_PIN_SET);
            Motor1_Speed = piancha *100;
            Motor3_Speed = 6000-piancha * 100;

        }
    }
    // Motor1_Speed = 500;
    // Motor3_Speed = 6000-500;

    

}

//向左转弯
void Move_left()
{
    HAL_GPIO_WritePin (Motor_GPIO, Motor1_Pin|Motor2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin (Motor_GPIO, Motor3_Pin|Motor4_Pin, GPIO_PIN_SET);
    Motor2_Speed = Motor1_Speed = 1500;
    Motor4_Speed = Motor3_Speed = 6000-1500;
    HAL_Delay (1100);
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

    HAL_GPIO_WritePin (Motor_GPIO, Motor1_Pin|Motor2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin (Motor_GPIO, Motor3_Pin|Motor4_Pin, GPIO_PIN_SET);



}

  


int max (int a, int b)
{
    if (a>b)
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