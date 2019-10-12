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

u8 lsens = 0;//光敏

u32 battery = 0 ;//电压

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
    SystemInit();                              //初始化系统，使得系统频率为72M
    systick_init();                            //配置Systick，使得1us产生中断
    Lsens_Init();
    Adc_Init(ADC1,GPIO_Pin_1);
    usart_config();
    short num=0;
    while(DS18B20_Init())//初始化DS18B20,18B20 
    { 
        printf("DS18B20 Check Failed!\r\n");
        delay_us(400000);
    } 
    printf("DS18B20 Ready!\r\n"); 
    while(1)
    {
         USART_SendData(USART1, 0xff);
        //DHT11传感器测试  PA4
        if(dht11_read_data() == 1)
        {
          USART_SendData(USART1, dht11_buffer[0]);//湿度
          delay_us(5000);
          USART_SendData(USART1, dht11_buffer[2]);//温度
          dht11_buffer[0] = 0;
          dht11_buffer[1] = 0;
          dht11_buffer[2] = 0;
          dht11_buffer[3] = 0;
          dht11_buffer[4] = 0;
        }
        else
        {
          USART_SendData(USART1, 0x00);//发送空数据
          USART_SendData(USART1, 0x00);
        }
        delay_us(5000);
        //光敏传感器 PA0
        lsens = Lsens_Get_Val();
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
        if(lsens != 0)
        {
          USART_SendData(USART1,lsens);//光照
          lsens = 0;
        }
        else
        {
          USART_SendData(USART1, 0x00);//发送空数据
        }
        delay_us(5000);
        //水温传感器 PA2
        num=DS18B20_Get_Temp();
        if(num != 0)
        {
          USART_SendData(USART1,num);//水温
          num = 0;
        }
        else
        {
          USART_SendData(USART1, 0x00);//发送空数据
        }
        delay_us(5000); 
        //电压
        battery = Get_battery_volt(10);
        temp = (float) battery * ( 3.3 / 4096);
        percent = (int)((3.3-temp)/3.3*100);
        if(percent != 0)
        {
          USART_SendData(USART1, percent);//电压
          percent= 0;
        }
        else
        {
          USART_SendData(USART1, 0x00);//发送空数据
        }       
        delay_us(5000);
        //土壤温湿度 PA1
        temp = (float) (Get_battery_volt(10)*(3.3/4096));
        TuRangShiDu =(u8)( (3.3 - temp) / 3.3 * 100 );
        //printf("rurangshidu:%d\r\n",TuRangShiDu);
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
        if(TuRangShiDu != 0)
        {
          USART_SendData(USART1, TuRangShiDu);//土壤湿度
          TuRangShiDu = 0;
        }
        else
        {
          USART_SendData(USART1, 0x00);//发送空数据
        }
        delay_us(3000000);
    }
}
