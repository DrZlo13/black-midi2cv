/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
#define HAL_PCD_MODULE_ENABLED
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"

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
#define LED_PIN_Pin LL_GPIO_PIN_13
#define LED_PIN_GPIO_Port GPIOC
#define TIM2_CH1_CH1_MSB_Pin LL_GPIO_PIN_0
#define TIM2_CH1_CH1_MSB_GPIO_Port GPIOA
#define TIM2_CH2_CH1_LSB_Pin LL_GPIO_PIN_1
#define TIM2_CH2_CH1_LSB_GPIO_Port GPIOA
#define USART2_DEBUG_TX_Pin LL_GPIO_PIN_2
#define USART2_DEBUG_TX_GPIO_Port GPIOA
#define USART2_DEBUG_RX_Pin LL_GPIO_PIN_3
#define USART2_DEBUG_RX_GPIO_Port GPIOA
#define ADC_PORTAMENTO_IN_Pin LL_GPIO_PIN_5
#define ADC_PORTAMENTO_IN_GPIO_Port GPIOA
#define GPIO_CH2_GATE_OUT_Pin LL_GPIO_PIN_7
#define GPIO_CH2_GATE_OUT_GPIO_Port GPIOA
#define GPIO_CH1_GATE_OUT_Pin LL_GPIO_PIN_0
#define GPIO_CH1_GATE_OUT_GPIO_Port GPIOB
#define GPIO_CLOCK_OUT_Pin LL_GPIO_PIN_1
#define GPIO_CLOCK_OUT_GPIO_Port GPIOB
#define GPIO_MODE_IN_Pin LL_GPIO_PIN_12
#define GPIO_MODE_IN_GPIO_Port GPIOB
#define TIM1_CH1_CH2_MSB_Pin LL_GPIO_PIN_8
#define TIM1_CH1_CH2_MSB_GPIO_Port GPIOA
#define TIM1_CH2_CH2_LSB_Pin LL_GPIO_PIN_9
#define TIM1_CH2_CH2_LSB_GPIO_Port GPIOA
#define USART1_MIDI_RX_Pin LL_GPIO_PIN_10
#define USART1_MIDI_RX_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
