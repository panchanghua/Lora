#ifndef __LSENS_H
#define __LSENS_H	
#include "adc.h" 
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "timer.h"
  
#define LSENS_READ_TIMES	10		//���������������ȡ����,����ô���,Ȼ��ȡƽ��ֵ
#define LSENS_ADC_CHX		ADC_Channel_0	//����������������ڵ�ADCͨ�����
    
void Lsens_Init(void); 				//��ʼ������������
u8 Lsens_Get_Val(void);				//��ȡ������������ֵ
#endif 


