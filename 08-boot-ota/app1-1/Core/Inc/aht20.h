#ifndef __AHT20_H
#define __AHT20_H

#include <stdint.h>

#define ATH20_SLAVE_ADDRESS		0x70		/* I2C从机地址  (0x38<<1) */

//****************************************
// 定义 AHT20 内部地址
//****************************************
#define	AHT20_STATUS_REG			0x00	//状态字 寄存器地址
#define	AHT20_INIT_REG				0xBE	//初始化 寄存器地址
#define	AHT20_TrigMeasure_REG	0xAC	//触发测量 寄存器地址

uint8_t AHT20_Read_Status(void);
void AHT20_SendAC(void);
void AHT20_Init(void);
void AHT20_Read_CTdata(uint32_t *ct);
void AHT20_Test(void);
void AHT20_Read(void);

#endif
