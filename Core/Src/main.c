/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dcmi.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// #include "dcmi_ov5640.h"
#include "usart_dma.h"
#include <string.h>
#include "stdio.h"
#include "camprocess.h"
#include "sys.h"
#include "motor.h"
#include "ov5640test.h"
#include "Display.h"
#include "esp8266.h"
//#include "JY60.h"
#include "openmv.h"
#include "test.h"

const uint8_t testLen = 7;
uint8_t testBuffer[testLen];

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint8_t round1;
extern uint8_t round2;
extern uint8_t round3;
extern uint8_t round4;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DCMI_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM6_Init();
  MX_TIM8_Init();
  MX_TIM12_Init();
  MX_TIM14_Init();
  MX_UART7_Init();
  MX_UART8_Init();
  MX_USART6_UART_Init();
  MX_UART4_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

    Motor_Init();

    // testInit  ();

    Servo_Init();

    DisPlay_Init();

    HAL_UART_Receive_IT(&huart1, testBuffer, testLen);
    
    // Assignment[0] = 2;
    // Assignment[1] = 1;
    // Assignment[2] = 3;

    // HAL_Delay (6000);

    openmv_Init();
    ToStartAngle();

    // procedure = 57;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    // target = 200;

    while (1)
    {
        if (CLEARFLAG == 1 && IF_LINE == 1)
        {
            Move_Stop();
            CLEARFLAG = 0;
        }

        if (IF_MOVE == 1 && IF_LINE == 1) // IF_MOVE == 1 并且 �???始循�??? 前进
        {
            Move_Forward();
        }

        if (Pick_ins == 1)
        {
            PickCargo_Yuanliao(Swuliao[2]);
            Pick_ins = 0;
        }

        // �??下全是cargoset
        {
            if (cargo_flag == 1)
            {
                CargoAction(Cargo1_ActionUp, Cargo1_ActionDown);
                cargo_flag = 0;
            }
            else if (cargo_flag == 2)
            {
                CargoAction(Cargo2_ActionUp, Cargo2_ActionDown);
                cargo_flag = 0;
            }
            else if (cargo_flag == 3)
            {
                CargoAction(Cargo3_ActionUp, Cargo3_ActionDown);
                cargo_flag = 0;
            }
            else if (cargo_flag == 4)
            {
                CargoFetch(Cargo1_FetchUp, Cargo1_FetchDown);
                cargo_flag = 0;
            }
            else if (cargo_flag == 5)
            {
                CargoFetch(Cargo2_FetchUp, Cargo2_FetchDown);
                cargo_flag = 0;
            }
            else if (cargo_flag == 6)
            {
                CargoFetch(Cargo3_FetchUp, Cargo3_FetchDown);
                cargo_flag = 0;

            }

            else if (cargo_flag == 10)
            {

                boardcolor[3] = 1;
                CargoFetch(Cargo1_FetchUp, Cargo1_FetchDown);
                DisposeCargo_Ground(Calculate_angle);
                HAL_Delay(3000);
                PickCargo_Ground(Calculate_angle);
                // CargoAction (Cargo1_ActionUp, Cargo1_ActionDown);
                farcircleagnle.holder = 23 + 4;
                ActionFunc(farcircleagnle);
                HAL_Delay(1000);
                HAL_UART_Transmit(&huart2, Flocation, 7, 0xff);

                cargo_flag = 0;
            }
            // else if (cargo_flag == 11)
            // {
            //     ToStartAngle();
            //     // HAL_Delay(1000);
            //     // StartAction ();

            //     // ActionFunc (waitAngle);
                
            //     cargo_flag = 0;

            // }
        //    else if (cargo_flag == 13) 
        //    {
        //        StartAction();
        //        ActionFunc(lineAngle);
        //        cargo_flag = 0;
        //        procedure = 12;
        //    }
        }

        Procedure_Setting (procedure);
        // printCnt();
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  for (volatile int i = 0; i<5000000; i++);
    // test_init();


    procedure = 1;
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
}







