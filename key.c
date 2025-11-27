#include "key.h"
#include "common.h"

// 全局变量定义（在main.c中定义，这里声明外部引用）
extern MenuState_t current_menu;
extern uint8_t current_selection;
extern float temp_threshold;
extern float humid_threshold;

KeyState_t Get_Key(void)
{
    static uint32_t last_time = 0;
    uint32_t current_time = HAL_GetTick();

    // 按键消抖
    if(current_time - last_time < 20) {
        return KEY_NONE;
    }

    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET) {
        last_time = current_time;
        return KEY_UP;
    }
    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET) {
        last_time = current_time;
        return KEY_DOWN;
    }
    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET) {
        last_time = current_time;
        return KEY_SURE;
    }
    else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_RESET) {
        last_time = current_time;
        return KEY_BACK;
    }

    return KEY_NONE;
}

void Key_Handler(KeyState_t key)
{
    if(key == KEY_NONE) return;

    switch(current_menu) {
        case MENU_MAIN:
            Handle_Main_Menu(key);
            break;
        case MENU_TEMP_SET:
            Handle_Temp_Setting(key);
            break;
        case MENU_HUMID_SET:
            Handle_Humid_Setting(key);
            break;
        case MENU_SMOKE_SET:
            Handle_Smoke_Setting(key);
            break;
    }
}

void Handle_Main_Menu(KeyState_t key)
{
    switch(key) {
		case KEY_UP:
			// 向上移动选择（循环）
			if(current_selection == 0) {
				current_selection = 3;  // 从顶部循环到底部
			} else {
				current_selection--;
			}
			break;

		case KEY_DOWN:
			// 向下移动选择（循环）
			if(current_selection == 3) {
				current_selection = 0;  // 从底部循环到顶部
			} else {
				current_selection++;
			}
			break;

		case KEY_SURE:
            // 确认进入设置界面（只有温度和湿度可以进入）
            if(current_selection == 0) {
                // 进入温度设置
                current_menu = MENU_TEMP_SET;
            } else if(current_selection == 1) {
                // 进入湿度设置
                current_menu = MENU_HUMID_SET;
            }else if(current_selection == 3){
            	// 进入烟雾设置
            	current_menu = MENU_SMOKE_SET;
            }

            break;

		case KEY_BACK:
            // 主界面按返回无反应
            break;

		case KEY_NONE:
            // 无按键，不做任何处理
            break;
    }
}

void Handle_Temp_Setting(KeyState_t key)
{
    switch(key) {
        case KEY_UP:
            // 温度增加
            temp_threshold += 0.5f;
            break;

        case KEY_DOWN:
            // 温度减少
            temp_threshold -= 0.5f;
            if(temp_threshold < 0) temp_threshold = 0;
            break;

        case KEY_SURE:
            // 确认修改并返回主界面
            current_menu = MENU_MAIN;
            break;

        case KEY_BACK:
            // 返回主界面
            current_menu = MENU_MAIN;
            break;

        case KEY_NONE:
            // 无按键，不做任何处理
            break;
    }
}

void Handle_Humid_Setting(KeyState_t key)
{
    switch(key) {
        case KEY_UP:
            // 湿度增加
            humid_threshold += 1.0f;
            break;

        case KEY_DOWN:
            // 湿度减少
            humid_threshold -= 1.0f;
            if(humid_threshold < 0) humid_threshold = 0;
            break;

        case KEY_SURE:
            // 确认修改并返回主界面
            current_menu = MENU_MAIN;
            break;

        case KEY_BACK:
            // 返回主界面
            current_menu = MENU_MAIN;
            break;

        case KEY_NONE:
            // 无按键，不做任何处理
            break;
    }
}

void Handle_Smoke_Setting(KeyState_t key)
{
    switch(key) {
        case KEY_UP:
            // 烟雾增加
        	smoke_threshold += 1.0f;
            break;

        case KEY_DOWN:
            // 烟雾减少
        	smoke_threshold -= 1.0f;
            if(smoke_threshold < 0) smoke_threshold = 0;
            break;

        case KEY_SURE:
            // 确认修改并返回主界面
            current_menu = MENU_MAIN;
            break;

        case KEY_BACK:
            // 返回主界面
            current_menu = MENU_MAIN;
            break;

        case KEY_NONE:
            // 无按键，不做任何处理
            break;
    }
}
