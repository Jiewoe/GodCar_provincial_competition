#include "servoDriver.h"

/*

    定义全局变量记录舵机当前角度

    载物台舵机1，2，3 同步转动
    机械臂舵机6，7    分别转动

*/

uint8_t  NowAngle_Cargo;
uint8_t  NowAngle_Paw;
uint8_t  NowAngle_LeftArm;
uint8_t  NowAngle_RightArm;
uint16_t NowAngle_Holder;

uint8_t  CargoStatus;

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
    CargoControl  (CargoAngle);
    HolderControl (HolderAngle);

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

void CargoControl(uint8_t angle)
{
    if(angle<=180)
    {
        uint32_t cargoAjustAngle = (uint32_t)((angle*1.0/180.0)*CARGO_FULL_ANGLE)+ZERO_ANGLE;

        Servo1_Angle = cargoAjustAngle;
        Servo2_Angle = cargoAjustAngle;
        Servo3_Angle = cargoAjustAngle;

        NowAngle_Cargo = angle;
    }
    else
        return;
}

/*
    CargoSet: 开启或者关闭载物台

    SetStatus:
        CARGO_SET   1 开启载物台
        CARGO_UNSET 0 关闭载物台
*/
void CargoSet(uint8_t SetStatus)
{
    if (SetStatus != CargoStatus)
    {
        if (SetStatus == CARGO_SET)
        {
            CargoControl((uint8_t)180);
            CargoStatus = SetStatus;
        }
        else if (SetStatus == CARGO_UNSET)
        {
            CargoControl ((uint8_t)0);
            CargoStatus = SetStatus;
        }
    }
    else
        return;
}