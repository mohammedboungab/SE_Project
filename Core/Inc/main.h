/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f7xx_hal.h"

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
#define pluie_Pin GPIO_PIN_15
#define pluie_GPIO_Port GPIOA
#define pluie_EXTI_IRQn EXTI15_10_IRQn
#define SD_Pin GPIO_PIN_13
#define SD_GPIO_Port GPIOC
#define user_led_Pin GPIO_PIN_1
#define user_led_GPIO_Port GPIOI
#define user_btn_Pin GPIO_PIN_11
#define user_btn_GPIO_Port GPIOI
#define user_btn_EXTI_IRQn EXTI15_10_IRQn
#define poussoir_btn_2_Pin GPIO_PIN_9
#define poussoir_btn_2_GPIO_Port GPIOF
#define poussoir_btn_2_EXTI_IRQn EXTI9_5_IRQn
#define poussoir_btn_1_Pin GPIO_PIN_8
#define poussoir_btn_1_GPIO_Port GPIOF
#define poussoir_btn_1_EXTI_IRQn EXTI9_5_IRQn
#define blue_led_Pin GPIO_PIN_9
#define blue_led_GPIO_Port GPIOH
#define red_led_Pin GPIO_PIN_11
#define red_led_GPIO_Port GPIOH
#define green_led_Pin GPIO_PIN_10
#define green_led_GPIO_Port GPIOH

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
