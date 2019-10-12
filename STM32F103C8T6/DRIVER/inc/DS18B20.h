#ifndef __DS18B20_H

#define __DS18B20_H
#include "sys.h"
#include "stm32f10x.h" 

//IO��������
#define DS18B20_IO_IN()  {GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=8<<0;}
#define DS18B20_IO_OUT() {GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=3<<0;}
////IO����						  
#define	DS18B20_DQ_OUT PAout(2) //���ݶ˿�	PA0
#define	DS18B20_DQ_IN  PAin(2)  //���ݶ˿�	PA0 


u8 DS18B20_Init(void);//��ʼ��DS18B20

void DS18B20_Start(void);//��ʼ�¶�ת��

void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�

u8 DS18B20_Read_Byte(void);//����һ���ֽ�

u8 DS18B20_Read_Bit(void);//����һ��λ

u8 DS18B20_Check(void);//����Ƿ����DS18B20

void DS18B20_Rst(void);//��λDS18B20

short DS18B20_Get_Temp(void); //�õ��¶�ֵ
#endif