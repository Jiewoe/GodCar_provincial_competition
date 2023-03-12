#include "servoDriver.h"

/*

    定义全局变量记录舵机当前角度

    载物台舵机1，2，3 分别转动
    机械臂舵机6，7    分别转动

*/
// uint8_t  NowAngle_Cargo;
// uint8_t  NowAngle_Paw;
uint8_t  NowAngle_LeftArm;
uint8_t  NowAngle_RightArm;
uint16_t NowAngle_Holder;

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
ActionParameter Cargo1_ActionDown = {147, 52, 210, 1};    //1号货架放下动作参数
ActionParameter Cargo1_ActionUp = {155, 24, 210 , 1};      //1号货架抬起动作参数,下同

ActionParameter Cargo2_ActionDown = {155, 48, 237, 2};
ActionParameter Cargo2_ActionUp = {155, 24, 237, 2};

ActionParameter Cargo3_ActionDown = {145, 65, 267, 3};
ActionParameter Cargo3_ActionUp = {155, 24, 267, 3};

ActionParameter Cargo1_FetchDown = {150, 47, 208, 1};
ActionParameter Cargo1_FetchUp = {153, 25, 208, 1};

ActionParameter Cargo2_FetchDown = {165, 39, 235, 2};
ActionParameter Cargo2_FetchUp = {163, 20, 235, 2};

ActionParameter Cargo3_FetchDown = {155, 48, 266, 3};
ActionParameter Cargo3_FetchUp = {155, 26, 266, 3};

/*
    地面物料拿取、放置
*/
ActionParameter CargoRed_GroundDown =   {65, 120, 23, 1};
ActionParameter CargoBlue_GroundDown =  {55, 118, 87, 3};
ActionParameter CargoGreen_GroundDown = {52, 142, 54, 2};

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
ActionParameter lineAngle = {120, 65, 58, 0};
/*
    找圆位置circleAngle
    108,74,58,0
    95,55,58,0

*/
ActionParameter circleAngle = {95, 55, 58, 0};

/*
    扫码位置scanCodeAngle
*/
ActionParameter scanCodeAngle = {};

/*
    圆盘位置识别角度
*/
ActionParameter stageangle = {135,46, 58, 0};

/*
    圆盘抓取角度
*/
ActionParameter materialAngle = {97, 112, 58, 0};

/*
    特殊找线角度
*/
ActionParameter specialLineAngle = {107, 56, 58, 0};
/*
    等待抓取物块角度

    水平等待角度 right 135， left 115
    垂直等待角度 right 142  left  55
*/
ActionParameter waitAngle = {135, 115, 58, 0};

