#ifndef __IR_SENSOR_H
#define __IR_SENSOR_H

#include "stm32f1xx_hal.h"

// 红外传感器引脚配置
#define IR_SENSOR_PORT GPIOB
#define IR_SENSOR_PIN  GPIO_PIN_3

// 函数声明
void IR_Sensor_Init(void);
uint8_t IR_Sensor_Read(void);

#endif
