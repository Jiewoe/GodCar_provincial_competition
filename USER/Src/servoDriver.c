#include "servoDriver.h"

/*

    定义全局变量记录舵机当前角度

    载物台舵机1，2，3 分别转动
    机械臂舵机6，7    分别转动

*/

// uint8_t  NowAngle_Cargo;
// uint8_t  NowAngle_Paw;
// uint8_t  NowAngle_LeftArm;
// uint8_t  NowAngle_RightArm;
// uint16_t NowAngle_Holder;

uint8_t  CargoStatus_1;
uint8_t  CargoStatus_2;
uint8_t  CargoStatus_3;

/*
    机械臂取放动作标志位
*/
uint8_t cargo_flag=0;

/*
    机械臂取放动作参数
*/
ActionParameter Cargo1_ActionDown = {140, 65, 208, 1};    //1号货架放下动作参数
ActionParameter Cargo1_ActionUp = {155, 28, 208, 1};      //1号货架抬起动作参数,下同

ActionParameter Cargo2_ActionDown = {148, 61, 234, 2};
ActionParameter Cargo2_ActionUp = {155, 27, 234, 2};

ActionParameter Cargo3_ActionDown = {140, 70, 260, 3};
ActionParameter Cargo3_ActionUp = {155, 28, 260, 3};

ActionParameter Cargo1_FetchDown = {145, 58, 208, 1};
ActionParameter Cargo1_FetchUp = {145, 35, 208, 1};

ActionParameter Cargo2_FetchDown = {155, 51, 234, 2};
ActionParameter Cargo2_FetchUp = {155, 27, 234, 2};

ActionParameter Cargo3_FetchDown = {148, 56, 262, 3};
ActionParameter Cargo3_FetchUp = {148, 32, 262, 3};

/*
    自由位置（过渡位置）freeAngle
*/
ActionParameter freeAngle = {120, 65, 155, 0};
/*
    巡线位置lineAngle：

    云台角度：56~58
    rightarm ：125
    leftarm：85

*/
ActionParameter lineAngle = {125, 85, 57, 0};
/*
    找圆位置circleAngle
*/
ActionParameter circleAngle = {125, 65, 57, 0};
/*
    扫码位置scanCodeAngle
*/
ActionParameter scanCodeAngle = {};


/*

    初始化
    一般使用
    pwm输出开启



*/
void Servo_Init(void)
{
    ActionFunc (circleAngle);

    CargoSet (CARGO_1, CARGO_SET);
    CargoSet(2, 1);
    CargoSet(3, 1);

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
        //NowAngle_Holder = angle;
    }
    else
        return;
}

void PawControl(uint8_t angle)
{
    if(angle<=180)
    {
        Servo4_Angle = (uint32_t)((angle*1.0/180.0)*PAW_FULL_ANGLE)+ZERO_ANGLE;
        //NowAngle_Paw = angle;
    }
    else
        return;
}


void LeftArmControl(uint8_t LeftAngle)
{
    if(LeftAngle<=180)
    {
        Servo6_Angle = (uint32_t)((LeftAngle*1.0/180.0)*ARM_FULL_ANGLE)+ZERO_ANGLE;

        //NowAngle_LeftArm = LeftAngle;
    }
    else
        return;
}

void RightArmControl (uint8_t RightAngle)
{
    if (RightAngle<=180)
    {
        Servo7_Angle = (uint32_t)((RightAngle*1.0/180.0)*ARM_FULL_ANGLE)+ZERO_ANGLE;

        //NowAngle_RightArm = RightAngle;
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

/*
    通用型动作函数， 只运动左右大臂和云台
*/
void ActionFunc(ActionParameter angle)
{
    RightArmControl (angle.rightArm);
    LeftArmControl (angle.leftArm);
    HolderControl (angle.holder);
}



/*
    物块放置动作函数
    物块放到载物台上的动作
*/
void CargoAction (ActionParameter up, ActionParameter down)
{
    //回归自由度数
    ActionFunc(freeAngle);

    HAL_Delay (1000);

    //调整合适的移动角度，打开载物台
    if (up.cargoNo == 1)
    {
        RightArmControl (up.rightArm);
        LeftArmControl (up.leftArm);
    }
    else if (up.cargoNo == 2 || up.cargoNo == 3)
    {
        RightArmControl (125);
        LeftArmControl  (35);
    }

    CargoSet (up.cargoNo, 0);

    HAL_Delay (600);

    //移动到载物台上方
    HolderControl (up.holder);
    HAL_Delay (400);


    // RightArmControl (160);
    // LeftArmControl (25);

    HAL_Delay (400);

    //放下物块
    RightArmControl (down.rightArm);
    LeftArmControl (down.leftArm);
    
    HAL_Delay (600);

    PawControl (125);

    HAL_Delay  (400);

    //抬起机械臂
    LeftArmControl (up.leftArm);
    RightArmControl (up.rightArm);

    HAL_Delay (600);

    // LeftArmControl (35);
    // RightArmControl (140);
    HAL_Delay (500);

    //载物台关闭，移开机械臂
    CargoSet (down.cargoNo, 1);
    HolderControl (180);

    HAL_Delay (500);
    PawControl (100);

    //回归自由角度
    ActionFunc(freeAngle);
}

/*
    从载物台拿物块动作
*/
void CargoFetch (ActionParameter up, ActionParameter down)
{
    ActionFunc(freeAngle);

    HAL_Delay (800);

    LeftArmControl  (up.leftArm);

    HAL_Delay (600);

    RightArmControl (up.rightArm);

    HAL_Delay (800);

    //转动到载物台
    HolderControl (up.holder);
    PawControl (120);
    CargoSet (up.cargoNo, 0);

    HAL_Delay (800);

    LeftArmControl (down.leftArm);

    HAL_Delay (500);

    PawControl (90);

    HAL_Delay (300);

    //调整移出角度
    RightArmControl (125);
    LeftArmControl (35);

    HAL_Delay (600);
    CargoSet (up.cargoNo, 1);

    HolderControl (180);

    HAL_Delay (600);
    ActionFunc(freeAngle);

}


//原料区拿起放到车上
void PickCargo_Yuanliao (uint8_t color)
{
    //这里应该有一个拿起的函数
    switch (color)
    {
    case 1:
        CargoAction (Cargo1_ActionUp, Cargo1_ActionDown);
        break;
    case 2:
        CargoAction (Cargo2_ActionUp, Cargo2_ActionDown);
        break;
    case 3:
        CargoAction (Cargo3_ActionUp, Cargo3_ActionDown);
        break;
    }
}

void PickCargo_Ground(uint8_t color)
{
    return ;
    
}

