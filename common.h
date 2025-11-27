#ifndef __COMMON_H
#define __COMMON_H

#include "main.h"
#include "oled.h"
#include "dht11.h"
#include "ds18b20.h"
#include "string.h"
#include "stdio.h"
#include "ir_sensor.h"
#include "mq2.h"
#include "esp8266.h"

// 菜单状态枚举
typedef enum {
    MENU_MAIN = 0,      // 主界面
    MENU_TEMP_SET,      // 温度设置
    MENU_HUMID_SET,     // 湿度设置
	MENU_SMOKE_SET        // 烟雾设置
} MenuState_t;

// 按键状态枚举
typedef enum {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_SURE,
    KEY_BACK
} KeyState_t;

// 全局变量声明
extern MenuState_t current_menu;
extern uint8_t current_selection;
extern float temp_threshold;   // 温度报警阈值
extern float humid_threshold;  // 湿度报警阈值
extern float smoke_threshold;  // 烟雾报警阈值

// 函数声明
KeyState_t Get_Key(void);
void Key_Handler(KeyState_t key);
void Handle_Main_Menu(KeyState_t key);
void Handle_Temp_Setting(KeyState_t key);
void Handle_Humid_Setting(KeyState_t key);
void Display_Current_Screen(void);
void Display_Main_Menu(void);
void Display_Temp_Setting(void);
void Display_Humid_Setting(void);
void Update_Outputs(void);

#endif
