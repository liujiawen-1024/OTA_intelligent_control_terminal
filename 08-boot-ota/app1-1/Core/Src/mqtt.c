#include "mqtt.h"
#include "wifi4g.h"
#include "usart.h"
#include "oled.h"
#include "modbus.h"
#include "cjson.h"
#include "crc32.h"
#include "boot.h"


__IO uint8_t MQTT_UPLoad_Flag  = 0 ; // 数据上传服务器标志位， 每隔5秒上传1次
__IO uint8_t MQTT_Download_Flag = 0 ;  // 开始接收服务器下发的命令，开始解析的标志位
__IO uint8_t MQTT_OTA_FLAG = 0; //  请求服务器发送分包固件标识位

OTA_FW_Info_T OTA_Info ; // 描述OTA固件信息的结构体

uint8_t CAT1_Parse_ATComander(uint8_t *recvbuf)
{
		char * leftp = (char *)recvbuf ; 
		char * rightp = leftp ; 
		char * startp = leftp ;
		uint32_t dataNum = 0 ; 
		memset(OTA_Info.recv_buf,0,sizeof(OTA_Info.recv_buf)) ; // 清空数组 

		// 解析服务器下发的命令
		char substr[64] = {"v1/devices/me/rpc/request/"};
		if( (startp = strstr((char *)startp,substr) ) != NULL )
		{
                dataNum = strtol(startp + strlen(substr), NULL, 10);
                leftp   = strstr((char *)startp,"{"); // 找到左花括号
                rightp  = strstr((char *)startp,"}"); // 找到右花括号
                if( (leftp != NULL) && (rightp != NULL))
                {
                        strncpy((char *)OTA_Info.recv_buf,leftp,rightp-leftp+1); // 复制字符串
                        //printf("jsonbuf=%s | dataNum=%d\n",OTA_Info.recv_buf,dataNum);
                        MQTT_Parse_DeviceData((uint8_t*)OTA_Info.recv_buf,dataNum);// 解析json字符串并控制设备
                        return SET;
                }
                //startp = startp + strlen(substr); // 移动位置
				
		}
		/***********************************************************/
		// 解析服务器下发的命令
		memset(substr,0,sizeof(substr));
		startp = (char *)recvbuf ;
		strcpy(substr,"v1/devices/me/attributes") ;
		if( (startp = strstr((char *)startp,substr) ) != NULL )
		{
					leftp   = strstr((char *)startp,"{"); // 找到左花括号
					rightp  = strstr((char *)startp,"}"); // 找到右花括号
					if( (leftp != NULL) && (rightp != NULL))
					{
							strncpy((char *)OTA_Info.recv_buf,leftp,rightp-leftp+1); // 复制字符串
							//printf("jsonbuf=%s\n",OTA_Info.recv_buf);
							MQTT_Parse_OTAData(OTA_Info.recv_buf);// 解析json字符串并控制设备
							return SET;
					}
					//startp = startp + strlen(substr); // 移动位置
				
		}
		
		/***********************************************************/
		// 解析服务器下发的命令
		memset(substr,0,sizeof(substr));
		startp = (char *)recvbuf ;

		sprintf(substr,"v2/fw/response/%d/chunk/%d,%d,",
		OTA_Info.request_id,OTA_Info.chunk_id,OTA_Info.request_bytes) ;
		//printf("ota:substr=%s\n",substr);
		if( (startp = strstr((char *)startp,substr) ) != NULL )
		{
					memcpy(OTA_Info.recv_buf,startp+strlen(substr),OTA_Info.request_bytes); // 复制字符串
					//printf("jsonbuf=%s\n",OTA_Info.recv_buf);
					OTA_Info.recv_flag = 1; //  OTA 接收到固件的内容
					return SET;
					//MQTT_Parse_OTAData((uint8_t*)OTA_Info.recv_buf);// 解析json字符串并控制设备	
					//startp = startp + strlen(substr); // 移动位置			
		}
		return RESET;
}