// 系统定时器中�??????????????????????
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(htim);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_TIM_PeriodElapsedCallback could be implemented in the user file
     */
    if (htim == &htim2)
    {
        round1++;
        // printf("\nround1 ++ %d\n", round1);
        HAL_TIM_Base_Start_IT(&htim2);
    }
    if (htim == &htim3)
    {
        round2++;
        // printf ("round2 ++ %d\n",round2);
        HAL_TIM_Base_Start_IT(&htim3);
    }
    if (htim == &htim4)
    {
        round3++;
        HAL_TIM_Base_Start_IT(&htim4);
        // printf ("round3 ++ %d\n",round3);
    }
    if (htim == &htim8)
    {
        round4++;
        // printf ("round4 ++ %d\n",round4);
        HAL_TIM_Base_Start_IT(&htim8);
    }
    // if (htim == &htim6)
    // {
    //     //printf ("%f", ang1.Yaw);
    //     if (IF_MOVE == 1)
    //     {
    //         Move_Forward();

    //         //这里要改
    //         // Lateral_correction();
    //     }

    //     //写完成条�??????????????????????
    //     if (IF_MOVE == 0)
    //     {
    //         procedure++;
    //     }
    // }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // if (huart == &huart5)
    // {
    //     printf ("%f\n%f\n%f\n\n", ang1.Pitch,ang1.Roll, ang1.Yaw);
    //     JY60_Message_Pross(DMARecieveBuffer_JY60, &acce1, &angv1, &ang1);
    //     //HAL_UART_Receive_DMA(&huart5, DMARecieveBuffer_JY60, 11);
    // }
    if (huart == &huart2)
    {
        MV_DataProcess1(openmv1);
    }
    if (huart == &huart3)
    {
        MV_DataProcess2(openmv2);
    }
    // if (huart == &huart1)
    // {
    //     int a = -15;        // 增大时向�??
    //     float y0 = 125 - 80;
    //     float x0 = 86.03 + 118.20 - 80;

    //     double p = x0 + (double)testBuffer[0] * testBuffer[1] / 1.4 + a;
    //     double k = acos((sqrt(p * p + (y0) * (y0)) / 2) / 150);
    //     double m = atan(p / (y0));
    //     double n = (PI - k - m) / PI * 180; //
    //     double l = (PI / 2 - m + k) / PI * 180;

    //     Calculate_angle.leftArm = l + 35;
    //     Calculate_angle.rightArm = 140 - n;
    //     Calculate_angle.cargoNo = 0;
    //     Calculate_angle.holder = NowAngle_Holder;

    //     // 测试用记得删�??
    //     ActionFunc (Calculate_angle);

    //     // DisPlay_Porcess (testBuffer);
    //     //HAL_UART_Transmit(&huart2, testBuffer, testLen, 0x00ff);
    //     HAL_UART_Receive_IT(&huart1, testBuffer, testLen);
    // }
    // if (huart == &huart1)
    // {
    //     MV_message_check (openmv1, &huart1);
    // }
}

// 空闲中断处理函数
void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
{
    if (RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) // 判断idle标志被置�??????????????????
    {
        __HAL_UART_CLEAR_IDLEFLAG(huart); // 清除标志
        HAL_UART_DMAStop(huart);          // 停止DMA传输
        // 处理中断标志�????????????????

        if (huart == &huart4)
        {
            DisPlay_Porcess(Display_Buffer);
            HAL_UART_Receive_DMA(&huart4, Display_Buffer, 64);
        }
        //    if (huart == &huart7)
        //    {
        //        HAL_UART_Receive_DMA(&huart7, 0x24000000,64);
        //    }
        //    if (huart == &huart8)
        //    {

        //    }
        //        if (huart == &huart5)
        //        {
        //            JY60_Message_Pross(DMARecieveBuffer_JY60, &acce1, &angv1, &ang1);
        //            HAL_UART_Receive_DMA(&huart5, DMARecieveBuffer_JY60, 15);
        //        }
        //        WifiProcess(huart);
    }
}

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_HFNMI_PRIVDEF);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
