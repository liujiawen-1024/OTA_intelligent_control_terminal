#ifndef __WIFI4G_H
#define __WIFI4G_H
#include <stdint.h>
#include "main.h"
#include "fifo.h"

enum
{
  WIFI4G_NOT = 0, // Not found
  WIFI4G_OK,
  WIFI4G_ERROR
};

extern __IO uint8_t WIFI4G_CMD_Status; // OK状态标志位

extern uint8_t Parse_Substr[];

uint8_t Test_WIFI4G_CMD_Status(uint32_t Timeout);
uint8_t WIFI4G_Parse_Queue(sequeue_t *sq);
uint8_t ESP8266_Connect_WIFI(void);
void ESP8266_Init(void);
void ESP8266_Test(void);

#endif
