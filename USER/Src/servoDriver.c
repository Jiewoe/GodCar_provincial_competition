#include "servoDriver.h"
#include "math.h"

/*

    定义全局变量记录舵机当前角度

    载物台舵机1，2，3 分别转动
    机械臂舵机6，7    分别转动

*/
// uint8_t  NowAngle_Cargo;
// uint8_t  NowAngle_Paw;
uint8_t NowAngle_LeftArm;
uint8_t NowAngle_RightArm;
float NowAngle_Holder;

uint8_t CargoStatus_1;
uint8_t CargoStatus_2;
uint8_t CargoStatus_3;

/*
    机械臂取放动作标志位
*/
uint8_t cargo_flag = 0;

/*
    机械臂取放动作参数
*/
ActionParameter Cargo1_ActionDown = {146, 61, 207, 1}; // 1号货架放下动作参数
ActionParameter Cargo1_ActionUp = {160, 16, 207, 1};   // 1号货架抬起动作参数,下同

ActionParameter Cargo2_ActionDown = {158, 55, 234, 2};
ActionParameter Cargo2_ActionUp = {163, 20, 234, 2};

ActionParameter Cargo3_ActionDown = {148, 60, 260, 3};
ActionParameter Cargo3_ActionUp = {155, 24, 260, 3};

ActionParameter Cargo1_FetchDown = {146, 55, 207, 1};
ActionParameter Cargo1_FetchUp = {160, 16, 207, 1};

ActionParameter Cargo2_FetchDown = {158, 55, 234, 2};
ActionParameter Cargo2_FetchUp = {163, 20, 234, 2};

ActionParameter Cargo3_FetchDown = {145, 55, 260, 3};
ActionParameter Cargo3_FetchUp = {168, 20, 260, 3};

/*
    地面物料拿取、放置
*/
ActionParameter Board1_GroundDown = {65, 120, 23, 1};
ActionParameter Board3_GroundDown = {55, 118, 87, 3};
ActionParameter Board2_GroundDown = {52, 142, 54, 2};

ActionParameter Calculate_angle;


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
ActionParameter farcircleagnle = {80, 50, 58, 0};

/*
    扫码位置scanCodeAngle
*/
ActionParameter scanCodeAngle = {};

/*
    圆盘位置识别角度
*/
ActionParameter stageangle = {135, 40, 54, 0};

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
ActionParameter waitAngle = {135, 115, 54, 0};

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
    ActionFunc(circleAngle);

    CargoSet(CARGO_1, CARGO_SET);
    CargoSet(2, 1);
    CargoSet(3, 1);

    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
}

/*

    控制函数:
    >> 检测角度是否正确
    >> 更改占空比
    >> 更新角度nowAngle

*/
void HolderSet(float angle)
{
    Servo5_Angle = (uint32_t)((angle * 1.0 / 270.0) * HOLDER_FULL_ANGLE) + ZERO_ANGLE;
}

void HolderControl(float angle)
{
    int speed = 300000;
    if (angle <= 270)
    {
        if (NowAngle_Holder > angle + 1)
        {
            for (float angle_temp = NowAngle_Holder; angle_temp >= angle; angle_temp--)
            {
                HolderSet(angle_temp);
                for (volatile int i = 0; i < speed; i++)
                    ;
            }
        }
        else if (NowAngle_Holder < angle - 1)
        {
            for (float angle_temp = NowAngle_Holder; angle_temp <= angle; angle_temp++)
            {
                HolderSet(angle_temp);
                for (volatile int i = 0; i < speed; i++)
                    ;
            }
        }

        // Servo5_Angle = (uint32_t)((angle * 1.0 / 270.0) * HOLDER_FULL_ANGLE) + ZERO_ANGLE;

        NowAngle_Holder = angle;
        HolderSet(NowAngle_Holder);
        return;
    }
    else
        return;
}

void PawControl(uint8_t angle)
{
    if (angle <= 180)
    {
        Servo4_Angle = (uint32_t)((angle * 1.0 / 180.0) * PAW_FULL_ANGLE) + ZERO_ANGLE;
        // NowAngle_Paw = angle;
    }
    else
        return;
}

