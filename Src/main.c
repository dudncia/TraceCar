/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"
#include "pid.h"
#include "bsp.h"
#include "vofa.h"
#include "controller.h"
#include "gray.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//标志位
uint8_t Controller_ready=0;

float filtered_speed=0.0f;
const float a=0.8;
//基础速度
float BaseSpeed=60.0f;
float pos_error=0.0f;//传感器偏差
/*  实例  */
PID_t A_speed_pid;
PID_t B_speed_pid;
PID_t A_position_pid;
PID_t B_position_pid;
PID_t gray_position_pid;
Motor_t A_left_motor; 
Motor_t B_right_motor;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

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
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
    
    BSP_Init();
    HAL_TIM_Base_Start_IT(&htim1);
    Controller_Init();
    //Gray_SendConfig(1,0);//设置为接收模拟量
    Gray_SendConfig(0,1);//设置为接收数字量
    
    //上电延时，等待灰度传感器初始化 20s
    HAL_Delay(20*1000);
    
    //pwm软启动，防止开始速度过快（调试时应该使用，比赛可以考虑不用）
    
    
    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /*这个if判断里的东西有点多可以封装成函数*/
    
    if(Controller_ready==1){
        Controller_ready=0;
        
        //判断小车是否运行到临界位置 依旧每10ms判断一次
        //可以封装成函数
        if(Motor_GetPosition_m(&A_left_motor)>MaxDistance){
            if(Gray_GetAnalog(0)>Purple_min&&Gray_GetAnalog(0)<Purple_max){//左边的传感器进入终点区域
                static uint8_t begin=0;
                if(begin==0){
                    Controller_Set_Target_Position(A_position_pid.current_value+End_Move);
                    begin=1;
                }
                SetBaseSpeed();
            }
        }
        
        //计算pid并输出pwm ，每10ms触发一次
        gray_Controller_Update_Callback(BaseSpeed,pos_error);
        
        //临时测试：只读编码器计数，不驱动电机（手转轮子看 total 用）
//        Motor_Update_Speed(&A_left_motor);
//        Motor_Update_Speed(&B_right_motor);
        
        //每50ms发送数据
        static int send_sount=0;
        if(++send_sount>=5)
        {
            send_sount=0;
            
            /*以下代码可按需注释掉不需要的部分*/
            
//            //发送传感器偏移量
//            float data_to_send_gray[]={
//                                    gray_position_pid.target,//偏移量的目标值
//                                    gray_position_pid.current_value//偏移量的当前值
//            };
//            VOFA_SendData(data_to_send_gray,2);
            
            
//            //发送 A left 左轮速度
            float data_to_send_speedA[]={
                                    A_speed_pid.target,
                                    A_speed_pid.current_value
            };
            VOFA_SendData(data_to_send_speedA,2);

            
//            //发送 B right 右轮速度
            float data_to_send_speedB[]={
                                    B_speed_pid.target,
                                    B_speed_pid.current_value
            };
            VOFA_SendData(data_to_send_speedB,2);        
            
             //发送传感器数据
             //VOFA_SendData((float*)gray_analog,8);  
                //HAL_UART_Transmit(&huart1,(const uint8_t*)gray_digital,8,100);
                //VOFA_SendData(&pos_error,1);
            
        }
    }
        
    
    //计算偏移量
    if(gray_updated==1){
    //pos_error=Gray_CalcLineErrorAnalog();
    pos_error=Gray_CalcLineError();    
    gray_updated=0;    
    }


    
      
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//每10ms进行更新中断
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM1)
    {
        //置标志位表示进行PID计算
        Controller_ready=1;
    }
}

//每接受到一个字节就处理数据
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==USART2)
    {
       Gray_UartRxCallback();
    }
}

/* USER CODE END 4 */

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
#ifdef USE_FULL_ASSERT
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