/*

    初始化
    一般使用
    pwm输出开启



*/
void Servo_Init(void)
{
    NowAngle_Holder = circleAngle.holder;
    NowAngle_RightArm = circleAngle.rightArm;
    NowAngle_LeftArm = circleAngle.leftArm;
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
void HolderSet(uint16_t angle)
{
    Servo5_Angle = (uint32_t)((angle*1.0/270.0)*HOLDER_FULL_ANGLE)+ZERO_ANGLE;
}

void HolderControl(uint16_t angle)
{
    uint8_t speed = 5;
    if(angle<=270)
    {
        // if (NowAngle_Holder > angle)
        // {
        //     for (uint16_t angle_temp=NowAngle_Holder;angle_temp>=angle; angle_temp--)
        //     {
        //         HolderSet (angle_temp);
        //         HAL_Delay (speed);
        //     }
        // }
        // else 
        // {
        //     for (uint16_t angle_temp=angle;angle_temp<=NowAngle_Holder; angle_temp++)
        //     {
        //         HolderSet (angle_temp);
        //         HAL_Delay (speed);
        //     }
        // }
        
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
        // NowAngle_Paw = angle;
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
                    Servo1_Angle = (uint32_t)(CARGO_FULL_ANGLE+ZERO_ANGLE);
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
                    Servo3_Angle = (uint32_t)(CARGO_FULL_ANGLE+ZERO_ANGLE-40);
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
        LeftArmControl  (32);
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

    PawControl (115);

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
    PawControl (115);
    CargoSet (up.cargoNo, 0);

    HAL_Delay (800);

    LeftArmControl (down.leftArm);
    RightArmControl (down.rightArm);

    HAL_Delay (500);

    PawControl (80);

    HAL_Delay (300);

    //调整移出角度
    RightArmControl (125);
    LeftArmControl (30);

    HAL_Delay (600);
    CargoSet (up.cargoNo, 1);

    HolderControl (180);

    HAL_Delay (600);
    ActionFunc(freeAngle);

}

/*
    1 red
    2 green
    3 blue

*/
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

/*
    拿起地上的物料函数
*/
void PickCargo_Ground(ActionParameter down)
{
    //找圆角度
    ActionFunc (circleAngle);

    HAL_Delay (1000);

    //转动舵机
    if (down.cargoNo != 1)  //红色物料区域不需要转动云台
    {
        HolderControl (down.holder);
        HAL_Delay (600);
    }

    //打开抓夹，回缩大臂
    PawControl (120);
    LeftArmControl (down.leftArm);

    HAL_Delay (700);

    //放下大臂
    RightArmControl (down.rightArm);
    
    HAL_Delay (600);

    //抓取
    PawControl (80);

    HAL_Delay (500);

    LeftArmControl (118);   //118
    RightArmControl (100); //100

    HAL_Delay (600);

    //放置到载物台
    switch (down.cargoNo)
    {
        case 1: //red
            CargoAction (Cargo1_ActionUp, Cargo1_ActionDown);
            break;
        case 2:
            CargoAction (Cargo2_ActionUp, Cargo2_ActionDown);
            break;
        case 3:
            CargoAction (Cargo3_ActionUp, Cargo3_ActionDown);
            break;
    }

    HAL_Delay (800);
}

void DisposeCargo_Ground(ActionParameter down)
{
    ActionFunc (freeAngle);

    //从载物台上拿到物块
    switch (down.cargoNo)
    {
        case 1:
            CargoFetch (Cargo1_FetchUp, Cargo1_FetchDown);
            break;
        case 2:
            CargoFetch (Cargo2_FetchUp, Cargo2_FetchDown);
            break;
        case 3:
            CargoFetch (Cargo3_FetchUp, Cargo3_FetchDown);
            break;
    }

    HAL_Delay (1200);
    
    //移动到放置点上方
    HolderControl (down.holder);

    HAL_Delay (800);

    //放下物块
    RightArmControl (down.rightArm);
    LeftArmControl (down.leftArm);

    HAL_Delay (1500);

    //打开抓夹
    PawControl (120);

    HAL_Delay (400);

    if (down.cargoNo == 2)
    {
        LeftArmControl (151);
        RightArmControl (80);
        HAL_Delay (600);
    }

    //先上抬机械臂
    LeftArmControl (118);   //118
    RightArmControl (100); //100

    HAL_Delay (600);

    //回到找圆的姿势
    RightArmControl (circleAngle.rightArm);
    LeftArmControl  (circleAngle.leftArm);
    PawControl (90);

    HAL_Delay (500);

    //云台回归
    HolderControl (down.holder);

    HAL_Delay (800);
}

/*
    抓取逻辑
*/
void PickCargo_Logic(void)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        switch (Assignment[i])
        {
            case 1:
                PickCargo_Ground (CargoRed_GroundDown);
                break;
            case 2:
                PickCargo_Ground (CargoGreen_GroundDown);
                break;
            case 3:
                PickCargo_Ground(CargoBlue_GroundDown);
                break;
        }
    }
}

/*
    放置逻辑
*/
void DisposeCargo_Logic(void)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        switch (Assignment[i])
        {
            case 1:
                DisposeCargo_Ground (CargoRed_GroundDown);
                break;
            case 2:
                DisposeCargo_Ground (CargoGreen_GroundDown);
                break;
            case 3:
                DisposeCargo_Ground(CargoBlue_GroundDown);
                break;
        }
    }
}




