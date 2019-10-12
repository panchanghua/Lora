#include "lsens.h"
#include "stm32f10x_adc.h"

 
//��ʼ������������
void Lsens_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTFʱ��	
  Adc_Init(ADC2,GPIO_Pin_0);	
	/*GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PF8 anolog����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	*/
	//Adc3_Init();
}
//��ȡLight Sens��ֵ
//0~100:0,�;100,���� 
u8 Lsens_Get_Val(void)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<LSENS_READ_TIMES;t++)
	{
		temp_val+=Get_Adc(ADC2,LSENS_ADC_CHX);	//��ȡADCֵ
		delay_us(5000);
	}
	temp_val/=LSENS_READ_TIMES;//�õ�ƽ��ֵ 
	if(temp_val>4000)temp_val=4000;
	return (u8)(100-(temp_val/40));
}