uint8_t MQTT_Parse_DeviceData(uint8_t *json,uint32_t dataNum)
{
	
		cJSON *cjson_device = NULL;
		cJSON *cjson_method = NULL;
		cJSON *cjson_params = NULL;
		/* 解析整段JSO数据 */
		cjson_device = cJSON_Parse((char *)json);
		if (cjson_device == NULL)
		{
			//printf("parse fail.\n");
			return RESET;
		}
		else
		{
			// printf("json->%s\n",cJSON_Print(cjson_device));
		}
		/* 依次根据名称提取JSON数据（键值对） */
		cjson_method  = cJSON_GetObjectItem(cjson_device, "method");
		cjson_params  = cJSON_GetObjectItem(cjson_device, "params");
		//printf("method:%s\n",cjson_method->valuestring);
		//printf("params:%d\n",cjson_params->valueint);
		
		// 获取LED1的状态
		if(strcmp(cjson_method->valuestring,"led1Status") == 0 ) {	
				// ESC 按键被按下后,引脚 变为低电平
				if (HAL_GPIO_ReadPin(LED1_GPIO_Port, LED1_Pin) == GPIO_PIN_RESET)
				{
						MQTT_SendData((uint8_t *)PUB2,dataNum,"true");
				}else {
						MQTT_SendData((uint8_t *)PUB2,dataNum,"false");
				}	
		}
		// 获取beep的状态
		else if(strcmp(cjson_method->valuestring,"beepStatus") == 0 ) {	
				// ESC 按键被按下后,引脚 变为低电平
				if (HAL_GPIO_ReadPin(BEEP_GPIO_Port, BEEP_Pin) == GPIO_PIN_SET)
				{
						MQTT_SendData((uint8_t *)PUB2,dataNum,"true");
				}else {
						MQTT_SendData((uint8_t *)PUB2,dataNum,"false");
				}	
		}
		// 获取继电器的状态
		else if(strcmp(cjson_method->valuestring,"relayStatus") == 0 ) {	
				// ESC 按键被按下后,引脚 变为低电平
				if (HAL_GPIO_ReadPin(RELAY_GPIO_Port, RELAY_Pin) == GPIO_PIN_SET)
				{
						MQTT_SendData((uint8_t *)PUB2,dataNum,"true");
				}else {
						MQTT_SendData((uint8_t *)PUB2,dataNum,"false");
				}	
		}
		
		
		// 控制LED1的状态
		if(strcmp(cjson_method->valuestring,"led1Set") == 0 ) 
		{	
				if(cjson_params->valueint == 1 ) 
				{	
						//LED_Control(LED1, ON);
						REG_HOLD_BUF[0]  |= LED1_CMD  ;   // 第0位 写 1
						MQTT_SendData((uint8_t *)PUB2,dataNum,"true");
				}else if(cjson_params->valueint == 0 ) 
				{
						//LED_Control(LED1, OFF);
						REG_HOLD_BUF[0]  = REG_HOLD_BUF[0] & (~LED1_CMD)  ; // 第0位 写 0 
						MQTT_SendData((uint8_t *)PUB2,dataNum,"false");
				}
		}

		// 控制BEEP的状态
		else if(strcmp(cjson_method->valuestring,"beepSet") == 0 ) 
		{	
				if(cjson_params->valueint == 1 ) 
				{	
						//LED_Control(LED1, ON);
						REG_HOLD_BUF[0]  |= BEEP_CMD  ;   // 第0位 写 1
						MQTT_SendData((uint8_t *)PUB2,dataNum,"true");
				}else if(cjson_params->valueint == 0 ) 
				{
						//LED_Control(LED1, OFF);
						REG_HOLD_BUF[0]  = REG_HOLD_BUF[0] & (~BEEP_CMD)  ; // 第0位 写 0 
						MQTT_SendData((uint8_t *)PUB2,dataNum,"false");
				}
		}
		
		// 控制relay的状态
		if(strcmp(cjson_method->valuestring,"relaySet") == 0 ) 
		{	
				if(cjson_params->valueint == 1 ) 
				{	
						//LED_Control(LED1, ON);
						REG_HOLD_BUF[0]  |= RELAY_CMD  ;   // 第0位 写 1
						MQTT_SendData((uint8_t *)PUB2,dataNum,"true");
				}else if(cjson_params->valueint == 0 ) 
				{
						//LED_Control(LED1, OFF);
						REG_HOLD_BUF[0]  = REG_HOLD_BUF[0] & (~RELAY_CMD)  ; // 第0位 写 0 
						MQTT_SendData((uint8_t *)PUB2,dataNum,"false");
				}
		}
		
		cJSON_Delete(cjson_device); // 一定要释放内存
		return SET;

}


