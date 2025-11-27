#include "esp8266.h"
#include "oled.h"
#include "display.h"
#include "key.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdarg.h>
#include "common.h"
#include "gpio.h" 

extern float temperature;
extern DHT11_Data dht11_data;
extern float temp_threshold;
extern float humid_threshold;
uint8_t person_detected = 0;
uint8_t person_detected1;

// UART接收相关变量
uint32_t uart_time = 0;
uint8_t uart_index = 0;
uint8_t uart_buffer[512] = {0};
extern UART_HandleTypeDef huart3; // 确保 huart3 在其他地方正确定义

// MQTT连接信息
char url[] = "mqtts.heclouds.com";
char device_name[] = "esp-01s";
char produce_ID[] = "O8FQdif3j4";
char subscribe_topic[] = "$sys/O8FQdif3j4/esp-01s/thing/property/post/reply";
char publish_topic[] = "$sys/O8FQdif3j4/esp-01s/thing/property/post";
char token[] = "version=2018-10-31&res=products%%2FO8FQdif3j4%%2Fdevices%%2Fesp-01s&et=2204630406&method=md5&sign=XxMBsJWp2la17gNvjEpUAw%%3D%%3D";

void Update_Outputs(void)
{
    DHT11_Data dht11_data;
    float temperature;
    static uint8_t person_beeped = 0;
    static uint8_t temp_humid_beeped = 0;
    
    // 读取传感器数据
    uint8_t dht11_status = DHT11_ReadData(&dht11_data);
    uint8_t ds18b20_status = DS18B20_ReadTemp(&temperature);


    // 温湿度任意一个超过(动态)阈值就触发报警
    if((ds18b20_status == 0 && temperature > temp_threshold) ||
           (dht11_status == 0 && dht11_data.humidity_int > humid_threshold)) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);  // 打开继电器

            if(!temp_humid_beeped) {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);  // 打开蜂鸣器
                HAL_Delay(300);
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);    // 关闭蜂鸣器
                temp_humid_beeped = 1;
            }
        } else {  // 检测到无人
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);    // 关闭继电器
            temp_humid_beeped = 0;  // 重置，下次超标时再响
        }

    // 人员检测控制
    if(HAL_GPIO_ReadPin(IR_SENSOR_PORT, IR_SENSOR_PIN) == 0) {  // 检测到有人
           HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);  // 开启继电器
		   if(!person_beeped) {
			   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);  // 打开蜂鸣器
			   HAL_Delay(300);
			   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);    // 关闭蜂鸣器
			   person_beeped = 1;
		   }
   } else {  // 检测到无人
	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);    // 关闭继电器
	   person_beeped = 0;  // 重置，下次有人时再响
   }

}
/**
 * @brief 格式化打印函数，通过UART发送
 */
void uart_print(UART_HandleTypeDef *huart, char* format, ...) {
    char buf[512] = {0};
    va_list ap;
    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);
    HAL_UART_Transmit(huart, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
}

/**
 * @brief ESP8266初始化和MQTT连接主函数
 */
