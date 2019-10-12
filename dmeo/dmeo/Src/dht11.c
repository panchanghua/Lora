#include "dht11.h"
#include "stm32l0xx.h"
#include "stm32l0xx_hal_gpio.h"
#include "stm32l0xx_hal_gpio_ex.h"
#include "stm32l0xx_hal_rcc.h"
#include "stm32l0xx_hal.h"
#include "stm32l0xx_it.h"
#include "main.h"
#include "delay.h"
#include "core_cm0plus.h"

/*__IO uint32_t TimingDelay;

void systick_init_1(void)
{
    /*配置Systick重载值，系统时钟为72MHz*/
    /*设置72000，中断时间：72000*（1/72000000） = 1ms*/
    /*if(SysTick_Config(HAL_RCC_GetHCLKFreq()/1000000) == 1)
    {
        while(1);
    }
    /*
    while(Systick_Config(72000) == 1);
    *//*
}
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    {
        TimingDelay--;
    }
}
void delay_us(__IO uint32_t nTime)
{
    TimingDelay = nTime;//时钟滴答数
    while(TimingDelay != 0);
}*/
/*
void delay_us(uint32_t nus)
{
  uint32_t temp;
  SysTick->LOAD = 9*nus;
  SysTick->VAL=0X00;//清空计数器
  SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
   do
   {
      temp=SysTick->CTRL;//读取当前倒计数值
   }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
   SysTick->CTRL=0x00; //关闭计数器
   SysTick->VAL =0X00; //清空计数器
}*/
void delay_us(uint32_t delay)
{
  uint32_t tickstart=0;
  uint32_t tickNum=0;
  uint32_t tickMax=SysTick->LOAD;
  uint32_t delay_usvalue=(SysTick->LOAD / 1000)* delay;
  tickstart = SysTick->VAL;
  DEBUG_Printf("%ld,%ld \r\n",tickMax,delay_usvalue);
  while(1)
  {
    uint32_t cur_tick = SysTick->VAL;
    if(cur_tick > tickstart )
    {
      tickNum = tickstart + (tickMax - cur_tick);
    }
    else
    {
      tickNum = tickstart - cur_tick;
    }
    if(tickNum>delay_usvalue)
    {
      break;
    }
  }
  DEBUG_Printf("%ld,%ld \r\n",tickNum,delay_usvalue);
  
  
}
void dht11_gpio_input(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  GPIO_InitStructure.Pin = DHT11_GPIO_PIN;
  GPIO_InitStructure.Mode =  GPIO_MODE_INPUT;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  
  
  HAL_GPIO_Init(DHT11_GPIO_TYPE, &GPIO_InitStructure);
}

void dht11_gpio_output(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  GPIO_InitStructure.Pin = DHT11_GPIO_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  
  HAL_GPIO_Init(DHT11_GPIO_TYPE, &GPIO_InitStructure);
}

void dht11_reset(void)
{
    // 按照DHT11手册步骤
    dht11_gpio_output();
    DHT11_OUT_L();
    HAL_Delay(20);
    DHT11_OUT_H();
    delay_us(30);
    dht11_gpio_input();
}

u16 dht11_scan(void)
{
    return DHT11_IN();
}

u16 dht11_read_bit(void)
{
    while (DHT11_IN() == GPIO_PIN_RESET);
    delay_us(30);
    if (DHT11_IN() == GPIO_PIN_SET)
    {
        while (DHT11_IN() == GPIO_PIN_SET);
        return 1;
    }
    else
    {
        return 0;
    }
}


u16 dht11_read_byte(void)
{
    u16 i;
    u16 data = 0;
    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        data |= dht11_read_bit();
        DEBUG_Printf("data = %d\r\n",data);
    }
    return data;
}


u16 dht11_read_data(u8 buffer[5])
{
    u16 i = 0;
    
    dht11_reset();
    if (dht11_scan() == GPIO_PIN_RESET)
    {
        //检测到DHT11响应
        while (dht11_scan() == GPIO_PIN_RESET);
        while (dht11_scan() == GPIO_PIN_SET);
        for (i = 0; i < 5; i++)
        {
            buffer[i] = dht11_read_byte();
        }
        
        while (dht11_scan() == GPIO_PIN_RESET);
        dht11_gpio_output();
        DHT11_OUT_H();
        
        u8 checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
          DEBUG_Printf("buffer[0] = %d \r\n",buffer[0]);
          DEBUG_Printf("buffer[1] = %d \r\n",buffer[1]);
          DEBUG_Printf("buffer[2] = %d \r\n",buffer[2]);
          DEBUG_Printf("buffer[3] = %d \r\n",buffer[3]);
          DEBUG_Printf("buffer[4] = %d \r\n",buffer[4]);
        if (checksum != buffer[4])
        {
            // checksum error
            return 1;
        }
    }
    else
    {
      return 1;
    }
    return 0;
}