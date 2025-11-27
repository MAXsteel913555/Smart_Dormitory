#include "ir_sensor.h"

// 初始化红外传感器
void IR_Sensor_Init(void) {
    // 在CubeMX中配置为输入模式即可
    // 通常使用上拉输入，因为HC-SR501输出高电平表示检测到人
}

// 读取红外传感器状态
uint8_t IR_Sensor_Read(void) {
    return HAL_GPIO_ReadPin(IR_SENSOR_PORT, IR_SENSOR_PIN);
}
