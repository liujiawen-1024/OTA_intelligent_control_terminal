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
  HAL_GPIO_WritePin(GPIOB, RELAY_Pin|RS485_WR_Pin, GPIO_PIN_RESET);

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

  /*Configure GPIO pin : RS485_WR_Pin */
  GPIO_InitStruct.Pin = RS485_WR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(RS485_WR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY_OK_Pin KEY_UP_Pin KEY_DOWN_Pin KEY_RIGHT_Pin
                           KEY_LEFT_Pin */
  GPIO_InitStruct.Pin = KEY_OK_Pin|KEY_UP_Pin|KEY_DOWN_Pin|KEY_RIGHT_Pin
                          |KEY_LEFT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : KEY_ESC_Pin */
  GPIO_InitStruct.Pin = KEY_ESC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY_ESC_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

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
  * @brief  LED1 LED2 测试函数
  *
  * @note   验证LED1 LED2的功能
  *
  * @retval None
  */
void LED_Test(void)
{
		LED_Control(LED1,ON); 
		LED_Control(LED2,OFF); 
		HAL_Delay(500); 
		LED_Control(LED1,OFF); 
		LED_Control(LED2,ON); 
		HAL_Delay(500); 
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
  * @brief  BEEP 测试函数
  *
  * @note   验证BEEP的功能
  *
  * @retval None
  */
void BEEP_Test(void)
{
		BEEP_Control(ON); 
		HAL_Delay(200); 
		BEEP_Control(OFF); 
		HAL_Delay(800); 
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

/**
  * @brief  RELAY 测试函数
  *
  * @note   验证RELAY的功能
  *
  * @retval None
  */
void RELAY_Test(void)
{
		RELAY_Control(ON); 
		HAL_Delay(1000); 
		RELAY_Control(OFF); 
		HAL_Delay(1000); 
}


/**
  * @brief  获取按键的值
  *
  * @note   详细描述细节，根据按下的按键返回按键的键值
  *
  * @param  无
  * @retval  按下的键值 ， 定义在gpio.h 当中
  */
uint8_t KEY_Scan(void)
{
	// UP 按键被按下后,引脚变为低电平 
	if(HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,KEY_UP_Pin) == GPIO_PIN_RESET) 
	{
			return KEY_UP;
	}
	// DOWN 按键被按下后,引脚变为低电平 
	else 	if(HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port,KEY_DOWN_Pin) == GPIO_PIN_RESET) 
	{
			return KEY_DOWN;
	}
	// LEFT 按键被按下后,引脚变为低电平 
	else 	if(HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port,KEY_LEFT_Pin) == GPIO_PIN_RESET) 
	{
			return KEY_LEFT;
	}
	// RIGHT 按键被按下后,引脚变为低电平 
	else 	if(HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port,KEY_RIGHT_Pin) == GPIO_PIN_RESET) 
	{
			return KEY_RIGHT;
	}
	// OK 按键被按下后,引脚变为低电平 
	else 	if(HAL_GPIO_ReadPin(KEY_OK_GPIO_Port,KEY_OK_Pin) == GPIO_PIN_RESET) 
	{
			return KEY_OK;
	}
	// ESC 按键被按下后,引脚变为低电平 
	else 	if(HAL_GPIO_ReadPin(KEY_ESC_GPIO_Port,KEY_ESC_Pin) == GPIO_PIN_RESET) 
	{
			return KEY_ESC;
	}
	return 0;  // 什么都没有按下
}


void KEY_Test(void)
{
	uint8_t keyval = KEY_Scan(); 
	if(keyval == KEY_UP)
	{
			LED_Control(LED1,ON); // led1 on 
	} 
	else if(keyval == KEY_DOWN)
	{
			LED_Control(LED1,OFF); // led1 off 
	}
	else if(keyval == KEY_LEFT)
	{
			LED_Control(LED2,ON); // led2 on 
	}
	else if(keyval == KEY_RIGHT)
	{
			LED_Control(LED2,OFF); // led2 off 
	}
	else if(keyval == KEY_OK)
	{
			RELAY_Control(ON); // relay on 
	}
	else if(keyval == KEY_ESC)
	{
			RELAY_Control(OFF); // relay off 
	}
}


/* USER CODE END 2 */
