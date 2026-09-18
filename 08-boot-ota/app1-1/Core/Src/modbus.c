#include "modbus.h"
#include <stdint.h>
#include <stdio.h>
#include "gpio.h"
#include "aht20.h"
#include "ina226.h"
#include "adc.h"
#include "i2c.h"


// 定义1个 500ms 定时时间到标志位, 刷新各种硬件的状态   
__IO uint8_t TIM1_Timeout_Flag = 0 ; 
uint8_t SlaveAddress = 1  ; // 设备的默认地址为1
__IO uint8_t Modify_SlaveAdress_Flag = 0 ; // 修改从机地址标志位

// 十路保持寄存器  可读可写的2字节寄存器
#define REG_HOLD_SIZE   10
uint16_t REG_HOLD_BUF[REG_HOLD_SIZE];

void Modbus_Parse(void)
{
		if(TIM1_Timeout_Flag)  // 500ms 周期性的执行一次
		{
				TIM1_Timeout_Flag = 0 ;	
				//  刷新各种硬件传感器的状态  
				AHT20_Read(); // 读温湿度		
			
				// 刷新ina226传感器
				INA226_Read();
			
				// 刷新电位器和CPU温度
				ADC_VR_CPU_Read();
			
			  // 状态指示灯 
				
		}
		
		if(REG_HOLD_BUF[0] & LED1_CMD)   
		{
				LED_Control(LED1,ON);
		}
		else 
		{
				LED_Control(LED1,OFF);
		}
		/***************************************/
		if(REG_HOLD_BUF[0] & LED2_CMD)   
		{
				LED_Control(LED2,ON);
		}
		else 
		{
				LED_Control(LED2,OFF);
		}
		/***************************************/
		if(REG_HOLD_BUF[0] & BEEP_CMD)   
		{
				BEEP_Control(ON);
		}
		else 
		{
				BEEP_Control(OFF);
		}
		/***************************************/
		if(REG_HOLD_BUF[0] & RELAY_CMD)   
		{
				RELAY_Control(ON);
		}
		else 
		{
				RELAY_Control(OFF);
		}
}