uint8_t MQTT_SendData(uint8_t *topic,uint32_t dataNum,char *status)
{

	uint8_t sendbuf[128] = {0};
	uint8_t buf[256] = {0};
  
	if(strncmp((char *)topic,PUB1,strlen(PUB1)-1) == 0 ) 
	{
		
		  if(dataNum == 1) // 上传传感器数据
			{
				// MQPUB,1,v1/devices/me/telemetry
				sprintf((char *)sendbuf,"{TP:%d,RH:%d,VO:%d,CU:%d,PW:%d,VR:%d,CPU:%d}",
				REG_HOLD_BUF[1],REG_HOLD_BUF[2], REG_HOLD_BUF[3],REG_HOLD_BUF[4],
				REG_HOLD_BUF[5],REG_HOLD_BUF[6],REG_HOLD_BUF[7]);
				// 上行主题1   上传服务器数据的  "v1/devices/me/telemetry"
				sprintf((char *)buf,"MQPUB,1,%s,%s",PUB1,sendbuf);
			}
			else if(dataNum == 2)  // ota 状态更新
			{
				sprintf((char *)sendbuf,"{\"fw_state\":\"UPDATED\"}");
				// 上行主题1   上传服务器数据的  "v1/devices/me/telemetry"
				sprintf((char *)buf,"MQPUB,1,%s,%s",PUB1,sendbuf);
			}
			else if(dataNum == 3)  // ota 状态更新
			{
				sprintf((char *)sendbuf,"{\"fw_state\":\"FAILED\"}");
				// 上行主题1   上传服务器数据的  "v1/devices/me/telemetry"
				sprintf((char *)buf,"MQPUB,1,%s,%s",PUB1,sendbuf);
			}
	
	}
	else if(strncmp((char *)topic,PUB2,strlen(PUB2)-1) == 0 ) 
	{
			// 上行主题2   回复服务器下发的命令 PUB2  "v1/devices/me/rpc/response/"
			sprintf((char *)buf,"MQPUB,1,%s%d,%s",PUB2,dataNum,status);
	}
	
	else if(strncmp((char *)topic,PUB3,strlen(PUB3)-1) == 0 ) 
	{
			// 上行主题2   发送下载固件命令 "v2/fw/request/+/chunk/#"
			sprintf((char *)buf,"MQPUB,1,v2/fw/request/%d/chunk/%d,%s",
				OTA_Info.request_id,OTA_Info.chunk_id,status);
	}


	//printf("MQTT Send Data:%s",buf);
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
		    /* 解析整段JSO数据 */
    cjson_device = cJSON_Parse((char *)json);
    if(cjson_device == NULL)
    {
        //printf("parse fail.\n");
        return RESET;
    }

    /* 依次根据名称提取JSON数据（键值对） */
    cjson_led1 = cJSON_GetObjectItem(cjson_device, "LED1");
		cjson_led2 = cJSON_GetObjectItem(cjson_device, "LED2");
		cjson_beep = cJSON_GetObjectItem(cjson_device, "BEEP");
		cjson_relay = cJSON_GetObjectItem(cjson_device, "RELAY");

//		printf("LED1: %d\n", cjson_led1->valueint);
//		printf("LED1: %d\n", cjson_led1->valueint);
//		printf("LED1: %d\n", cjson_led1->valueint);
//		printf("LED1: %d\n", cjson_led1->valueint);
		if(cjson_led1 != NULL)
		{
				if(cjson_led1->valueint == 0 ) // led1 off 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] & (~LED1_CMD) ;  // 第0位 写0
				}
				else 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] | LED1_CMD    ; // 第0位 写1
				}

		}
		
		if(cjson_led2 != NULL)
		{
				if(cjson_led2->valueint == 0 ) // led2 off 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] & (~LED2_CMD) ;  // 第1位 写0
				}
				else 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] | LED2_CMD    ; // 第1位 写1
				}

		}
		
		if(cjson_beep != NULL)
		{
				if(cjson_beep->valueint == 0 ) // beep off 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] & (~BEEP_CMD) ;  // 第2位 写0
				}
				else 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] | BEEP_CMD    ; // 第2位 写1
				}

		}
		
		if(cjson_relay != NULL)
		{
				if(cjson_relay->valueint == 0 ) // relay off 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] & (~RELAY_CMD) ;  // 第3位 写0
				}
				else 
				{
						REG_HOLD_BUF[0] = REG_HOLD_BUF[0] | RELAY_CMD    ; // 第3位 写1
				}

		}
		
		cJSON_Delete(cjson_device); // 一定要释放内存
		return SET;
}

