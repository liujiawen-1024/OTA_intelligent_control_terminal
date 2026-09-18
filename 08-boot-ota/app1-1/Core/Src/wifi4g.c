
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "stm32f1xx_it.h"
#include "wifi4g.h"
#include "main.h"
#include "usart.h"
#include "oled.h"
#include "gpio.h"
#include "fifo.h"
#include "mqtt.h"

#pragma diag_suppress 870

__IO uint8_t WIFI4G_CMD_Status = 0;	   // 命令执行结果的状态标志位
__IO uint8_t WIFI4G_TouChuan_Flag = 0; // 透传标志

uint8_t Parse_Substr[32] = {0}; // 用来判断指令成功的状态

uint8_t Test_WIFI4G_CMD_Status(uint32_t Timeout)
{
	while (WIFI4G_CMD_Status == WIFI4G_NOT) // 没找到继续找
	{
		if ((Timeout--) == 0)
			return WIFI4G_NOT; // 超时时间到 ， 函数返回
		HAL_Delay(1);
	}
	return WIFI4G_CMD_Status;
}

// 这里要使用一个全局变量 Parse_Substr, 表示要解析的数组
uint8_t RecvBuf[NSize] = {0};
uint8_t WIFI4G_Parse_Queue(sequeue_t *sq)
{
	// memset(RecvBuf,0,NSize);
	// 把队列中的数据导出到数组内， 便于进行搜索
	if (sq != NULL)
	{
		int32_t i = 0;
		while (!Queue_IsEmpty(sq))
		{
			Dequeue(sq, RecvBuf + i);
			HAL_UART_Transmit(&huart1,RecvBuf+i, 1,1000); //  把收到的数据进行转发
			i++;
		}
		RecvBuf[i] = 0;
	}

	// 使用 "{" 对接收的数据进行分割， 分割后可以提取出json字符串
	if(MQTT_Download_Flag) // 连接到服务器后开始解析数据
	{
			CAT1_Parse_ATComander(RecvBuf);
			return SET; // 处理完成后返回 
	}
	
	// printf("RecvBuf:%s\n",RecvBuf);
	if (strstr((const char *)RecvBuf, (const char *)Parse_Substr) != NULL) // 找到单词 返回单词的位置
	{
		// printf("found Parse_Substr:%s\n",Parse_Substr);
		return WIFI4G_OK; // 找到 指令返回OK
	}
	else if (strstr((const char *)RecvBuf, (const char *)"ERROR\r\n") != NULL)
	{
		// printf("found Parse_Substr:%s\n","ERROR");
		return WIFI4G_ERROR; // 找到指令返回ERROR
	}
	else
	{
		// printf("not found Parse_Substr:%s\n",Parse_Substr);
		return WIFI4G_NOT; // 没找到返回假
	}
}

