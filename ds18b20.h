#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f1xx_hal.h"

// DS18B20引脚定义（PA15）
#define DS18B20_DQ_PORT    GPIOA
#define DS18B20_DQ_PIN     GPIO_PIN_15

// 引脚方向切换宏定义
#define DS18B20_DQ_OUT()   {DS18B20_DQ_PORT->CRH &= 0xFFFF0FFF; DS18B20_DQ_PORT->CRH |= 0x00003000;} // 推挽输出
#define DS18B20_DQ_IN()    {DS18B20_DQ_PORT->CRH &= 0xFFFF0FFF; DS18B20_DQ_PORT->CRH |= 0x00004000;} // 浮空输入

// 引脚电平操作宏定义
#define DS18B20_DQ_HIGH()  HAL_GPIO_WritePin(DS18B20_DQ_PORT, DS18B20_DQ_PIN, GPIO_PIN_SET)
#define DS18B20_DQ_LOW()   HAL_GPIO_WritePin(DS18B20_DQ_PORT, DS18B20_DQ_PIN, GPIO_PIN_RESET)
#define DS18B20_DQ_READ()  HAL_GPIO_ReadPin(DS18B20_DQ_PORT, DS18B20_DQ_PIN)

uint8_t DS18B20_Init(void);                  // 初始化DS18B20
uint8_t DS18B20_ReadTemp(float *temp);       // 读取温度（返回0成功，1失败）

#endif
