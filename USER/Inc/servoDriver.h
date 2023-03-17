#ifndef __SERVO_DRIVER_H__
#define __SERVO_DRIVER_H__

#include "stm32h7xx_hal.h"
#include "sys.h"
#include "openmv.h"

//计数器最大值


#define PI 3.1415



#define TIM5_ARR  19999
#define TIM12_ARR 19999
#define TIM14_ARR 19999

/*

    载物台是否开启状态
    载物台编号

*/
#define CARGO_SET   1
#define CARGO_UNSET 0

#define CARGO_1 1
#define CARGO_2 2
#define CARGO_3 3

/*

    舵机时基脉冲 20ms
    角度对应比例(180度舵机为例)
    0.5ms--------------0度；
    1.0ms------------45度；
    1.5ms------------90度；
    2.0ms-----------135度；
    2.5ms-----------180度；


    定义最大角度对应占空比比例
    0.1 = 2/20
    0.025 = 0.5/20

*/
#define ANGLE_180   0.1
#define ANGLE_270   0.1
#define ANGLE_0     0.025
#define ANGLE_45    0.04
#define ANGLE_135   0.075

/*

    各个舵机转动的最大角度
    HOLDER 云台
    ARM    机械臂
    CARGO  货斗
    PAW    爪架

*/
#define PAW_FULL_ANGLE      ((TIM5_ARR)*(ANGLE_180))
#define CARGO_FULL_ANGLE    ((TIM5_ARR)*(ANGLE_45))
#define HOLDER_FULL_ANGLE   ((TIM14_ARR)*(ANGLE_270))
#define ARM_FULL_ANGLE      ((TIM12_ARR)*(ANGLE_180))
#define ZERO_ANGLE          ((uint16_t)((TIM12_ARR)*(ANGLE_0)))         //这里只用tim12的计数最大值定义了,计数区间不出意外的话应该都是一样的



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

/*
    动作参数结构体
    rightArm    右臂角度
    leftArm     左臂角度
    holder      云台角度
    CargoNo     货架编号
*/
typedef struct CargoActionPara
{
    uint8_t rightArm;
    uint8_t leftArm;
    float holder;
    uint8_t cargoNo;

} ActionParameter;

extern ActionParameter Cargo1_ActionDown;
extern ActionParameter Cargo1_ActionUp;
extern ActionParameter Cargo2_ActionDown;
extern ActionParameter Cargo2_ActionUp;
extern ActionParameter Cargo3_ActionDown;
extern ActionParameter Cargo3_ActionUp;

extern ActionParameter Cargo1_FetchDown;
extern ActionParameter Cargo1_FetchUp;
extern ActionParameter Cargo2_FetchDown;
extern ActionParameter Cargo2_FetchUp;
extern ActionParameter Cargo3_FetchDown;
extern ActionParameter Cargo3_FetchUp;

extern ActionParameter Board1_GroundDown;
extern ActionParameter Board3_GroundDown;
extern ActionParameter Board2_GroundDown;

extern ActionParameter freeAngle;
extern ActionParameter lineAngle;
extern ActionParameter circleAngle;
extern ActionParameter scanCodeAngle;

extern ActionParameter stageangle;
extern ActionParameter specialLineAngle;
extern ActionParameter materialAngle;
extern ActionParameter waitAngle;
extern ActionParameter Calculate_angle;
extern ActionParameter farcircleagnle;

extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim14;

extern uint8_t NowAngle_Cargo;
extern uint8_t NowAngle_Paw;
extern uint8_t NowAngle_LeftArm;
extern uint8_t NowAngle_RightArm;
extern float NowAngle_Holder;

extern uint8_t  CargoStatus_1;
extern uint8_t  CargoStatus_2;
extern uint8_t  CargoStatus_3;

extern uint8_t cargo_flag;

void Servo_Init(void);
void HolderControl(float angle);
void HolderSet(float angle);
void PawControl(uint8_t angle);
void LeftArmControl (uint8_t LeftAngle);
void RightArmControl (uint8_t RightAngle);
void Servo_Init(void);
void CargoSet(uint8_t Cargo_pos, uint8_t SetStatus);
void CargoAction (ActionParameter up, ActionParameter down);
void CargoFetch(ActionParameter up, ActionParameter down);
void PickCargo_Yuanliao (uint8_t color);
void PickCargo_Ground(ActionParameter down);
void ActionFunc(ActionParameter angle);
void PickCargo_Logic(void);
void DisposeCargo_Logic(void);
void DisposeCargo_Ground(ActionParameter down);

void Servo_process(uint8_t zhengfu, uint8_t yuntai, uint8_t jixiebifuhao, uint8_t jixiebipiancha);
/*
    1---红色
    2---绿色
    3---蓝色
*/

#endif
