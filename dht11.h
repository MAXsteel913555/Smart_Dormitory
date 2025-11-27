#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f1xx_hal.h"

typedef struct {
    uint8_t humidity_int;
    uint8_t humidity_deci;
    uint8_t temp_int;
    uint8_t temp_deci;
    uint8_t checksum;
} DHT11_Data;


#define DHT11_PORT GPIOA
#define DHT11_PIN  GPIO_PIN_1

// 函数声明
uint8_t DHT11_Init(void);
uint8_t DHT11_ReadData(DHT11_Data *data);
void DHT11_Delay_us(uint16_t us);

#endif
