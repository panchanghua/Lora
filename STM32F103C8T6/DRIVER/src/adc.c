#include "adc.h"

void  Adc_Init(ADC_TypeDef* ADCx,uint16_t GPIO_Pin_x)
{    
 	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	if(ADCx == ADC1)
          RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
        else if(ADCx == ADC2)
          RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC2	, ENABLE );	  //ʹ��ADC2ͨ��ʱ��
        
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
	
        //PA2 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_x;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
        ADC_DeInit(ADCx);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
        
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
        ADC_Init(ADCx, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   
	
        ADC_Cmd(ADCx, ENABLE);	//ʹ��ָ����ADC1
	
        ADC_ResetCalibration(ADCx);	//ʹ�ܸ�λУ׼  	 
	
        while(ADC_GetResetCalibrationStatus(ADCx));	//�ȴ���λУ׼����	
	
        ADC_StartCalibration(ADCx);	 //����ADУ׼
	
        while(ADC_GetCalibrationStatus(ADCx));	 //�ȴ�У׼����
}


u16 Get_Adc(ADC_TypeDef* ADCx,u8 ch)   
{
        u16 DataValue;
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADCx, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			     
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);		//ʹ��ָ����ADC1�����ת����������		 
	while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC ));//�ȴ�ת������
        
        DataValue = ADC_GetConversionValue(ADCx);
        
	return DataValue;	//�������һ��ADC1�������ת�����
}

u32 Get_battery_volt(u8 times)   
{  
	u32 Volt = 0;
        u8 i;
        
        for(i=0;i<times;i++)
        {
          Volt += Get_Adc(ADC1,Battery_Ch);
          delay_us(5000);
        }
        
	return Volt/times;
}

