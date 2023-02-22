#include "servoDriver.h"

/*

    定义全局变量记录舵机当前角度

    载物台舵机1，2，3 分别转动
    机械臂舵机6，7    分别转动

*/

uint8_t  NowAngle_Cargo;
uint8_t  NowAngle_Paw;
uint8_t  NowAngle_LeftArm;
uint8_t  NowAngle_RightArm;
uint16_t NowAngle_Holder;

uint8_t  CargoStatus_1;
uint8_t  CargoStatus_2;
uint8_t  CargoStatus_3;

/*

    初始化函数：
    初始化舵机角度
    需要先进行pwm输出初始化：Servo_Init(void)

*/
void InitServoAngle(uint8_t CargoAngle, uint8_t PawAngle, uint16_t HolderAngle, uint8_t LeftArmAngle, uint8_t RightArmAngle)
{

    PawControl (PawAngle);
    LeftArmControl (LeftArmAngle);
    RightArmControl(RightArmAngle);
    HolderControl (HolderAngle);
    
    //载物台初始化没写进去，载物台角度多少为打开，多少为关闭
    //CargoSet(CARGO_1, CARGO_UNSET);

}


/*

    初始化
    一般使用
    pwm输出开启

*/
void Servo_Init(void)
{
    HAL_TIM_PWM_Start (&htim5, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start (&htim5, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start (&htim5, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start (&htim5, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start (&htim12, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start (&htim12, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start (&htim14, TIM_CHANNEL_1);
}




/*

    控制函数:
    >> 检测角度是否正确
    >> 更改占空比
    >> 更新角度nowAngle

*/

void HolderControl(uint16_t angle)
{
    if(angle<=270)
    {
        Servo5_Angle = (uint32_t)((angle*1.0/270.0)*HOLDER_FULL_ANGLE)+ZERO_ANGLE;
        NowAngle_Holder = angle;
    }
    else
        return;
}

void PawControl(uint8_t angle)
{
    if(angle<=180)
    {
        Servo4_Angle = (uint32_t)((angle*1.0/180.0)*PAW_FULL_ANGLE)+ZERO_ANGLE;
        NowAngle_Paw = angle;
    }
    else
        return;
}


void LeftArmControl(uint8_t LeftAngle)
{
    if(LeftAngle<=180)
    {
        Servo6_Angle = (uint32_t)((LeftAngle*1.0/180.0)*ARM_FULL_ANGLE)+ZERO_ANGLE;

        NowAngle_LeftArm = LeftAngle;
    }
    else
        return;
}

void RightArmControl (uint8_t RightAngle)
{
    if (RightAngle<=180)
    {
        Servo7_Angle = (uint32_t)((RightAngle*1.0/180.0)*ARM_FULL_ANGLE)+ZERO_ANGLE;

        NowAngle_RightArm = RightAngle;
    }
    else
        return;
}

/*
    CargoSet:调整指定载物台的状态

    Cargo_pos:载物台的位置

        CARGO_1 1
        CARGO_2 2
        CARGO_3 3

    SetStatus:设置状态

        CARGO_SET   1 开启载物台
        CARGO_UNSET 0 关闭载物台

*/
void CargoSet(uint8_t Cargo_pos, uint8_t SetStatus)
{
    switch (Cargo_pos)
    {
        case CARGO_1:
        {
            if (SetStatus != CargoStatus_1)
            {
                if (SetStatus == CARGO_SET)
                {
                    Servo1_Angle = (uint32_t)(CARGO_FULL_ANGLE+ZERO_ANGLE-50);
                    CargoStatus_1 = SetStatus;
                }
                else if (SetStatus == CARGO_UNSET)
                {
                    Servo1_Angle = (uint32_t)(ZERO_ANGLE);
                    CargoStatus_1 = SetStatus;
                }
            }
            else
                return;
            break;
        }
        case CARGO_2:
        {
            if (SetStatus != CargoStatus_2)
            {
                if (SetStatus == CARGO_SET)
                {
                    Servo2_Angle = (uint32_t)(CARGO_FULL_ANGLE+ZERO_ANGLE);
                    CargoStatus_2 = SetStatus;
                }
                else if (SetStatus == CARGO_UNSET)
                {
                    Servo2_Angle = (uint32_t)(ZERO_ANGLE);
                    CargoStatus_2 = SetStatus;
                }
            }
            else
                return;
            break;
        }
        case CARGO_3:
        {
            if (SetStatus != CargoStatus_3)
            {
                if (SetStatus == CARGO_SET)
                {
                    Servo3_Angle = (uint32_t)(CARGO_FULL_ANGLE+ZERO_ANGLE-50);
                    CargoStatus_3 = SetStatus;
                }
                else if (SetStatus == CARGO_UNSET)
                {
                    Servo3_Angle = (uint32_t)(ZERO_ANGLE);
                    CargoStatus_3 = SetStatus;
                }
            }
            else
                return;
            break;
        }
        default:
            return;
    }
}