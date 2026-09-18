#include "modbus.h"
#include <stdint.h>
#include <stdio.h>
#include "gpio.h"
#include "aht20.h"
#include "ina226.h"
#include "adc.h"
#include "i2c.h"
#include "mb.h"

// 定义一个 500ms 定时时间标识位 ， 刷新硬件传感器
__IO uint8_t TIM1_Timeout_Flag = 0;
// 设备默认从机地址
uint8_t SlaveAddress = 1;
// 从机地址标志位
__IO uint8_t Modify_SlaveAddress_Flag = 0;

uint8_t Write_SlaveAddress(uint8_t addr,uint8_t data)
{
	// i  是eeprom的内存地址 
	// AT24C02_ADDRESS_WRITE 是24c02的地址
	// 1000 是超时时间 
	HAL_I2C_Mem_Write(&hi2c1, AT24C02_ADDRESS_WRITE,addr,I2C_MEMADD_SIZE_8BIT,&data,1, 1000);
	return SET;
}
 
uint8_t Read_SlaveAddress(uint8_t addr,uint8_t *pdata)
{
	// i  是eeprom的内存地址 
	// AT24C02_ADDRESS_READ 是24c02的地址
	// 1000 是超时时间 
	HAL_I2C_Mem_Read(&hi2c1, AT24C02_ADDRESS_READ,addr,I2C_MEMADD_SIZE_8BIT,pdata,1,1000); 
	
	// eeprom 没有被写入过时 ，默认为0
	if(*pdata == 0)
	{
		*pdata = 1;
	}
	
	return SET;
}

void Modbus_Init(void)
{
	// 程序执行后 ， 自动从 eeprom 读取从机地址
	Read_SlaveAddress(0,&SlaveAddress);
	printf("Read_SlaveAddress is %d\n",SlaveAddress);
	eMBInit(MB_RTU, SlaveAddress, 0, 115200, MB_PAR_NONE,1); // 初始化modbus为RTU方式，波特率115200，无校验
	eMBEnable();
}

void Modbus_Parse(void)
{
	if(TIM1_Timeout_Flag)
	{
		TIM1_Timeout_Flag  = 0;
		AHT20_Read(); // 读取温湿度
		INA226_Read(); // 读取电流电压功率
		ADC_VR_CPU_Read(); // 读取电压和CPU温度
	}	
	
	if(Modify_SlaveAddress_Flag)
	{
		Modify_SlaveAddress_Flag = 0;
		SlaveAddress = REG_HOLD_BUF[9];
		Write_SlaveAddress(0,SlaveAddress);
		HAL_Delay(10);
		// 关闭重启 modbus
		eMBDisable();
		eMBClose();
		Modbus_Init();	
	}
	
	if(REG_HOLD_BUF[0] & LED1_CMD)
	{
		LED_Control(LED1,ON);
	}
	else
	{
		LED_Control(LED1,OFF);
	}
	/*********************************/
	if(REG_HOLD_BUF[0] & LED2_CMD)
	{
		LED_Control(LED2,ON);
	}
	else
	{
		LED_Control(LED2,OFF);
	}
	/*********************************/
	if(REG_HOLD_BUF[0] & BEEP_CMD)
	{
		BEEP_Control(ON);
	}
	else
	{
		BEEP_Control(OFF);
	}
	/*********************************/
	if(REG_HOLD_BUF[0] & RELAY_CMD)
	{
		RELAY_Control(ON);
	}
	else
	{
		RELAY_Control(OFF);
	}
}

