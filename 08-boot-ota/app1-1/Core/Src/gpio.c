/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
uint8_t Key_Value ; // 按键按下的值
/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED2_Pin|LED1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : BEEP_Pin */
  GPIO_InitStruct.Pin = BEEP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BEEP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RELAY_Pin LED2_Pin LED1_Pin */
  GPIO_InitStruct.Pin = RELAY_Pin|LED2_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/**
  * @brief  控制LED1 LED2 亮灭的函数
  *
  * @note   详细描述细节，ON 用来控制led亮， OFF用来控制LED熄灭
  *
  * @param  device: 可以是LED1, LED2 , 是两个宏，定义在gpio.h中
  * @param  cmd: 用来控制LED的亮灭命令
	*           ON  : 点亮LED  
	*           OFF ：熄灭LED
  * @retval None
  */
void LED_Control(uint8_t device,uint8_t cmd)
{
		if(device == LED1)
		{
				if(cmd == ON ) // LED1 ON 
				{
					HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); 
				}
				else if(cmd == OFF)  // LED1 OFF
				{
					HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); 
				}
		}
		else if(device == LED2)
		{
				if(cmd == ON ) // LED2 ON 
				{
					HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); 
				}
				else if(cmd == OFF) // LED2 OFF
				{
					HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); 
				}
		}
	
}


/**
  * @brief  控制蜂鸣器的函数
  *
  * @note   详细描述细节，ON 用来控制beep响， OFF用来控制beep不响
  *
  * @param  cmd: 用来控制BEEP的响与不响命令
  *           ON  : BEEP响 
  *           OFF ：不响
  * @retval None
  */
void BEEP_Control(uint8_t cmd)
{

			if(cmd == ON ) // BEEP ON 
			{
				HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET); 
			}
			else if(cmd == OFF)  // BEEP OFF
			{
				HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET); 
			}
}




/**
  * @brief  控制继电器的函数
  *
  * @note   详细描述细节，ON 用来控制继电器吸合， OFF用来控制继电器断开
  *
  * @param  cmd: 用来控制BEEP的响与不响命令
  *           ON  : BEEP响 
  *           OFF ：不响
  * @retval None
  */
void RELAY_Control(uint8_t cmd)
{

			if(cmd == ON ) // RELAY ON 
			{
				HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET); 
			}
			else if(cmd == OFF)  // RELAY OFF
			{
				HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET); 
			}
}






/* USER CODE END 2 */
