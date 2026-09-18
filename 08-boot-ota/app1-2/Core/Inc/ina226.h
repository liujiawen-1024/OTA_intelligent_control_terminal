#ifndef __INA226_H_
#define __INA226_H_
 
#include "main.h"
#include "i2c.h"
#include "oled.h"
 
#define INA226_ADDR                 0x82   // 地址

//写配置寄存器
//0100_010_100_100_111 //16次平均,1.1ms,1.1ms,连续测量分流电压和总线电压
//0100 0101 0010 0111
// 4    5    2    7

//0100_011_111_100_111 //64次平均,8.2ms,8.2ms,连续测量分流电压和总线电压
//0100 0111 1111 1111
// 4    7    f    f

//#define Configuration_Register_Init 0x4527 
#define Configuration_Register_Init 0x47ff 

//写校准寄存器
//LSB选择0.1mA,分压电阻选0.01R Cal=0.00512/(0.1mA*0.01R) * 1000 =5120
#define Calibration_Register_Init   5120
 
#define Configuration_Register      0x00 // 配置寄存器
#define Shunt_Voltage_Register      0x01 // 电压降寄存器
#define Bus_Voltage_Register        0x02 // 总线电压寄存器
#define Power_Register              0x03 // 功率寄存器
#define Current_Register            0x04 // 电流寄存器
#define Calibration_Register        0x05 // 校准寄存器
 
#define Shunt_Voltage_Register_LSB  2.5f   // 固定值
#define Bus_Voltage_Register_LSB    1.25f  // 固定值
#define Current_Register_LSB        0.1f  // 电流测量最小单位
#define Power_Register_LSB          (0.1f * 25) // 功率测量最小单位
 
void INA226_Init(void);
uint16_t INA226_Read_Bus_Voltage(void);
uint16_t INA226_Read_Current(void);
uint16_t INA226_Read_Pow(void);
void INA226_Test(void);
void INA226_Read(void);
 
#endif //  _INA226_H_
