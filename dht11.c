#include "dht11.h"
#include "stm32f1xx_hal_tim.h"

DHT11_Data dht11_data;

// 添加外部声明
extern TIM_HandleTypeDef htim3;

// 微秒级延时函数
void DHT11_Delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    HAL_TIM_Base_Start(&htim3);
    while (__HAL_TIM_GET_COUNTER(&htim3) < us);
    HAL_TIM_Base_Stop(&htim3);
}

// 设置GPIO为输出模式
static void DHT11_Set_Output(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

// 设置GPIO为输入模式
static void DHT11_Set_Input(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

// 初始化DHT11
uint8_t DHT11_Init(void) {
    DHT11_Set_Output();
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    HAL_Delay(1000);  // 上电后等待1s稳定
    return 0;
}

// 读取一个字节
static uint8_t DHT11_Read_Byte(void) {
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        while (!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));  // 等待低电平结束
        DHT11_Delay_us(40);  // 延时40us判断高低电平

        if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) {
            byte |= (1 << (7 - i));  // 高位在前
            while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));  // 等待高电平结束
        }
    }
    return byte;
}

// 读取温湿度数据
uint8_t DHT11_ReadData(DHT11_Data *data) {
    uint8_t buf[5] = {0};

    // 发送开始信号
    DHT11_Set_Output();
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    HAL_Delay(18);  // 至少18ms低电平
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    DHT11_Delay_us(30);  // 主机拉高20-40us

    // 等待DHT11响应
    DHT11_Set_Input();
    if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) return 1;  // 无响应

    while (!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));  // 等待80us低电平结束
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));   // 等待80us高电平结束

    // 读取40位数据
    for (uint8_t i = 0; i < 5; i++) {
        buf[i] = DHT11_Read_Byte();
    }

    // 校验数据
    if (buf[0] + buf[1] + buf[2] + buf[3] == buf[4]) {
        data->humidity_int = buf[0];
        data->humidity_deci = buf[1];
        data->temp_int = buf[2];
        data->temp_deci = buf[3];
        data->checksum = buf[4];
        return 0;  // 读取成功
    }
    return 1;  // 校验失败
}
