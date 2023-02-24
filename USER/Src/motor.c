
#include "motor.h"

uint8_t round1 = 0; 
uint8_t round2 = 0;
uint8_t round3 = 0;
uint8_t round4 = 0;

//单位厘米
uint8_t target = 0;

//状态控制
uint8_t procedure = 0;

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
        return;
    }
    //======进行线性启动=======
    //最低速度为500 
    if (target-Piancha < 10)
    {
        Motor2_Speed = Motor4_Speed = max (800, (target - Piancha)*200);//在这里调整最大速度
        return;
    }
    //======进行线性停止=======
    if (Piancha < 10)
    {
        Motor2_Speed = Motor4_Speed = max (500, (Piancha)*200);
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
    Motor1_Speed = Motor2_Speed = 0;
}

//向左转弯
void Move_left()
{
    HAL_GPIO_WritePin (Motor_GPIO, Motor2_Pin|Motor2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin (Motor_GPIO, Motor3_Pin|Motor4_Pin, GPIO_PIN_SET);
    Motor2_Speed = Motor1_Speed = 1500;
    Motor4_Speed = Motor3_Speed = 6000-1500;
    HAL_Delay (1000);
    Motor_Init();
}



int max (int a, int b)
{
    if (a>b)
    return a;
    return b;
}