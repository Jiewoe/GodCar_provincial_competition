
#include "motor.h"

uint8_t round1 = 0; 
uint8_t round2 = 0;
uint8_t round3 = 0;
uint8_t round4 = 0;

//单位厘米
uint8_t target = 0;
uint8_t procedure = 0;
uint8_t IF_MOVE = 0;

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
    round1 = round2 = round3 = round4 = 0;
    Motor1_CNT = Motor2_CNT = Motor3_CNT = Motor4_CNT = 0;
}




void Move_Forward ()
{
    short Piancha = target - ((float)(round1 + round3)/2 + (float)(Motor1_CNT + Motor3_CNT)/120000)*3.14159*6;
    //=====完成操作=======
    if (Piancha < 2)
    {
        target = 0;
        IF_MOVE = 0;
        Motor1_Speed = Motor3_Speed = 0; //停车
        return;
    }
    //======进行线性启动=======
    //最低速度为500 
    if (target-Piancha < 10)
    {
        Motor3_Speed = Motor1_Speed = max (500, (target - Piancha)*100);//在这里调整最大速度
        return;
    }
    //======进行线性停止=======
    if (Piancha < 10)
    {
        Motor3_Speed = Motor1_Speed = max (500, (Piancha)*100);
        return;
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
}


void Lateral_correction (short piancha)
{
    //相应阈值调整
    if (piancha > 5)
    {
        //先调整车轮旋转方向
        //在调整车轮速度
        return;
    }
    if (piancha < -5)
    {
        //调整车轮方向
        //调整车轮速度
        return;
    }
}



int max (int a, int b)
{
    if (a>b)
    return a;
    return b;
}