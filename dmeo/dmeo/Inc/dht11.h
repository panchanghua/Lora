#ifndef __DHT11_H  //���δ����__DHT11_H
#define __DHT11_H  //��ʼ��__DHT11_H

#include "stdio.h"           //����C��׼��
#include "stm32l0xx.h"
#include "stm32l0xx_hal_gpio.h"
#include "stm32l0xx_hal_gpio_ex.h"
#include "stm32l0xx_hal_rcc.h"
#include "stm32l0xx_hal.h"
#include "max7q.h"

#define DHT11_GPIO_TYPE        GPIOB  //����DHT11_GPIO��������GPIO_B
#define DHT11_GPIO_PIN         GPIO_PIN_0  //����DHT11_PIN��������GPIO_Pin_0

#define DHT11_OUT_H()   HAL_GPIO_WritePin(DHT11_GPIO_TYPE,DHT11_GPIO_PIN,GPIO_PIN_SET);
//#define DHT11_SetBit()    GPIO_SetBits(DHT11_GPIO,DHT11_PIN)   //����DHT11����������ߵ�ƽ

#define DHT11_OUT_L()   HAL_GPIO_WritePin(DHT11_GPIO_TYPE,DHT11_GPIO_PIN,GPIO_PIN_RESET);
//#define DHT11_ResetBit()  GPIO_ResetBits(DHT11_GPIO,DHT11_PIN)  //����DHT11����������͵�ƽ

#define DHT11_IN()   HAL_GPIO_ReadPin(DHT11_GPIO_TYPE,DHT11_GPIO_PIN)
//#define DHT11_ReadBit()   GPIO_ReadInputDataBit(DHT11_GPIO,DHT11_PIN)  //����MCU��ȡDHT11�����ŵ�ƽ

void dht11_gpio_input(void);
void dht11_gpio_output(void);
u16 dht11_scan(void);
u16 dht11_read_bit(void);
u16 dht11_read_byte(void);
u16 dht11_read_data(u8 buffer[4]);
/*void TimingDelay_Decrement(void);
void delay_us(__IO uint32_t nTime);
void systick_init_1(void);*/
#endif  //���ڽ�������
