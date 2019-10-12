#ifndef __DHT11_H  //如果未定义__DHT11_H
#define __DHT11_H  //初始化__DHT11_H

#include "stdio.h"           //调用C标准库
#include "stm32l0xx.h"
#include "stm32l0xx_hal_gpio.h"
#include "stm32l0xx_hal_gpio_ex.h"
#include "stm32l0xx_hal_rcc.h"
#include "stm32l0xx_hal.h"
#include "max7q.h"

#define DHT11_GPIO_TYPE        GPIOB  //定义DHT11_GPIO的引脚是GPIO_B
#define DHT11_GPIO_PIN         GPIO_PIN_0  //定义DHT11_PIN的引脚是GPIO_Pin_0

#define DHT11_OUT_H()   HAL_GPIO_WritePin(DHT11_GPIO_TYPE,DHT11_GPIO_PIN,GPIO_PIN_SET);
//#define DHT11_SetBit()    GPIO_SetBits(DHT11_GPIO,DHT11_PIN)   //设置DHT11的引脚输出高电平

#define DHT11_OUT_L()   HAL_GPIO_WritePin(DHT11_GPIO_TYPE,DHT11_GPIO_PIN,GPIO_PIN_RESET);
//#define DHT11_ResetBit()  GPIO_ResetBits(DHT11_GPIO,DHT11_PIN)  //设置DHT11的引脚输出低电平

#define DHT11_IN()   HAL_GPIO_ReadPin(DHT11_GPIO_TYPE,DHT11_GPIO_PIN)
//#define DHT11_ReadBit()   GPIO_ReadInputDataBit(DHT11_GPIO,DHT11_PIN)  //设置MCU读取DHT11的引脚电平

void dht11_gpio_input(void);
void dht11_gpio_output(void);
u16 dht11_scan(void);
u16 dht11_read_bit(void);
u16 dht11_read_byte(void);
u16 dht11_read_data(u8 buffer[4]);
/*void TimingDelay_Decrement(void);
void delay_us(__IO uint32_t nTime);
void systick_init_1(void);*/
#endif  //用于结束定义
