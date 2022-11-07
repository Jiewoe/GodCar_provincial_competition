#ifndef __SERVO_DRIVER_H__
#define __SERVO_DRIVER_H__

#include "stm32h7xx_hal.h"

//计数器最大值
#define TIM5_ARR  19999
#define TIM12_ARR 19999
#define TIM14_ARR 19999

/*

    舵机时基脉冲 20ms
    角度对应比例
    0.5ms--------------0度；
    1.0ms------------45度；
    1.5ms------------90度；
    2.0ms-----------135度；
    2.5ms-----------180度；

    定义最大角度对应占空比比例

*/
#define ANGLE_180   0.125
#define ANGLE_270   0.125

/*

    各个舵机转动的最大角度
    HOLDER 云台
    ARM    机械臂
    CARGO  货斗
    PAW    爪架

*/
#define PAW_FULL_ANGLE      ((TIM5_ARR)*(ANGLE_180))
#define CARGO_FULL_ANGLE    ((TIM5_ARR)*(ANGLE_180))
#define HOLDER_FULL_ANGLE   ((TIM14_ARR)*(ANGLE_270))
#define ARM_FULL_ANGLE      ((TIM12_ARR)*(ANGLE_180))


/*

    servo1, 2 and 3     载物台舵机
    servo4              爪架舵机
    servo5              云台舵机
    servo6 and 7        大臂起降舵机



    servo1, 2, 3, 4     TIM5_channel_1, 2, 3, 4
    servo5              TIM14_channel_1
    servo6,7            TIM12_channel_2, TIM12_channel_1

*/
#define Servo1_Angle (TIM5->CCR1)
#define Servo2_Angle (TIM5->CCR2)
#define Servo3_Angle (TIM5->CCR3)
#define Servo4_Angle (TIM5->CCR4)
#define Servo5_Angle (TIM14->CCR1)
#define Servo6_Angle (TIM12->CCR1)
#define Servo7_Angle (TIM12->CCR2)

extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim14;

extern uint8_t NowAngle_Cargo;
extern uint8_t NowAngle_Paw;
extern uint8_t NowAngle_LeftArm;
extern uint8_t NowAngle_RightArm;
extern uint16_t NowAngle_Holder;

void InitServo(void);
void InitServoTest(uint8_t CargoAngle, uint8_t PawAngle, uint16_t HolderAngle, uint8_t LeftArmAngle, uint8_t RightArmAngle);
void HolderControl(uint16_t angle);
void PawControl(uint8_t angle);
void ArmControl(uint8_t LeftAngle, uint8_t RightAngle);
void CargoControl(uint8_t angle);

#endif
