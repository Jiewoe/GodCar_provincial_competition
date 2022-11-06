#include "ov5640test.h"

CamStruct ov5640;

void testInit(void)
{
    DCMI_OV5640_Init();
    OV5640_AF_Download_Firmware();
    OV5640_Set_Vertical_Flip(OV5640_Disable);
    OV5640_Set_Horizontal_Mirror(OV5640_Enable);
    OV5640_Set_Pixformat(Pixformat_GRAY);

    ov5640.BufferAddress = Camera_Buffer;
    ov5640.Cam_Result = Usart_Buffer;
    ov5640.CamWide = Display_Width;
    ov5640.CanHigh = Display_Height;

    OV5640_DMA_Transmit_Continuous(Camera_Buffer, Display_BufferSize);
}

void testFunc(void)
{
    if (OV5640_FrameState == 1)
    {
        if (USART1_TX_State == 1)
        {
            memcpy((uint8_t *)Usart_Buffer, (uint8_t *)Camera_Buffer, Display_Width * Display_Height);

            printf("\n OV5640输出帧率:%d ", OV5640_FPS);
            printf("\n USART输出帧率:%d ", USART_FPS);

            printf("\n image:%d,%d,%d,%d,%d \n",
                   0,
                   Display_Width * Display_Height,
                   Display_Width,
                   Display_Height,
                   24);
            CannyEdgeTest(&ov5640, 1);

            HAL_UART_Transmit_DMA(&huart1, (uint8_t *)Usart_Buffer, Display_Width * Display_Height);
        }
        OV5640_FrameState = 0;
        USART1_TX_State = 0;
    }
}