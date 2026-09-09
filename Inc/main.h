/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define E2A_Moter_Pin GPIO_PIN_0
#define E2A_Moter_GPIO_Port GPIOA
#define E2B_Moter_Pin GPIO_PIN_1
#define E2B_Moter_GPIO_Port GPIOA
#define Gray_Tx_Pin GPIO_PIN_2
#define Gray_Tx_GPIO_Port GPIOA
#define Gray_Rx_Pin GPIO_PIN_3
#define Gray_Rx_GPIO_Port GPIOA
#define ADC_Moter_Pin GPIO_PIN_6
#define ADC_Moter_GPIO_Port GPIOA
#define PWMB_Moter_Pin GPIO_PIN_0
#define PWMB_Moter_GPIO_Port GPIOB
#define PWMA_Moter_Pin GPIO_PIN_1
#define PWMA_Moter_GPIO_Port GPIOB
#define BIN2_Moter_Pin GPIO_PIN_12
#define BIN2_Moter_GPIO_Port GPIOB
#define BIN1_Moter_Pin GPIO_PIN_13
#define BIN1_Moter_GPIO_Port GPIOB
#define AIN1_Moter_Pin GPIO_PIN_14
#define AIN1_Moter_GPIO_Port GPIOB
#define AIN2_Moter_Pin GPIO_PIN_15
#define AIN2_Moter_GPIO_Port GPIOB
#define Vofa_Tx_Pin GPIO_PIN_9
#define Vofa_Tx_GPIO_Port GPIOA
#define Vofa_Rx_Pin GPIO_PIN_10
#define Vofa_Rx_GPIO_Port GPIOA
#define E1A_Moter_Pin GPIO_PIN_6
#define E1A_Moter_GPIO_Port GPIOB
#define E1B_Moter_Pin GPIO_PIN_7
#define E1B_Moter_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define MaxDistance 10 //小车运行到该距离是开始判断是否检测到终点 m

//检测终点的紫色的范围
#define Purple_min 3333
#define Purple_max 3777

//终点时最后移动的距离 一圈的脉冲数*转的圈数
#define End_Move 960*3

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
