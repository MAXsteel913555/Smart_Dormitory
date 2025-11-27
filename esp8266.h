#ifndef ESP8266_H
#define ESP8266_H

#include "stm32f1xx_hal.h" 
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"
#include "oled.h"



void esp8266_run(void);
void uart_print(UART_HandleTypeDef *huart,char* format,...);
void uart_process(void);
void extract_mqtt_info(const char *str, char *info1, char *info_b);


#endif

