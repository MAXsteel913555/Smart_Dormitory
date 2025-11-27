#include "ds18b20.h"


// 微秒级延时
static void DS18B20_DelayUs(uint16_t us)
{
    uint32_t ticks = us * (SystemCoreClock / 1000000) / 8;
    while(ticks--);
}

// 发送复位脉冲
static uint8_t DS18B20_Reset(void)
{
    uint8_t retry = 0;

    DS18B20_DQ_OUT();       // 切换为输出模式
    DS18B20_DQ_LOW();       // 拉低总线480-960us
    DS18B20_DelayUs(600);

    DS18B20_DQ_HIGH();      // 释放总线15-60us
    DS18B20_DelayUs(30);

    DS18B20_DQ_IN();        // 切换为输入模式
    while(DS18B20_DQ_READ() && retry < 200)  // 等待从机响应（拉低总线）
    {
        retry++;
        DS18B20_DelayUs(1);
    }

    if(retry >= 200) return 1;  // 未检测到从机
    else retry = 0;

    while(!DS18B20_DQ_READ() && retry < 240) // 等待从机释放总线
    {
        retry++;
        DS18B20_DelayUs(1);
    }
    if(retry >= 240) return 1;

    return 0;  // 初始化成功
}

// 写一个字节
static void DS18B20_WriteByte(uint8_t data)
{
    DS18B20_DQ_OUT();
    for(uint8_t i = 0; i < 8; i++)
    {
        DS18B20_DQ_LOW();       // 拉低总线1-15us
        DS18B20_DelayUs(10);

        if(data & 0x01) DS18B20_DQ_HIGH();  // 写1：释放总线
        else DS18B20_DQ_LOW();              // 写0：保持低电平

        DS18B20_DelayUs(50);    // 保持60-120us
        DS18B20_DQ_HIGH();      // 释放总线
        data >>= 1;
    }
}

// 读一个字节
static uint8_t DS18B20_ReadByte(void)
{
    uint8_t data = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        DS18B20_DQ_OUT();
        DS18B20_DQ_LOW();       // 拉低总线至少1us
        DS18B20_DelayUs(2);

        DS18B20_DQ_HIGH();      // 释放总线
        DS18B20_DelayUs(8);

        DS18B20_DQ_IN();        // 切换为输入模式
        if(DS18B20_DQ_READ()) data |= (0x01 << i);

        DS18B20_DelayUs(40);    // 等待剩余时间
    }
    return data;
}

// 初始化DS18B20
uint8_t DS18B20_Init(void)
{
    return DS18B20_Reset();
}

// 读取温度
uint8_t DS18B20_ReadTemp(float *temp)
{
    uint8_t temp_h, temp_l;
    uint16_t temp_val;

    if(DS18B20_Reset() != 0) return 1;  // 复位失败

    DS18B20_WriteByte(0xCC);   // 跳过ROM指令
    DS18B20_WriteByte(0x44);   // 温度转换指令
    HAL_Delay(750);            // 等待转换完成（最大750ms）

    if(DS18B20_Reset() != 0) return 1;  // 再次复位

    DS18B20_WriteByte(0xCC);   // 跳过ROM指令
    DS18B20_WriteByte(0xBE);   // 读取暂存器指令

    temp_l = DS18B20_ReadByte();  // 读取低8位
    temp_h = DS18B20_ReadByte();  // 读取高8位

    temp_val = (temp_h << 8) | temp_l;

    // 温度计算（12位精度）
    if(temp_val & 0x8000)      // 负温度
    {
        *temp = -(float)((~temp_val + 1) * 0.0625);
    }
    else                        // 正温度
    {
        *temp = (float)(temp_val * 0.0625);
    }

    return 0;  // 读取成功
}
