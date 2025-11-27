#include "display.h"
#include "common.h"

// 全局变量定义（在main.c中定义，这里声明外部引用）
extern MenuState_t current_menu;
extern uint8_t current_selection;
extern float temp_threshold;
extern float humid_threshold;
extern float temperature;
extern DHT11_Data dht11_data; 

void Display_Current_Screen(void)
{
    switch(current_menu) {
        case MENU_MAIN:
            Display_Main_Menu();
            break;
        case MENU_TEMP_SET:
            Display_Temp_Setting();
            break;
        case MENU_HUMID_SET:
            Display_Humid_Setting();
            break;
        case MENU_SMOKE_SET:
            Display_Smoke_Setting();
            break;
    }
}

void Display_Main_Menu(void)
{
    char message[50];
//    DHT11_Data dht11_data;
//    float temperature;
    uint8_t person_detected;
    float smoke_level;

    uint8_t dht11_status = DHT11_ReadData(&dht11_data);
    uint8_t ds18b20_status = DS18B20_ReadTemp(&temperature);
    person_detected = IR_Sensor_Read();
    smoke_level = MQ2_Get_Smoke_Level();

    OLED_NewFrame();

    // 第1行：温度 (Temperature)
    if(ds18b20_status == 0) {

        sprintf(message,"温度:%.1fC", temperature);
    } else {
        sprintf(message, "温度: --.-C");
    }
    if(current_selection == 0) {
        OLED_PrintString(0, 0, ">", &font15x15, OLED_COLOR_NORMAL);
        OLED_PrintString(10, 0, message, &font15x15, OLED_COLOR_NORMAL);
    } else {
        OLED_PrintString(0, 0, message, &font15x15, OLED_COLOR_NORMAL);
    }

    // 第2行：湿度 (Humidity)
    if(dht11_status == 0) {

        sprintf(message, "湿度: %d.%d%%", dht11_data.humidity_int, dht11_data.humidity_deci);
    } else {
        sprintf(message, "湿度: --.-%%");
    }
    if(current_selection == 1) {
        OLED_PrintString(0, 15, ">", &font15x15, OLED_COLOR_NORMAL);
        OLED_PrintString(10, 15, message, &font15x15, OLED_COLOR_NORMAL);
    } else {
        OLED_PrintString(0, 15, message, &font15x15, OLED_COLOR_NORMAL);
    }

    // 第3行：红外 (Infrared)
    if(person_detected) {

        sprintf(message, "红外: 无人");
    } else {
        sprintf(message, "红外: 有人");
    }
    if(current_selection == 2) {
        OLED_PrintString(0, 32, ">", &font15x15, OLED_COLOR_NORMAL);
        OLED_PrintString(10, 32, message, &font15x15, OLED_COLOR_NORMAL);
    } else {
        OLED_PrintString(0, 32, message, &font15x15, OLED_COLOR_NORMAL);
    }

    // 第4行：烟雾 (Smoke)
    if(smoke_level >= 0 && smoke_level <= 100) {
        sprintf(message, "烟雾: %.0f%%", smoke_level);
    } else {
        sprintf(message, "烟雾: --%%");
    }
    if(current_selection == 3) {
        OLED_PrintString(0, 50, ">", &font15x15, OLED_COLOR_NORMAL);
        OLED_PrintString(10, 50, message, &font15x15, OLED_COLOR_NORMAL);
    } else {
        OLED_PrintString(0, 50, message, &font15x15, OLED_COLOR_NORMAL);
    }

    OLED_ShowFrame();
}

void Display_Temp_Setting(void)
{
    char message[50];
    OLED_NewFrame();


    OLED_PrintString(35, 0, "温度设置", &font15x15, OLED_COLOR_NORMAL);

    // 当前阈值
    sprintf(message, "Threshold: %.1fC", temp_threshold);
    OLED_PrintString(0, 25, message, &font15x15, OLED_COLOR_NORMAL);

    OLED_ShowFrame();
}

void Display_Humid_Setting(void)
{
    char message[50];
    OLED_NewFrame();


    OLED_PrintString(35, 0, "湿度设置", &font15x15, OLED_COLOR_NORMAL);

    // 当前阈值
    sprintf(message, "Threshold: %.0f%%", humid_threshold);
    OLED_PrintString(10, 25, message, &font15x15, OLED_COLOR_NORMAL);

    OLED_ShowFrame();
}

void Display_Smoke_Setting(void)
{
    char message[50];
    OLED_NewFrame();


    OLED_PrintString(35, 0, "烟雾设置", &font15x15, OLED_COLOR_NORMAL);

    // 当前阈值
    sprintf(message, "Threshold: %.0f%%", smoke_threshold);
    OLED_PrintString(10, 25, message, &font15x15, OLED_COLOR_NORMAL);

    OLED_ShowFrame();
}
