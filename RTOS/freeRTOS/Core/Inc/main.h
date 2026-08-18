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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f4xx_hal.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern CAN_HandleTypeDef  hcan1;
extern I2C_HandleTypeDef  hi2c1;
extern UART_HandleTypeDef huart1;   /* GPS module   */
extern UART_HandleTypeDef huart2;   /* GSM module   */
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define LCD_RS_PORT     GPIOB
#define LCD_RS_PIN      GPIO_PIN_12
#define LCD_EN_PORT     GPIOB
#define LCD_EN_PIN      GPIO_PIN_13
#define LCD_D4_PORT     GPIOB
#define LCD_D4_PIN      GPIO_PIN_14
#define LCD_D5_PORT     GPIOB
#define LCD_D5_PIN      GPIO_PIN_15
#define LCD_D6_PORT     GPIOB
#define LCD_D6_PIN      GPIO_PIN_10
#define LCD_D7_PORT     GPIOB
#define LCD_D7_PIN      GPIO_PIN_11


/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_CAN1_Init(void);
void MX_I2C1_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void Error_Handler(void);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
