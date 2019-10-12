#include "led.h"
#include "timer.h"
#include "key.h"
#include "usart.h"
#include "lcd.h"
#include "dht11.h"
#include "adc.h"
#include "battery.h"
#include "lsens.h"
#include "DS18B20.h"
#define MAX_VALUE 4095

u8 lsens = 0;//����

u32 battery = 0 ;//��ѹ

int Shui_wen;
uint8_t *temp_ds18b20;
float temp;
int value,percent;
u8 TuRangShiDu;
extern key_state key0, key1, key2, key3;
extern ptl_stack tx_stack;
extern ptr_stack rx_stack;


int main(void)
{   
    SystemInit();                              //��ʼ��ϵͳ��ʹ��ϵͳƵ��Ϊ72M
    systick_init();                            //����Systick��ʹ��1us�����ж�
    Lsens_Init();
    Adc_Init(ADC1,GPIO_Pin_1);
    usart_config();
    short num=0;
    while(DS18B20_Init())//��ʼ��DS18B20,18B20 
    { 
        printf("DS18B20 Check Failed!\r\n");
        delay_us(400000);
    } 
    printf("DS18B20 Ready!\r\n"); 
    while(1)
    {
         USART_SendData(USART1, 0xff);
        //DHT11����������  PA4
        if(dht11_read_data() == 1)
        {
          USART_SendData(USART1, dht11_buffer[0]);//ʪ��
          delay_us(5000);
          USART_SendData(USART1, dht11_buffer[2]);//�¶�
          dht11_buffer[0] = 0;
          dht11_buffer[1] = 0;
          dht11_buffer[2] = 0;
          dht11_buffer[3] = 0;
          dht11_buffer[4] = 0;
        }
        else
        {
          USART_SendData(USART1, 0x00);//���Ϳ�����
          USART_SendData(USART1, 0x00);
        }
        delay_us(5000);
        //���������� PA0
        lsens = Lsens_Get_Val();
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
        if(lsens != 0)
        {
          USART_SendData(USART1,lsens);//����
          lsens = 0;
        }
        else
        {
          USART_SendData(USART1, 0x00);//���Ϳ�����
        }
        delay_us(5000);
        //ˮ�´����� PA2
        num=DS18B20_Get_Temp();
        if(num != 0)
        {
          USART_SendData(USART1,num);//ˮ��
          num = 0;
        }
        else
        {
          USART_SendData(USART1, 0x00);//���Ϳ�����
        }
        delay_us(5000); 
        //��ѹ
        battery = Get_battery_volt(10);
        temp = (float) battery * ( 3.3 / 4096);
        percent = (int)((3.3-temp)/3.3*100);
        if(percent != 0)
        {
          USART_SendData(USART1, percent);//��ѹ
          percent= 0;
        }
        else
        {
          USART_SendData(USART1, 0x00);//���Ϳ�����
        }       
        delay_us(5000);
        //������ʪ�� PA1
        temp = (float) (Get_battery_volt(10)*(3.3/4096));
        TuRangShiDu =(u8)( (3.3 - temp) / 3.3 * 100 );
        //printf("rurangshidu:%d\r\n",TuRangShiDu);
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
        if(TuRangShiDu != 0)
        {
          USART_SendData(USART1, TuRangShiDu);//����ʪ��
          TuRangShiDu = 0;
        }
        else
        {
          USART_SendData(USART1, 0x00);//���Ϳ�����
        }
        delay_us(3000000);
    }
}