void esp8266_run() {
    // 初始化OLED
    OLED_Init();
    OLED_DisPlay_On(); // 开启显示
    // 初始化UART接收中断
    uart_index = 0;
    memset(uart_buffer, 0, sizeof(uart_buffer));
    HAL_UART_Receive_IT(&huart3, &uart_buffer[uart_index], 1);

    // --- 连接过程显示 ---
    OLED_NewFrame();
    OLED_PrintString(0, 0, "ESP8266", &font15x15, OLED_COLOR_NORMAL);
		OLED_PrintString(0, 15, "Connecting...", &font15x15, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    HAL_Delay(1000);

    // 1. 设置WiFi模式
    OLED_NewFrame();
    OLED_PrintString(0, 0, "1.Set CWMODE=1", &font15x15, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    uart_print(&huart3, "AT+CWMODE=1\r\n");
    HAL_Delay(1000);

    // 2. 连接WiFi
    OLED_NewFrame();
    OLED_PrintString(0, 0, "2.Connect WiFi", &font15x15, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    uart_print(&huart3, "AT+CWJAP=\"HONOR\",\"smh19801962526\"\r\n");
    HAL_Delay(5000); // WiFi连接可能需要几秒钟

    // 3. 配置MQTT用户信息
    OLED_NewFrame();
    OLED_PrintString(0, 0, "3.MQTT User Cfg", &font15x15, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    uart_print(&huart3, "AT+MQTTUSERCFG=0,1,\"esp-01s\",\"O8FQdif3j4\",\"version=2018-10-31&res=products%%2FO8FQdif3j4%%2Fdevices%%2Fesp-01s&et=2204630406&method=md5&sign=XxMBsJWp2la17gNvjEpUAw%%3D%%3D\",0,0,\"\"\r\n");
    HAL_Delay(3000);

    // 4. 连接MQTT服务器
    OLED_NewFrame();
    OLED_PrintString(0, 0, "4.MQTT Connect", &font15x15, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    // 注意：mqtts 是加密连接，端口通常是 8883。如果你的模块不支持SSL，可能需要用 mqtt 和 1883
    uart_print(&huart3, "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n");
    HAL_Delay(5000);

    // 5. 订阅主题
    OLED_NewFrame();
    OLED_PrintString(0, 0, "5.Subscribe", &font15x15, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    uart_print(&huart3, "AT+MQTTSUB=0,\"%s\",0\r\n", subscribe_topic);
    HAL_Delay(2000);

    // --- 连接成功 ---
    OLED_NewFrame();
    OLED_PrintString(0, 0, "Connect Success!", &font15x15, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    HAL_Delay(1500);

    // --- 进入主循环 ---
    uint32_t publish_counter = 0; // 发布计数器
    uint8_t biaozhi = 0;          // 标志位，防止在2秒内重复发布

    while(1) {
			    
    KeyState_t key = Get_Key();//获取按键状态
    Key_Handler(key);//处理按键

    Display_Current_Screen(); //显示当前界面
	  Update_Outputs();// 5. 根据阈值控制输出
			

        // 2. MQTT定时发布逻辑 (每2秒一次)
        if(uwTick / 1000 % 2 == 0) { // 判断是否为偶数秒
            if(biaozhi == 0) {
                biaozhi = 1; // 置位标志，确保本次2秒内只执行一次

                // 在发布前，可以先更新传感器数据
                // ... (这里可以添加读取DHT11, DS18B20等传感器的代码)

                // 发布数据时，在OLED上给出提示
//                OLED_NewFrame();
//                OLED_PrintString(0, 0, "Publishing...", &font15x15, OLED_COLOR_NORMAL);
//                OLED_ShowFrame();

                // 执行MQTT发布
                publish_counter++;
                uart_print(&huart3,"AT+MQTTPUB=0,\"$sys/O8FQdif3j4/esp-01s/thing/property/post\",\"{\\\"id\\\":\\\"456\\\"\\,\\\"params\\\":{\\\"Power\\\":{\\\"value\\\":%d}\\,\\\"temp\\\":{\\\"value\\\":%.1f}\\,\\\"daxiao\\\":{\\\"value\\\":%d.%d}\\}}}\",0,0\r\n",person_detected1,temperature,dht11_data.humidity_int,dht11_data.humidity_deci);	

                // 短暂延时，确保发布指令发送完成
                HAL_Delay(100);
            }
        } else {
            biaozhi = 0; // 奇数秒时，复位标志，为下一次发布做准备
        }

        // 3. 更新并显示当前屏幕（主菜单或设置界面）
        Display_Current_Screen();

        // 4. 短暂延时，降低CPU占用率
        HAL_Delay(100);
				Update_Outputs();
				person_detected = IR_Sensor_Read();
				person_detected1 = (person_detected == 1) ? 0 : 1;
					
    }
}

/**
 * @brief UART接收完成回调函数
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if(huart == &huart3){
        uart_time = uwTick; // 更新接收时间戳

        // 检查缓冲区是否已满
        if(uart_index < sizeof(uart_buffer) - 1){
            uart_index++;
            // 继续接收下一个字节
            HAL_UART_Receive_IT(&huart3, &uart_buffer[uart_index], 1);
        } else {
            // 缓冲区满，重置
            uart_index = 0;
            memset(uart_buffer, 0, sizeof(uart_buffer));
            HAL_UART_Receive_IT(&huart3, &uart_buffer[uart_index], 1);
        }
    }
}

/**
 * @brief UART数据处理函数 (目前为空，可以根据需要扩展)
 * 例如，可以在这里解析ESP8266返回的 "+MQTTRX" 消息
 */
void uart_process(void){
    // 你可以在这里添加对 uart_buffer 中数据的解析逻辑
    // if(uart_index > 0 && uwTick > uart_time + 100){
    //     // 处理接收到的数据...
    //     memset(uart_buffer, 0, sizeof(uart_buffer));
    //     uart_index = 0;
    // }
}

/**
 * @brief (可选) MQTT消息提取函数 (如果需要在esp8266.c中处理消息)
 */
void extract_mqtt_info(const char *str, char *info1, char *info_b) {
    // 函数实现保持不变
    const char *p1 = strchr(str, '"');
    if (!p1) return;
    const char *p2 = strchr(p1 + 1, '"');
    if (!p2) return;

    size_t len1 = p2 - p1 - 1;
    strncpy(info1, p1 + 1, len1);
    info1[len1] = '\0';

    const char *p_comma = strchr(p2 + 1, ',');
    if (!p_comma) return;
    int length = atoi(p_comma + 1);

    const char *msg_start = strchr(p_comma + 1, ',');
    if (!msg_start) return;
    msg_start++;

    strncpy(info_b, msg_start, length);
    info_b[length] = '\0';
}
