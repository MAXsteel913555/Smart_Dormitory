#include "delay.h"

void HAL_Delay_us(uint32_t us)
{
  uint32_t ticks;
  uint32_t told, tnow, tcnt = 0;
  uint32_t reload = SysTick->LOAD;  // 获取SysTick装载值
  ticks = us * (SystemCoreClock / 1000000);  // 计算需要的节拍数（系统时钟MHz数）
  told = SysTick->VAL;  // 获取当前计数值

  while (1)
  {
    tnow = SysTick->VAL;
    if (tnow != told)
    {
      if (tnow < told)
        tcnt += told - tnow;  // 正常计数（递减）
      else
        tcnt += reload - tnow + told;  // 溢出情况

      told = tnow;
      if (tcnt >= ticks)
        break;  // 延时时间到
    }
  }
}
