#include "servoDriver.h"

/*

    定义全局变量记录舵机当前角度

    载物台舵机1，2，3 同步转动
    机械臂舵机6，7    同步转动

*/

uint8_t NowAngle_Cargo;
uint8_t NowAngle_Paw;
uint8_t NowAngle_Arm;
int NowAngle_Holder;

/*

    初始化函数：
    提供有参构造和无参构造两种初始化函数
    有参构造作调试用

*/
void InitServo(void)
{

}

void InitServoTest(uint8_t CargoAngle, uint8_t PawAngle, uint8_t HolderAngle, uint8_t ArmAngle)
{
    HAL_TIM_PWM_Start (&htim5, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start (&htim5, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start (&htim5, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start (&htim5, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start (&htim12, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start (&htim12, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start (&htim14, TIM_CHANNEL_1);

    PawControl (PawAngle);
    ArmControl (ArmAngle);
    CargoControl  (CargoAngle);
    HolderControl (HolderAngle);
}


/*

    控制函数:
    >> 检测角度是否正确
    >> 更改占空比
    >> 更新角度nowAngle

*/
void HolderControl(int angle)
{
    int temp = angle+NowAngle_Holder;
    if(temp>=0 && temp<=270)
    {
        Servo5_Angle = (uint32_t)((angle*1.0/270.0)*HOLDER_FULL_ANGLE);
        NowAngle_Holder += angle;
    }
    else
        return;
}

void PawControl(int angle)
{
    int temp = angle+NowAngle_Paw;
    if(temp>=0 && temp<=180)
    {
        Servo4_Angle = (uint32_t)((angle*1.0/180.0)*PAW_FULL_ANGLE);
        NowAngle_Paw += angle;
    }
    else
        return;
}

void ArmControl(int angle)
{
    int temp = angle+NowAngle_Arm;
    if(temp>=0 && temp<=180)
    {
        uint32_t armAjustAngle = (uint32_t)((angle*1.0/180.0)*ARM_FULL_ANGLE);

        Servo6_Angle = armAjustAngle;
        Servo7_Angle = armAjustAngle;

        NowAngle_Arm += angle;
    }
    else
        return;
}

void CargoControl(int angle)
{
    int temp = angle+NowAngle_Cargo;
    if(temp>=0 && temp<=180)
    {
        uint32_t cargoAjustAngle = (uint32_t)((angle*1.0/180.0)*CARGO_FULL_ANGLE);

        Servo1_Angle = cargoAjustAngle;
        Servo2_Angle = cargoAjustAngle;
        Servo3_Angle = cargoAjustAngle;

        NowAngle_Cargo += angle;
    }
    else
        return;
}
