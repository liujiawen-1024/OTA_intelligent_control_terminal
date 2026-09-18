#ifndef __MQTT_H__
#define __MQTT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern __IO uint8_t MQTT_UPLoad_Flag;
extern __IO uint8_t MQTT_Download_Flag;

uint8_t ESP8266_Connect_MQTTServer(void);
uint8_t MQTT_SendData(void);
uint8_t MQTT_Parse_JsonData(uint8_t *json);

#ifdef __cplusplus
}
#endif

#endif /* __MQTT_H__ */

