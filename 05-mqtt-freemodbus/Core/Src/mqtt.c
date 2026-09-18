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
	uint8_t buf[64] = {0};
	printf("cpuid is %s\n",Get_CPUID());
	strcpy((char *)Parse_Substr,"OK\r\n");
	
	// 设置用户信息
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
uint8_t ML307_Connect_MQTTServer(void)
{
		uint8_t buf[64] = {0};
		OLED_ShowStr(0,4,(unsigned char *)"MQTTServer... --",2);	//测试8*16字符
		
		printf("cpuid:%s\n",Get_CPUID());
		
		strcpy((char *)Parse_Substr,"OK\r\n");
		// AT+DTUTASK="1","20"
		WIFI4G_CMD_Status = WIFI4G_NOT;			// 初始化标志位
		sprintf((char *)buf, "AT+DTUTASK=\"1\",\"20\"\r\n"); 
		HAL_UART_Transmit(&huart3, buf, strlen((char *)buf), 1000);
		if (Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)
		{
				return RESET;												// 等待OK返回
		}
		
		WIFI4G_CMD_Status = WIFI4G_NOT;			// 初始化标志位
		sprintf((char *)buf, "AT+MQTT=\"%s\"\r\n",Get_CPUID()); // 单链接
		HAL_UART_Transmit(&huart3, buf, strlen((char *)buf), 1000);
		if (Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)
		{
				return RESET;												// 等待OK返回
		}
	
		// 设置mqtt服务器
		WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
		strcpy((char *)buf,"AT+MQTTIP=\"broker.emqx.io\",\"1883\"\r\n");
		HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
		if(Test_WIFI4G_CMD_Status(5*1000) == WIFI4G_ERROR)  {
				return RESET ;  
		}
		
		//  订阅主题消息 , 订阅一个下行数据的主题 
		WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
		sprintf((char *)buf,"AT+MQTTSUB=0,\"STM32V9/DownLoad/%s\",0\r\n",Get_CPUID());  
		HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
		if(Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)  {
				return RESET ;  
		}		
		//  订阅主题消息 , 订阅一个下行数据的主题 
		WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
		sprintf((char *)buf,"AT+MQSUB=\"1\",\"1\",\"4\",\"STM32V9/DownLoad/%s\"\r\n",Get_CPUID());
		HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
		if(Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)  {
				return RESET ;  
		}	
		
		//  订阅主题消息 , 设置一个上行数据的主题 
		WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
		// AT+MQPUBM=0,1,0,4,"STM32V9/UPLoad/6703401957694950066AFF51"
		//AT+MQPUB="1","1","4","/a12o68BvvHO/myDevice/user/mypub"
		sprintf((char *)buf,"AT+MQPUB=\"1\",\"1\",\"4\",\"STM32V9/UPLoad/%s\"\r\n",Get_CPUID());
		
		HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
		if(Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)  {
				return RESET ;  
		}	
		
		//  设置单通道通信 ， 1个上行 1个下行 
		WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
		sprintf((char *)buf,"AT+MQMULTEN=0\r\n"); // 
		HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
		if(Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)  {
				return RESET ;  
		}	
				
		// AT+REST 重启后模块配置生效 
		
		//  订阅主题消息 , 订阅一个下行数据的主题 

		WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
		sprintf((char *)buf,"AT+REST\r\n");
		strcpy((char *)Parse_Substr,"MQTT_CONNECT:");// 服务器连接成功的返回值
		HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
		uint8_t ret = Test_WIFI4G_CMD_Status(180*1000) ;
		if (ret == WIFI4G_OK)
		{
				OLED_ShowStr(0,4,(unsigned char *)"MQTTServer... OK",2);	//测试8*16字符
				return SET;
		}
		else 
		{
				OLED_ShowStr(0,4,(unsigned char *)"MQTTServer... --",2);	//测试8*16字符
				return  RESET; 
		}
}


uint8_t MQTT_Connect_Server(void)
{
	#if MQTT_WIFI_4G_ENABLE
	// 连接WIFI
	ESP8266_Connect_WIFI();
	
	// 连接 MQTT 服务器
	if(ESP8266_Connect_MQTTServer() == SET)
	{
		MQTT_Download_Flag = 1 ;
		printf("MQTT 服务器连接成功！！！\n");
	}
	
	#else
	// 连接mqtt服务器
	if( ML307_Connect_MQTTServer() == SET)
	{
	MQTT_Download_Flag = 1 ; // 开启解析 服务器数据的标志位
	printf("MQTT 服务器连接成功！！！\n");
	}
	#endif
	return SET;
}

uint8_t MQTT_SendData(void)
{
	static uint8_t sendbuf[128] = {0};
	static uint8_t buf[255] = {0};
	
	#if MQTT_WIFI_4G_ENABLE
	sprintf((char *)sendbuf,"\"{\\\"TP\\\":%d\\,\\\"RH\\\":%d\\,\\\"VO\\\":%d\\,\\\"CU\\\":%d\\,\\\"PW\\\":%d\\,\\\"VR\\\":%d\\,\\\"CPU\\\":%d}\"",
	REG_HOLD_BUF[1],REG_HOLD_BUF[2],
	REG_HOLD_BUF[3],REG_HOLD_BUF[4],REG_HOLD_BUF[5],REG_HOLD_BUF[6],REG_HOLD_BUF[7]);
	
	// 发送数据到mqtt服务器
	//printf("%s",sendbuf);
	sprintf((char *)buf,"AT+MQTTPUB=0,\"STM32V9/UPLoad/%s\",%s,0,0\r\n",Get_CPUID(),sendbuf);
	#else
	sprintf((char *)sendbuf,"{\"TP\":%d,\"RH\":%d,\"VO\":%d,\"CU\":%d,\"PW\":%d,\"VR\":%d,\"CPU\":%d}",
	REG_HOLD_BUF[1],REG_HOLD_BUF[2], REG_HOLD_BUF[3],REG_HOLD_BUF[4],
	REG_HOLD_BUF[5],REG_HOLD_BUF[6],REG_HOLD_BUF[7]);

	//printf("%s",sendbuf);
	// 发送数据到mqtt服务器
	sprintf((char *)buf,"%s",sendbuf);
	printf("%s",buf);
	#endif
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




