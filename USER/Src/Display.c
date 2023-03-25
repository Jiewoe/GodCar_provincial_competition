#include "Display.h"

// dma配置与空闲中断在主函数里面

void DisPlay_Init()
{
    ClearBuffer(Display_Buffer);
    __HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);       // 使能IDLE中断
    HAL_UART_Receive_DMA(&huart4, Display_Buffer, 64); // 开启串口DMA接收
}

void DisPlay_Porcess(uint8_t *buffer)
{
    if (buffer[0] != 0xEE)
    {
        return;
    }
    if (buffer[1] == 0xB1 && buffer[2] == 0x11) // 滑块指令与按钮指令
    {
        if (buffer[4] == 0x00) // 0号屏幕，大臂舵机和云台舵机（0x01）控制
        {
            if (buffer[6] == 0x01) // 1号滑块
            {
                HolderControl((uint16_t)(buffer[10] * 256 + buffer[11]));
                printf("\nHolder:%d\n", (uint16_t)(buffer[10] * 256 + buffer[11]));
            }
            if (buffer[6] == 0x02)
            {
                LeftArmControl((uint16_t)(buffer[10] * 256 + buffer[11]));
                printf("\nLeftArm:%d\n", (uint16_t)(buffer[10] * 256 + buffer[11]));
            }
            if (buffer[6] == 0x03)
            {
                RightArmControl((uint16_t)(buffer[10] * 256 + buffer[11]));
                printf("\nRightArm:%d\n", (uint16_t)(buffer[10] * 256 + buffer[11]));
            }
        }

        // 1号屏幕 货架控制
        if (buffer[4] == 0x01)
        {
            uint8_t set = 1;
            if (buffer[6] > 3)
            {
                set = 0;
                buffer[6] = buffer[6] - 3;
            }
            CargoSet(buffer[6], set);
        }

        // 2号屏幕 机械臂动作
        if (buffer[4] == 0x02 && buffer[9] == 00)
        {
            switch (buffer[6])
            {
            case 0x01:
                cargo_flag = 1;
                // ActionFunc (Board2_GroundDown);
                break;
            case 0x02:
                cargo_flag = 2;
                break;
            case 0x03:
                cargo_flag = 3;
                break;
            case 0x04:
                cargo_flag = 4;
                break;
            case 0x05:
                cargo_flag = 5;

                break;
            case 0x06:
                cargo_flag = 6;
                // ActionFunc ()
                break;
            default:
                cargo_flag = 0;
                break;
            }
        }

        // 3号屏幕 抓夹按钮
        if (buffer[4] == 0x03)
        {
            if (buffer[6] == 0x01) // 抓夹打开，角度为120
            {
                PawControl(120);
            }
            if (buffer[6] == 0x02) // 抓夹关闭，角度为90
            {
                PawControl(85);
            }
            if (buffer[6] == 3)
            {
                // procedure = 1;
                StartAction();
            }
        }
        // 4号屏幕
        if (buffer[4] == 0x04)
        {

            if (buffer[6] == 0x01 && buffer[9] == 00)
            {
                char a[]= "hello";
                HAL_UART_Transmit(&huart6, a, 5, 0xff);
                // ActionFunc(waitAngle);
                // Special_ActionFunc(waitAngle);
                // cargo_flag = 10;
                // ActionFunc (Board1_GroundDown);
            }
            else if (buffer[6] == 0x02 && buffer[9] == 00)
            {
                HAL_UART_Transmit(&huart2, Tellcolor, 7, 0xff);
                // cargo_flag = 11;
            }
            else if (buffer[6] == 0x03 && buffer[9] == 00)
            {
                HAL_UART_Transmit(&huart2, SLine, 7, 0xff);
                IF_LINE = 1;
            }

            else if (buffer[6] == 0x04 && buffer[9] == 0)
            {
                IF_CIRCLE = 1;
                ActionFunc(circleAngle);

                HAL_UART_Transmit(&huart2, FCircle, 7, 0x00ff);
            }
            else if (buffer[6] == 0x05 && buffer[9] == 0)
            {
                // IF_LINE = 1;
                // ActionFunc(specialLineAngle);
                ActionFunc(scanCodeAngle);
                HAL_UART_Transmit(&huart2, SCode, 7, 0x00ff);
                // cargo_flag = 11;
            }
            else if (buffer[6] == 0x06 && buffer[9] == 0)
            {
                PawControl(120);
                farcircleagnle.holder = 23;
                ActionFunc(farcircleagnle);
                HAL_UART_Transmit(&huart2, Flocation, 7, 0x00ff);
            }
            else if (buffer[6] == 0x07 && buffer[9] == 0)
            {
                // cargo_flag = 11;
                IF_CIRCLE = 1;
                ActionFunc(stageangle);
                HAL_UART_Transmit(&huart2, Smaterial, 7, 0x00ff);
                // HAL_UART_Transmit (&huart2, FCircle, 7, 0x00ff);
            }
            else if (buffer[6] == 0x08 && buffer[9] == 0)
            {
                cargo_flag = 12;
                HAL_UART_Transmit(&huart2, FCircle, 7, 0x00ff);
            }

            else if (buffer[6] == 0x09 && buffer[9] == 00)
            {
                HAL_UART_Transmit(&huart3, Stop, 7, 0x00ff);
                HAL_UART_Transmit(&huart2, Stop, 7, 0x00ff);
            }
        }
    }
}

