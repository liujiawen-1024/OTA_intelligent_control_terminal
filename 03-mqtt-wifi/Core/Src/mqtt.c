#include "mqtt.h"
#include "wifi4g.h"
#include "fifo.h"
#include "usart.h"
#include "oled.h"
#include "modbus.h"
#include "cJSON.h"
#include "gpio.h"

__IO uint8_t MQTT_UPLoad_Flag = 0;		// 上传数据标志位，5s
__IO uint8_t MQTT_Download_Flag = 0; 	// 接收服务器下发指令数据位

uint8_t *Get_CPUID(void)
{
	static uint8_t cpuid[32] = {0};
	
	// 每个芯片都有唯一的 96_bit unique ID
	uint32_t CPU_ID[3];
	CPU_ID[0] = *(__IO uint32_t *)(0X1FFFF7F0); // 高字节
	CPU_ID[1] = *(__IO uint32_t *)(0X1FFFF7EC); //
	CPU_ID[2] = *(__IO uint32_t *)(0X1FFFF7E8); // 低字节
	
	/* 芯片的唯一ID */
	sprintf((char *)cpuid, "%08X%08X%08X", CPU_ID[0], CPU_ID[1], CPU_ID[2]);
	return cpuid;
}

uint8_t ESP8266_Connect_MQTTServer(void)
{
	// 设置用户信息
	uint8_t buf[64] = {0};
	printf("cpuid is %s\n",Get_CPUID());
	WIFI4G_CMD_Status = WIFI4G_NOT;			// 初始化标志位
	sprintf((char *)buf, "AT+MQTTUSERCFG=0,1,\"%s\",\"\",\"\",0,0,\"\"\r\n",Get_CPUID()); // 单链接
	HAL_UART_Transmit(&huart3, buf, strlen((char *)buf), 1000);
	
	if (Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)
	{
		return RESET;												// 等待OK返回
	}

	// 连接 mqtt 服务器
	WIFI4G_CMD_Status = WIFI4G_NOT;			 // 初始化标志位
	strcpy((char *)buf, "AT+MQTTCONN=0,\"broker.emqx.io\",1883,1\r\n"); // 设置透传模式
	HAL_UART_Transmit(&huart3, buf, strlen((char *)buf), 1000);

	if (Test_WIFI4G_CMD_Status(5*1000) == WIFI4G_ERROR)
	{
		return RESET;												// 等待OK返回
	}

	// 订阅主题信息
	WIFI4G_CMD_Status = WIFI4G_NOT;
	sprintf((char *)buf,"AT+MQTTSUB=0,\"STM32V9/DownLoad/%s\",0\r\n",Get_CPUID());
	HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
	
	OLED_ShowStr(0,4,(unsigned char *)"MQTTServer... OK",2);	//测试8*16字符
	
	uint8_t ret = Test_WIFI4G_CMD_Status(5*1000);
	if(ret != WIFI4G_OK)
	{
		return RESET;
	}
	
	return SET;
}

uint8_t MQTT_SendData(void)
{
	static uint8_t sendbuf[128] = {0};
	static uint8_t buf[255] = {0};
	
	sprintf((char *)sendbuf,"\"{\\\"TP\\\":%d\\,\\\"RH\\\":%d\\,\\\"VO\\\":%d\\,\\\"CU\\\":%d\\,\\\"PW\\\":%d\\,\\\"VR\\\":%d\\,\\\"CPU\\\":%d}\"",
	REG_HOLD_BUF[1],REG_HOLD_BUF[2],
	REG_HOLD_BUF[3],REG_HOLD_BUF[4],REG_HOLD_BUF[5],REG_HOLD_BUF[6],REG_HOLD_BUF[7]);
	
	// 发送数据到mqtt服务器
	//printf("%s",sendbuf);
	sprintf((char *)buf,"AT+MQTTPUB=0,\"STM32V9/UPLoad/%s\",%s,0,0\r\n",Get_CPUID(),sendbuf);
	
	HAL_UART_Transmit(&huart3,(uint8_t*)buf,strlen((char *)buf),1000);
	
	return SET;
}

uint8_t MQTT_Parse_JsonData(uint8_t *json)
{
	cJSON *cjson_device = NULL;
	cJSON *cjson_led1 = NULL;
	cJSON *cjson_led2 = NULL;
	cJSON *cjson_beep = NULL;
	cJSON *cjson_relay = NULL;
	
	// 解析整段JSON数据
	cjson_device = cJSON_Parse((char *)json);
	if(cjson_device == NULL)
	{
		printf("parse fail.\n");
		return RESET;
	}
	else
	{
		//printf("json->%s\n",cJSON_Print(cjson_device));
	}
	
	cjson_led1 = cJSON_GetObjectItem(cjson_device,"LED1");
	cjson_led2 = cJSON_GetObjectItem(cjson_device,"LED2");
	cjson_beep = cJSON_GetObjectItem(cjson_device,"BEEP");
	cjson_relay = cJSON_GetObjectItem(cjson_device,"RELAY");
	
	/****************************************************************/
	if(cjson_led1->valueint != NULL)
	{
		if(cjson_led1->valueint == 1)
		{
			REG_HOLD_BUF[0] = REG_HOLD_BUF[0] | LED1_CMD;
		}
		else
		{
			REG_HOLD_BUF[0] = REG_HOLD_BUF[0] & (~LED1_CMD);
		}
	}
	/****************************************************************/
	if(cjson_led2->valueint != NULL)
	{
		if(cjson_led2->valueint == 1)
		{
			REG_HOLD_BUF[0] = REG_HOLD_BUF[0] | LED2_CMD;
		}
		else
		{
			REG_HOLD_BUF[0] = REG_HOLD_BUF[0] & (~LED2_CMD);
		}
	}
	/****************************************************************/
	if(cjson_beep->valueint != NULL)
	{
		if(cjson_beep->valueint == 1)
		{
			REG_HOLD_BUF[0] = REG_HOLD_BUF[0] | BEEP_CMD;
		}
		else
		{
			REG_HOLD_BUF[0] = REG_HOLD_BUF[0] & (~BEEP_CMD);
		}
	}	
	/****************************************************************/
	if(cjson_relay->valueint != NULL)
	{
		if(cjson_relay->valueint == 1)
		{
			REG_HOLD_BUF[0] = REG_HOLD_BUF[0] | RELAY_CMD;
		}
		else
		{
			REG_HOLD_BUF[0] = REG_HOLD_BUF[0] & (~RELAY_CMD);
		}
	}
	/****************************************************************/
	cJSON_Delete(cjson_device);
	
	return SET;
}




