#ifndef __MQ2_H
#define __MQ2_H

#include "stm32f1xx_hal.h"

// ADC通道定义
#define MQ2_ADC_HANDLE    hadc1
#define MQ2_ADC_CHANNEL   ADC_CHANNEL_0

// 校准参数
#define MQ2_CLEAN_AIR_RS_RO 9.8f
#define MQ2_CALIBRATION_SAMPLES 50

// 函数声明
void MQ2_Init(void);
float MQ2_Read_ADC_Voltage(void);
float MQ2_Read_ADC_Raw(void);
float MQ2_Calculate_RS(void);
float MQ2_Calculate_R0(void);
float MQ2_Get_Rs_R0_Ratio(void);
float MQ2_Get_Smoke_Level(void);
float MQ2_Get_Gas_Level(void);
uint8_t MQ2_Calibrate(void);

#endif
