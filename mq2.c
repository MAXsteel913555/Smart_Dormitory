#include "mq2.h"
#include <math.h>

// 外部ADC声明
extern ADC_HandleTypeDef hadc1;

// 私有变量
static float R0 = 10.0f;

/**
  * @brief  初始化MQ2传感器
  */
void MQ2_Init(void)
{
    // 预热传感器
    HAL_Delay(100);
}

/**
  * @brief  读取ADC原始值
  */
float MQ2_Read_ADC_Raw(void)
{
    uint32_t adc_value = 0;
    ADC_ChannelConfTypeDef sConfig = {0};

    // 配置ADC通道
    sConfig.Channel = MQ2_ADC_CHANNEL;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&MQ2_ADC_HANDLE, &sConfig);

    // 启动ADC转换
    HAL_ADC_Start(&MQ2_ADC_HANDLE);
    if (HAL_ADC_PollForConversion(&MQ2_ADC_HANDLE, 100) == HAL_OK) {
        adc_value = HAL_ADC_GetValue(&MQ2_ADC_HANDLE);
    }
    HAL_ADC_Stop(&MQ2_ADC_HANDLE);

    return (float)adc_value;
}

/**
  * @brief  读取ADC电压值
  */
float MQ2_Read_ADC_Voltage(void)
{
    float adc_raw = MQ2_Read_ADC_Raw();
    return (adc_raw * 3.3f) / 4095.0f;
}

/**
  * @brief  计算传感器电阻Rs
  */
float MQ2_Calculate_RS(void)
{
    float voltage = MQ2_Read_ADC_Voltage();
    // RL = 5KΩ负载电阻
    float rs = (3.3f - voltage) / voltage * 5.0f;
    return rs;
}

/**
  * @brief  校准传感器
  */
uint8_t MQ2_Calibrate(void)
{
    float rs_sum = 0.0f;
    uint8_t valid_samples = 0;

    for (int i = 0; i < MQ2_CALIBRATION_SAMPLES; i++) {
        float rs = MQ2_Calculate_RS();
        if (rs > 0.1f && rs < 100.0f) {
            rs_sum += rs;
            valid_samples++;
        }
        HAL_Delay(100);
    }

    if (valid_samples > 0) {
        float rs_avg = rs_sum / valid_samples;
        R0 = rs_avg / MQ2_CLEAN_AIR_RS_RO;
        return 1;
    }

    return 0;
}

/**
  * @brief  获取Rs/R0比值
  */
float MQ2_Get_Rs_R0_Ratio(void)
{
    float rs = MQ2_Calculate_RS();
    return rs / R0;
}

/**
  * @brief  获取烟雾浓度等级
  */
float MQ2_Get_Smoke_Level(void)
{
    float voltage = MQ2_Read_ADC_Voltage();

    // 基于电压的简单算法
    // 电压越高，烟雾浓度越高
    float smoke_level = 0.0f;

    if (voltage < 0.5f) {
        smoke_level = 0.0f;           // 无烟雾
    } else if (voltage < 1.0f) {
        smoke_level = (voltage - 0.5f) * 40.0f;  // 0-20%
    } else if (voltage < 1.5f) {
        smoke_level = 20.0f + (voltage - 1.0f) * 40.0f; // 20-40%
    } else if (voltage < 2.0f) {
        smoke_level = 40.0f + (voltage - 1.5f) * 40.0f; // 40-60%
    } else if (voltage < 2.5f) {
        smoke_level = 60.0f + (voltage - 2.0f) * 40.0f; // 60-80%
    } else {
        smoke_level = 80.0f + (voltage - 2.5f) * 40.0f; // 80-100%
    }

    if (smoke_level > 100.0f) smoke_level = 100.0f;
    if (smoke_level < 0.0f) smoke_level = 0.0f;

    return smoke_level;
}

/**
  * @brief  获取气体浓度
  */
float MQ2_Get_Gas_Level(void)
{
    float voltage = MQ2_Read_ADC_Voltage();

    // 简化PPM计算
    float ppm = voltage * 1000.0f;  // 每伏特对应1000PPM

    if (ppm > 3000.0f) ppm = 3000.0f;

    return ppm;
}
