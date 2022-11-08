#include "servoDriver.h"

/*

    定义全局变量记录舵机当前角度

    载物台舵机1，2，3 同步转动
    机械臂舵机6，7    同步转动

*/

uint8_t NowAngle_Cargo;
uint8_t NowAngle_Paw;
uint8_t NowAngle_LeftArm;
uint8_t NowAngle_RightArm;
uint16_t NowAngle_Holder;

/*

    初始化函数：
    提供有参构造和无参构造两种初始化函数
    有参构造作调试用

    无参构造删除 貌似是没什么用

*/


void InitServoTest(uint8_t CargoAngle, uint8_t PawAngle, uint16_t HolderAngle, uint8_t LeftArmAngle, uint8_t RightArmAngle)
{
    HAL_TIM_PWM_Start (&htim5, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start (&htim5, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start (&htim5, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start (&htim5, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start (&htim12, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start (&htim12, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start (&htim14, TIM_CHANNEL_1);

    PawControl (PawAngle);
    ArmControl (LeftArmAngle, RightArmAngle);
    CargoControl  (CargoAngle);
    HolderControl (HolderAngle);
}


/*

    初始化
    一般使用

*/

void Servo_Init()
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
        Servo5_Angle = (uint32_t)((angle*1.0/270.0)*HOLDER_FULL_ANGLE)+500;
        NowAngle_Holder = angle;
    }
    else
        return;
}

void PawControl(uint8_t angle)
{
    if(angle<=180)
    {
        Servo4_Angle = (uint32_t)((angle*1.0/180.0)*PAW_FULL_ANGLE)+500;
        NowAngle_Paw = angle;
    }
    else
        return;
}


//未改
void ArmControl(uint8_t LeftAngle, uint8_t RightAngle)
{
    if(LeftAngle<=180)
    {
        Servo6_Angle = (uint32_t)((LeftAngle*1.0/180.0)*ARM_FULL_ANGLE);

        NowAngle_LeftArm = LeftAngle;
    }
    if (RightAngle<=180)
    {
        Servo7_Angle = (uint32_t)((RightAngle*1.0/180.0)*ARM_FULL_ANGLE);

        NowAngle_RightArm = RightAngle;
    }
    else
        return;
}

void CargoControl(uint8_t angle)
{
    if(angle<=180)
    {
        uint32_t cargoAjustAngle = (uint32_t)((angle*1.0/180.0)*CARGO_FULL_ANGLE);

        Servo1_Angle = cargoAjustAngle;
        Servo2_Angle = cargoAjustAngle;
        Servo3_Angle = cargoAjustAngle;

        NowAngle_Cargo = angle;
    }
    else
        return;
}
