/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
#include "i2c.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
#include <string.h>
/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void EEPROM_Test(void)
{
	#define N 256 
	  
	uint8_t src[N]={0};
	uint8_t dst[N]={0};
	for(uint16_t i =0;i < N;i++)
	{
		src[i]= i;
		dst[i]= 0;
	}
	printf("writing data to eeprom...\n");
#if 0 
	// 每次写1个字节
	for(uint16_t i=0;i < N;i++)
	{
		// i  是eeprom的内存地址 
		// AT24C02_ADDRESS_WRITE 是24c02的地址
		// 1000 是超时时间 
		HAL_I2C_Mem_Write(&hi2c1, AT24C02_ADDRESS_WRITE,i,I2C_MEMADD_SIZE_8BIT,&src[i],1, 1000); 
		HAL_Delay(5) ; // 这个延时必须的 , 等待数据写入到掉电非易失区
	}
#else 
	// 每次写8个字节 , 每次写1页
	for(uint16_t i=0;i < N;i +=8)
	{
		// i  是eeprom的内存地址 
		// AT24C02_ADDRESS_WRITE 是24c02的地址
		// 1000 是超时时间 
		HAL_I2C_Mem_Write(&hi2c1, AT24C02_ADDRESS_WRITE,i,I2C_MEMADD_SIZE_8BIT,&src[i],8, 1000); 
		HAL_Delay(5) ; // 这个延时必须的 , 等待数据写入到掉电非易失区
	}
#endif 

	

	printf("read from eeprom...\n") ;
	HAL_Delay(20); // 等待eeprom 操作完成 
	
#if  0
	// 1次读1个字节
	for(uint16_t i =0 ;i< N;i++)
	{
		// i  是eeprom的内存地址 
		// AT24C02_ADDRESS_READ 是24c02的地址
		// 1000 是超时时间 
		HAL_I2C_Mem_Read(&hi2c1, AT24C02_ADDRESS_READ,i,I2C_MEMADD_SIZE_8BIT,&dst[i],1,1000); 
		HAL_Delay(1);
	}
#else
	// 0 : 表示的是起始地址  
	// dst : 这个的地址会自动增加 
	// N   : 表示地址增加的数量 
	HAL_I2C_Mem_Read(&hi2c1,AT24C02_ADDRESS_READ,0,I2C_MEMADD_SIZE_8BIT,dst,N,1000);	
#endif 
	
	for(int32_t i =0 ;i< N;i++)
	{
		printf("0x%02X ",dst[i]);
	}
	printf("\n");
	
	if(memcmp(dst,src,N) ==0)
	{
			printf("eeprom test ok\n");
	}
	else 
	{
			printf("eeprom test fail\n");
	}
    
	
}
/* USER CODE END 1 */