void ShowAssignmentCode(uint8_t *openmv)
{
    uint8_t changePage[9] = {0xEE, 0xB1, 00, 00, 05, 0xFF, 0xFC, 0xFF, 0xFF};
    HAL_UART_Transmit(&huart4, changePage, 9, 0x00ff);

    uint8_t AssignmentCode[18] = {0xEE, 0xB1, 0x10, 0x00, 0x05, 00, 01, 00, 00,
                                  00, 0x2B, 00, 00, 00, 0xFF, 0xFC, 0xFF, 0xFF};

    // for (uint8_t i=0; i<6; i=i+2)
    // {
    //     AssignmentCode[7+i] = openmv[i/2+2]/10 + '0';
    //     AssignmentCode[8+i] = openmv[i/2+2]%10 + '0';

    //     Assignment [i] = openmv[i/2+2]/10;
    //     Assignment [i+1] = openmv[i/2+2]%10;
    // }

    Assignment[0] = AssignmentCode[7] = openmv[2] / 10;
    Assignment[1] = AssignmentCode[8] = openmv[2] % 10;
    Assignment[2] = AssignmentCode[9] = openmv[3] / 10;
    AssignmentCode[10] = '+' - '0';
    Assignment[3] = AssignmentCode[11] = openmv[3] % 10;
    Assignment[4] = AssignmentCode[12] = openmv[4] / 10;
    Assignment[5] = AssignmentCode[13] = openmv[4] % 10;

    for (int i = 7; i < 14; i++)
    {
        AssignmentCode[i] += '0';
    }

    HAL_UART_Transmit(&huart4, AssignmentCode, 18, 0x00ff);
}

void showCase(void)
{
    uint8_t changePage[9] = {0xEE, 0xB1, 00, 00, 05, 0xFF, 0xFC, 0xFF, 0xFF};
    HAL_UART_Transmit(&huart4, changePage, 9, 0x00ff);

    if (procedure < 10)
    {
        uint8_t message1[12] = {0xEE, 0xB1, 0x10, 0x00, 0x05, 00, 01, 0x00, 0xFF, 0xFC, 0xFF, 0xFF};

        message1[7] = procedure + '0';

        HAL_UART_Transmit(&huart4, message1, 12, 0x00ff);
    }
    else if (procedure >= 10 && procedure < 20)
    {
        uint8_t message2[13] = {0xEE, 0xB1, 0x10, 0x00, 0x05, 00, 01, 0x00, 0x00, 0xFF, 0xFC, 0xFF, 0xFF};

        message2[7] = (procedure) / 10 + '0';
        message2[8] = (procedure) % 10 + '0';
        HAL_UART_Transmit(&huart4, message2, 13, 0x00ff);
    }
}