void LeftArmControl(uint8_t LeftAngle)
{
    int speed = 300000;
    if (LeftAngle <= 180)
    {
        if (NowAngle_LeftArm > LeftAngle)
        {
            for (uint16_t angle_temp = NowAngle_LeftArm; angle_temp >= LeftAngle; angle_temp--)
            {
                for (volatile int i = 0; i < speed; i++)
                    ;
                Servo6_Angle = (uint32_t)((angle_temp * 1.0 / 180.0) * ARM_FULL_ANGLE) + ZERO_ANGLE;
            }
        }
        else
        {
            for (uint16_t angle_temp = NowAngle_LeftArm; angle_temp <= LeftAngle; angle_temp++)
            {
                for (volatile int i = 0; i < speed; i++)
                    ;
                Servo6_Angle = (uint32_t)((angle_temp * 1.0 / 180.0) * ARM_FULL_ANGLE) + ZERO_ANGLE;
            }
        }
        // Servo6_Angle = (uint32_t)((LeftAngle * 1.0 / 180.0) * ARM_FULL_ANGLE) + ZERO_ANGLE;

        NowAngle_LeftArm = LeftAngle;
    }
    else
        return;
}

void RightArmControl(uint8_t RightAngle)
{
    int speed = 300000;
    if (RightAngle <= 180)
    {
        if (NowAngle_RightArm > RightAngle)
        {
            for (uint16_t angle_temp = NowAngle_RightArm; angle_temp >= RightAngle; angle_temp--)
            {
                for (volatile int i = 0; i < speed; i++)
                    ;
                Servo7_Angle = (uint32_t)((angle_temp * 1.0 / 180.0) * ARM_FULL_ANGLE) + ZERO_ANGLE;
            }
        }
        else
        {
            for (uint16_t angle_temp = NowAngle_RightArm; angle_temp <= RightAngle; angle_temp++)
            {
                for (volatile int i = 0; i < speed; i++)
                    ;
                Servo7_Angle = (uint32_t)((angle_temp * 1.0 / 180.0) * ARM_FULL_ANGLE) + ZERO_ANGLE;
            }
        }
        // Servo7_Angle = (uint32_t)((RightAngle * 1.0 / 180.0) * ARM_FULL_ANGLE) + ZERO_ANGLE;

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
                Servo1_Angle = (uint32_t)(CARGO_FULL_ANGLE + ZERO_ANGLE - 100);
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
                Servo2_Angle = (uint32_t)(CARGO_FULL_ANGLE + ZERO_ANGLE);
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
                Servo3_Angle = (uint32_t)(CARGO_FULL_ANGLE + ZERO_ANGLE - 50);
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
    HolderControl(angle.holder);
    RightArmControl(angle.rightArm);
    LeftArmControl(angle.leftArm);
}

/*
    物块放置动作函数
    物块放到载物台上的动作
*/
void CargoAction(ActionParameter up, ActionParameter down)
{
    // 回归自由度数
    ActionFunc(freeAngle);
    // 这里要等

    HAL_Delay(1000);

    // 调整合适的移动角度，打开载物台
    if (up.cargoNo == 1)
    {
        RightArmControl(up.rightArm);
        LeftArmControl(up.leftArm);
    }
    else if (up.cargoNo == 2 || up.cargoNo == 3)
    {
        RightArmControl(120);
        LeftArmControl(26);
    }

    CargoSet(up.cargoNo, 0);

    // HAL_Delay(600);

    // 移动到载物台上方
    HolderControl(up.holder);
    HAL_Delay(400);

    // RightArmControl (160);
    // LeftArmControl (25);

    HAL_Delay(400);

    // 放下物块
    RightArmControl(down.rightArm);
    LeftArmControl(down.leftArm);

    HAL_Delay(600);

    PawControl(115);

    HAL_Delay(400);

    // 抬起机械臂
    LeftArmControl(up.leftArm);
    RightArmControl(up.rightArm);

    // HAL_Delay(600);

    // LeftArmControl (35);
    // RightArmControl (140);
    // HAL_Delay(500);

    // 载物台关闭，移开机械臂
    CargoSet(down.cargoNo, 1);
    HolderControl(180);

    // HAL_Delay(500);
    PawControl(100);

    // 回归自由角度
    ActionFunc(freeAngle);
}

/*
    从载物台拿物块动作
*/
void CargoFetch(ActionParameter up, ActionParameter down)
{
    ActionFunc(freeAngle);

    // HAL_Delay(400);

    LeftArmControl(up.leftArm);

    // HAL_Delay(600);

    RightArmControl(up.rightArm);

    // HAL_Delay(800);

    // 转动到载物台
    CargoSet(up.cargoNo, 0);
    HolderControl(up.holder);
    PawControl(115);
    // CargoSet(up.cargoNo, 0);

    HAL_Delay(500);

    LeftArmControl(down.leftArm);
    RightArmControl(down.rightArm);

    HAL_Delay(500);

    PawControl(80);

    HAL_Delay(500);

    // 调整移出角度
    RightArmControl(120);
    LeftArmControl(26);

    // HAL_Delay(600);
    CargoSet(up.cargoNo, 1);

    HolderControl(180);

    // HAL_Delay(600);
    ActionFunc(freeAngle);
}

/*
    1 red
    2 green
    3 blue

*/
void PickCargo_Yuanliao(uint8_t color)
{
    ActionFunc(materialAngle);
    HAL_Delay(600);
    PawControl(85);
    HAL_Delay(500);
    ActionFunc(stageangle);
    HAL_Delay(1000);
    switch (color)
    {
    case 1:
        CargoAction(Cargo1_ActionUp, Cargo1_ActionDown);
        break;
    case 2:
        CargoAction(Cargo2_ActionUp, Cargo2_ActionDown);
        break;
    case 3:
        CargoAction(Cargo3_ActionUp, Cargo3_ActionDown);
        break;
    }
    PawControl(170);
    procedure++;
}

/*
    拿起地上的物料函数
*/
void PickCargo_Ground(ActionParameter down)
{
    // 找圆角度
    ActionFunc(circleAngle);

    HAL_Delay(1000);

    // 转动舵机
    // 红色物料区域不需要转动云台

    HolderControl(down.holder);
    HAL_Delay(600);

    // 打开抓夹，回缩大臂
    PawControl(120);
    LeftArmControl(down.leftArm);

    HAL_Delay(700);

    // 放下大臂
    RightArmControl(down.rightArm);

    HAL_Delay(600);

    // 抓取
    PawControl(80);

    HAL_Delay(500);

    LeftArmControl(118);  // 118
    RightArmControl(100); // 100

    HAL_Delay(600);

    // 放置到载物台
    switch (boardcolor[colorflag])
    {
    case 1: // red
        CargoAction(Cargo1_ActionUp, Cargo1_ActionDown);
        break;
    case 2:
        CargoAction(Cargo2_ActionUp, Cargo2_ActionDown);
        break;
    case 3:
        CargoAction(Cargo3_ActionUp, Cargo3_ActionDown);
        break;
    }

    HAL_Delay(800);
}

void DisposeCargo_Ground(ActionParameter down)
{
    ActionFunc(freeAngle);

    HAL_Delay(1000);

    // 移动到放置点上方
    HolderControl(down.holder);

    // HAL_Delay(800);

    // 放下物块
    RightArmControl((down.rightArm + circleAngle.rightArm) / 2);
    LeftArmControl((down.leftArm + circleAngle.leftArm) / 2);
    RightArmControl(down.rightArm);
    LeftArmControl(down.leftArm);

    HAL_Delay(1000);

    // 打开抓夹
    PawControl(120);

    // HAL_Delay(400);

    if (colorflag == 1 || colorflag == 4)
    {
        LeftArmControl(151);
        RightArmControl(80);
        HAL_Delay(600);
    }

    // 先上抬机械臂
    if (procedure < 56)
    {
        RightArmControl(100); // 100
        LeftArmControl(118);  // 118
    }
    else 
    {
        RightArmControl (140);
        LeftArmControl (75);

        HolderControl (135);
    }
    // HAL_Delay(600);

    // 回到找圆的姿势
    RightArmControl(circleAngle.rightArm);
    LeftArmControl(circleAngle.leftArm);
    PawControl(90);

    // HAL_Delay(500);

    // 云台回归
    // HolderControl(down.holder);

    // HAL_Delay(800);
}

/*
    抓取逻辑
*/
void PickCargo_Logic(void)
{
    for (uint8_t i = 1; i < 4; i++)
    {
        switch (i)
        {
        case 1:
            PickCargo_Ground(Board1_GroundDown);
            break;
        case 2:
            PickCargo_Ground(Board2_GroundDown);
            break;
        case 3:
            PickCargo_Ground(Board3_GroundDown);
            break;
        }

        colorflag++;
        // PickCargo_Ground()
    }
}

/*
    放置逻辑
    选取合适颜色的物块进行方式
    取出boardcolor中的颜色信息
*/
void DisposeCargo_Logic(void)
{
    PawControl(180);
    switch (boardcolor[colorflag])
    {
    case 1:
        CargoFetch(Cargo1_FetchUp, Cargo1_FetchDown);
        break;
    case 2:
        CargoFetch(Cargo2_FetchUp, Cargo2_FetchDown);
        break;
    case 3:
        CargoFetch(Cargo3_FetchUp, Cargo3_FetchDown);
        break;
    }
    colorflag++;

    DisposeCargo_Ground(Calculate_angle);
}

/*
    进行逆结算并且将角度存在对应的boardangle里面
    方便拿起来时候用
    但是要进行调整
    procedure++;
*/
void Servo_process(uint8_t zhengfu, uint8_t yuntai, uint8_t jixiebifuhao, uint8_t jixiebipiancha)
{
    // 复数先向前特定度数消灭回程查
    if (zhengfu == 1 && yuntai > 3)
    {
        // 这里不精确
        HolderControl(NowAngle_Holder + 10);
        HAL_UART_Transmit(&huart2, Flocation, 7, 0xff);
        return;
    }
    // 云台大于3就要调整，小于不用
    if ((yuntai > 5))
    {
        NowAngle_Holder = NowAngle_Holder - 1;
        HolderSet(NowAngle_Holder);
        // for (volatile int i; i<100000; i++);
        HAL_UART_Transmit(&huart2, Flocation, 7, 0xff);
        return;
    }else if ((yuntai > 3))
    {
        NowAngle_Holder = NowAngle_Holder - 0.5;
        HolderSet(NowAngle_Holder);
        // for (volatile int i; i<100000; i++);
        HAL_UART_Transmit(&huart2, Flocation, 7, 0xff);
        return;
    }
    else
    {

        int fuhao;
        if (jixiebifuhao == 1)
        {
            fuhao = 1;
        }
        else
        {
            fuhao = -1;
        }
        /*
            a由于openmv带来的倾斜导致的x坐标偏差
            jixiebipiancha 是openmv测出的像素点
            p是x距离
            k是等腰三角形的地角
            m是三角形到Y轴的角
            n是右臂角度（角度制）
            l是左臂角度（角度制）
            除数可以改变 计算比例用s


        */
        float y0;
        if (procedure < 56)
        {
            y0 = 125 - 5; // 减小是变高
        }
        else 
        {
            y0 = 125 - 77;
        }

        int a = -15;        // 增大时向前
        
        
        
        float bili;
        float x0;

        if (colorflag != 1 || colorflag != 4)
        {
            //这里要改 调参数
            bili = 1.4;
            x0 = 274.79-80;
        }
        else
        {
            bili = 1.4;
            x0 = 86.03 + 118.20 - 80;
        }

        double p = x0 + (double)jixiebipiancha * fuhao /bili + a;
        double k = acos((sqrt(p * p + (y0) * (y0)) / 2) / 150);
        double m = atan(p / (y0));
        double n = (PI - k - m) / PI * 180; //
        double l = (PI / 2 - m + k) / PI * 180;

        Calculate_angle.leftArm = l + 35;
        Calculate_angle.rightArm = 140 - n;
        Calculate_angle.cargoNo = 0;
        // Calculate_angle.holder = NowAngle_Holder-1.7;


        // 测试用记得删除
        // colorflag = 3;


        if (colorflag == 1 || colorflag == 4)
        {
            Calculate_angle.holder = NowAngle_Holder;
        }
        else
        {
            Calculate_angle.holder = NowAngle_Holder;
        }

        // 这里要改拿不起来
        switch (colorflag)
        {
        case 0:
        case 3:
            Board1_GroundDown = Calculate_angle;
            break;
        case 1:
        case 4:
            Board2_GroundDown = Calculate_angle;
            break;
        case 2:
        case 5:
            Board3_GroundDown = Calculate_angle;
            break;
        }
        HAL_UART_Transmit(&huart2, Stop, 7, 0xff);

        // cargo_flag = 10;

        procedure++;
    }
}
