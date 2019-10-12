#include "dht11.h"
u8 dht11_buffer[5];
void dht11_portin(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//Configure pin as input
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮动输入
GPIO_Init(GPIOA,&GPIO_InitStructure);
}
void dht11_portout(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//Configure pin as input
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
GPIO_Init(GPIOA,&GPIO_InitStructure);
}
void dht11_reset(void)
{
    dht11_portout(); // 将DHT11单总线设置成输出模式
    DHT11_OUT_L;    // MCU将单总线拉低
    delay_us(19000);// 保持19ms
    DHT11_OUT_H;    // MCU将单总线拉高
    delay_us(30);   //保持30us
    dht11_portin();
}
u16 dht11_scan(void)
{
    return DHT11_IN;
}
u16 dht11_read_bit(void)
{
    while(DHT11_IN == RESET);   //检测到低电平时候等待，每一位数据的50us低电平
    delay_us(40);
    if(DHT11_IN == SET)
    {
        while(DHT11_IN == SET);
        return 1;
    }
    else
        return 0;
}
u16 dht11_read_byte(void)
{
    u16 i;
    u16 data = 0;
    for(i = 0; i < 8; i ++)
    {
        data <<= 1;
        data |= dht11_read_bit();
    }
    return data;
}
u16 dht11_read_data(void)
{
    u16 i = 0;
    dht11_reset();
    if(dht11_scan() == 0)   //检测到DHT11响应
    {
        while(dht11_scan() == RESET);
        while(dht11_scan() == SET);
        for(i = 0; i < 5; i ++)
        {
            dht11_buffer[i] = dht11_read_byte();
        }
        while(DHT11_IN == RESET);
        dht11_portout();
        DHT11_OUT_H;
        if((dht11_buffer[0] + dht11_buffer[1] + dht11_buffer[2] + dht11_buffer[3]) == dht11_buffer[4])
        {
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}