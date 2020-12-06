/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#define LCD_RS_Pin GPIO_PIN_0
#define LCD_RS_GPIO_Port GPIOA
#define LCD_RW_Pin GPIO_PIN_1
#define LCD_RW_GPIO_Port GPIOA
#define LCD_E_Pin GPIO_PIN_2
#define LCD_E_GPIO_Port GPIOA
#define LCD_D4_Pin GPIO_PIN_3
#define LCD_D4_GPIO_Port GPIOA
#define LCD_D5_Pin GPIO_PIN_4
#define LCD_D5_GPIO_Port GPIOA
#define LCD_D6_Pin GPIO_PIN_5
#define LCD_D6_GPIO_Port GPIOA
#define LCD_D7_Pin GPIO_PIN_6
#define LCD_D7_GPIO_Port GPIOA
#define DISABLED_Pin GPIO_PIN_7
#define DISABLED_GPIO_Port GPIOA
#define DRV_MOSFET_Pin GPIO_PIN_10
#define DRV_MOSFET_GPIO_Port GPIOB
#define ON_Pin GPIO_PIN_12
#define ON_GPIO_Port GPIOB
#define OFF_Pin GPIO_PIN_13
#define OFF_GPIO_Port GPIOB
#define SELECT_Pin GPIO_PIN_7
#define SELECT_GPIO_Port GPIOB
#define SELECT_EXTI_IRQn EXTI9_5_IRQn
#define DISABLE_Pin GPIO_PIN_8
#define DISABLE_GPIO_Port GPIOB
#define DISABLE_EXTI_IRQn EXTI9_5_IRQn
#define RESET_Pin GPIO_PIN_9
#define RESET_GPIO_Port GPIOB
#define RESET_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
