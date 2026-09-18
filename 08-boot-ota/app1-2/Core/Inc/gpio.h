/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define    LED1     1 
#define    LED2     2  
#define    ON       1 
#define    OFF      0 
#define    KEY_UP   	1 
#define    KEY_DOWN 	2 
#define    KEY_LEFT 	3 
#define    KEY_RIGHT  4 
#define    KEY_OK   	5 
#define    KEY_ESC   	6 

extern uint8_t Key_Value ; // 按键按下的值


/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
void LED_Control(uint8_t device,uint8_t cmd);
void LED_Test(void);
void BEEP_Control(uint8_t cmd);
void BEEP_Test(void);
void RELAY_Control(uint8_t cmd);
void RELAY_Test(void);
uint8_t KEY_Scan(void);
void KEY_Test(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

