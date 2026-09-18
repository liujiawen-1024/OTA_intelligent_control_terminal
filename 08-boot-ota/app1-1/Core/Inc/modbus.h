#ifndef __MODBUS_H__
#define __MODBUS_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define      LED1_CMD    (1<<0)
#define      LED2_CMD    (1<<1)
#define      BEEP_CMD    (1<<2)
#define      RELAY_CMD   (1<<3)

extern __IO uint8_t TIM1_Timeout_Flag  ;  
extern uint16_t REG_HOLD_BUF[];
extern __IO uint8_t Modify_SlaveAdress_Flag ; // 修改从机地址标志位

void Modbus_Parse(void);

#ifdef __cplusplus
}
#endif

#endif /* __MODBUS_H__ */

