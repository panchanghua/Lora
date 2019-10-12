#ifndef _ADC_H
#define _ADC_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "timer.h"

#define Battery_Ch ADC_Channel_1
void Adc_Init(ADC_TypeDef* ADCx,uint16_t GPIO_Pin_x);
u16 Get_Adc(ADC_TypeDef* ADCx,u8 ch);
u32 Get_battery_volt(u8 times); 

extern u32 DL_Volt;
extern float DL_result;
#endif