uint8_t MQTT_Parse_OTAData(uint8_t *json)
{

		cJSON *cjson_device = NULL;
		cJSON *cjson_title = NULL;
		cJSON *cjson_version = NULL;
		cJSON *cjson_size = NULL;
		cJSON *cjson_checksum = NULL;
		
		/* 解析整段JSO数据 */
		cjson_device = cJSON_Parse((char *)json);
		if (cjson_device == NULL)
		{
			//printf("parse fail.\n");
			return RESET;
		}
		else
		{
			// printf("json->%s\n",cJSON_Print(cjson_device));
		}
		/* 依次根据名称提取JSON数据（键值对） */
		cjson_title  = cJSON_GetObjectItem(cjson_device, "fw_title");
		if(cjson_title == NULL )
		{
				return RESET;
		}
		cjson_version  = cJSON_GetObjectItem(cjson_device, "fw_version");
		cjson_size  = cJSON_GetObjectItem(cjson_device, "fw_size");
		cjson_checksum  = cJSON_GetObjectItem(cjson_device, "fw_checksum");
//		printf("fw_title:%s\n",cjson_title->valuestring);
//		printf("fw_version:%s\n",cjson_version->valuestring);
//		printf("fw_size:%d\n",cjson_size->valueint);
//		printf("fw_checksum:%s\n",cjson_checksum->valuestring);
		strcpy(OTA_Info.fw_title,cjson_title->valuestring);
		strcpy(OTA_Info.fw_version,cjson_version->valuestring);
		strcpy(OTA_Info.fw_checksum,cjson_checksum->valuestring);
		OTA_Info.fw_size = cjson_size->valueint;
		
		MQTT_OTA_FLAG = 1; // 可以处理OTA数据标志位 ， 可以从服务器下载数据包
		cJSON_Delete(cjson_device); // 一定要释放内存
		return SET;
}

// 每收到数据包时调用（len=256或最后一包）
void Update_Progress(uint32_t len) {
    OTA_Info.received_bytes += len;
    // 核心代码：计算百分比并显示（无itoa）
    uint8_t percent = (OTA_Info.received_bytes * 100) / OTA_Info.fw_size;
    char progress[20] = { 0 }; // 固定格式"  XX%"
    if(percent > 100 ) percent = 100; 
		if(percent == 100)
		{
				sprintf(progress,"OTA%s        OK",OTA_Info.fw_version);
		}
		else 
		{
				sprintf(progress,"OTA%s      %3d%%",OTA_Info.fw_version,percent);
		}
    OLED_ShowStr(0, 6, (uint8_t*)progress, 2); // 第2行显示（如" 75%")
}

uint8_t MQTT_OTA_GetFW(void)
{
		uint32_t size = OTA_Info.fw_size/256 ; // 一共要传输多少次
		uint8_t msglen = OTA_Info.fw_size%256/4; // 计算包长度
		if(OTA_Info.fw_size%256 != 0 ) // 说明有余数， 需要再次传输1次 
		{
				size++;
		}
		
		CRC32_Init(&crc_ctx);  // 重置CRC
		// 先擦除芯片的储存空间 ， 计算需要擦除多少扇区
		printf("Erase APP2 ...|size=%d\r\n",size);
		/* 擦除App2 */
		Erase_page(Application_2_Addr, Application_Size/1024);  //擦除 Application_Size 页	

		OTA_Info.received_bytes = 0 ; 
		// 或使用时间戳+随机数（更安全）
		OTA_Info.request_id = HAL_GetTick() + (rand() & 0xFFFF); ;
		OTA_Info.chunk_id  = 0 ; 
		OTA_Info.request_bytes = 256; // 默认请求数据为256字节 
		for(uint32_t i =0;i < size ; i++,OTA_Info.chunk_id++)
		{
				char strNumber[10]={0};
				if( (i == size -1) && (OTA_Info.fw_size%256 != 0) ) // 最后1包数据
				{
						OTA_Info.request_bytes = OTA_Info.fw_size%256 ;				
				}
				sprintf(strNumber,"%d",256);
				MQTT_SendData((uint8_t *)PUB3,OTA_Info.request_id,strNumber);
				/************************************************************/
				// 等待服务器返回OTA固件的数据
				uint32_t Timeout = 10*1000; // 超时等待10秒 
				while (OTA_Info.recv_flag == 0) //  没有接收到 服务器下发的数据
				{
					if ((Timeout--) == 0)
						return RESET; // 超时时间到 ， 函数返回
					HAL_Delay(1);
				}
				//printf("Timeout=%d\n",10*1000-Timeout);
				//HAL_Delay(100);
				OTA_Info.recv_flag = 0 ; // 清空标志位 
				
				// 进度百分比实现 
				// 每次收到数据包时调用
				Update_Progress(256);  // 更新进度
				/************************************************************/
				// 把接收的数据写到Flash内 ， 每次写256字节 
				if( (i == size -1) && (OTA_Info.fw_size%256 != 0) ) // 最后1包数据
				{
						if(OTA_Info.fw_size%256%4 != 0) // 有余数 
						{
								msglen ++; // 多增加1包数据
						}	
						//printf("zuihou:i=%d|msglen=%d\n",i,msglen);
						CRC32_Update(&crc_ctx, OTA_Info.recv_buf,OTA_Info.fw_size%256);  // 增量更新CRC
						WriteFlash(Application_2_Addr + i * 256,(uint32_t *)OTA_Info.recv_buf, msglen);
				}
				else 
				{
						CRC32_Update(&crc_ctx, OTA_Info.recv_buf, 256);  // 增量更新CRC
						WriteFlash(Application_2_Addr + i * 256,(uint32_t *)OTA_Info.recv_buf, 256/4);
				}
		}
		// 进行crc32 校验 
		uint32_t crc32 =  CRC32_Final(&crc_ctx,1);
		//printf("client_crc32=%x\n",crc32);
		char  crcstr[10] = {0}; 
		sprintf(crcstr,"%x",crc32);
		//printf("server_crc32=%s\n",OTA_Info.fw_checksum);
		if(strncmp(OTA_Info.fw_checksum,crcstr,strlen(OTA_Info.fw_checksum)) == 0 ) 
		{
				MQTT_SendData((uint8_t *)PUB1,2,NULL); // 成功
				uint32_t update_flag = 0xAAAAAAAA; // 升级固件标志位
				WriteFlash((Application_2_Addr + Application_Size - 4), &update_flag, 1);
				HAL_NVIC_SystemReset(); // 重启系统
		}
		else 
		{
				MQTT_SendData((uint8_t *)PUB1,3,NULL); // 3 表示失败 
				OLED_ShowStr(64+16, 6, (uint8_t*)"  Fail", 2); // 第2行显示（如" 75%")
				MQTT_OTA_FLAG = 1; // // 失败后继续重新下载固件 , 可以处理OTA数据标志位 . 
				return RESET ; 
		}
		
		return SET;

}





uint8_t MQTT_Connect_Server(void)
{
			uint8_t ret;
			uint8_t buf[128]={0};	
			/**************************---1---************************************/
			WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
			strcpy((char *)Parse_Substr,"OK\r\n");	 // 命令成功的返回值 
			sprintf((char *)buf,"AT+MQMULTEN=1\r\n"); // 使能多通道订阅与发布主题
			HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
			if(Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)  {
					return RESET; }
			
			/**************************---2---***********************************/
			WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
			//strcpy((char *)Parse_Substr,"OK\r\n");	// 命令成功的返回值 	
			sprintf((char *)buf,"AT+MQTTFILTER=0\r\n");  //不过滤任何主题
			HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
			if(Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)  {
					return RESET; }
			
			/***************************---3---**********************************/
			WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
			//strcpy((char *)Parse_Substr,"OK\r\n");	// 命令成功的返回值 	
			// 订阅服务器 下行主题1
			sprintf((char *)buf,"AT+MQSUBM=0,1,0,4,\"%s\"\r\n",SUB1); 
			HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
			if(Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)  {
					return RESET; }
			
			/***************************---4---**********************************/
			WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
			//strcpy((char *)Parse_Substr,"OK\r\n");	// 命令成功的返回值 	
			// 订阅服务器 下行主题2
			sprintf((char *)buf,"AT+MQSUBM=1,1,0,4,\"%s\"\r\n",SUB2); 
			HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
			if(Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)  {
					return RESET; }
			
			/***************************---5---**********************************/
			WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
			//strcpy((char *)Parse_Substr,"OK\r\n");	// 命令成功的返回值 	
			// 订阅服务器 下行主题2
			sprintf((char *)buf,"AT+MQSUBM=2,1,0,4,\"%s\"\r\n",SUB3); 
			HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
			if(Test_WIFI4G_CMD_Status(1000) == WIFI4G_ERROR)  {
					return RESET; }
					
			/***************************---6---**********************************/
			WIFI4G_CMD_Status = WIFI4G_NOT ;	 // 初始化标志位 
			strcpy((char *)Parse_Substr,"MQTT_CONNECT:");	 // 服务器连接功的返回值 		
			// 复位4G模组, 复位后自动连接服务器
			sprintf((char *)buf,"AT+REST\r\n");   
			HAL_UART_Transmit(&huart3,buf,strlen((char *)buf),1000);
			ret = Test_WIFI4G_CMD_Status(1000*180) ;  // 延时180秒 
			if (ret == WIFI4G_OK)
			{
					OLED_ShowStr(0,3,(unsigned char *)"MQTTServer... OK",2);	//测试8*16字符
					return SET;
			}
			else 
			{
					OLED_ShowStr(0,3,(unsigned char *)"MQTTServer... --",2);	//测试8*16字符
					return  RESET; 
			}

}


