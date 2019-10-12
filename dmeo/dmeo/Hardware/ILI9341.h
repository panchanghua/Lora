#ifndef __ILI9341_H__
#define __ILI9341_H__

#include "spi.h"
#include "gpio.h"


#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t


#define RES_HIGH()      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET)
#define RES_LOW()     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET)

#define CS_HIGH()      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define CS_LOW()     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)

#define DC_HIGH()      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET)
#define DC_LOW()     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET)

#define lLED_ON()    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET)
#define lLED_OFF()    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET)


void delay_10ms(int count);
void  ILI9341_INITIAL(void);
void  LCD_WriteCommand(uint8_t c);
void LCD_WriteData(uint8_t dat);
void LCD_Write_Data(uint16_t dat16);
void LCD_Write_Data1(uint8_t dat1,uint8_t dat2);
void LCD_ili9341_DrawPicture(uint16_t StartX,uint16_t StartY,uint16_t Xend,uint16_t Yend,const unsigned char *pic,uint8_t mode);
void LCDOpenWindows(uint16_t x, uint16_t y, uint16_t len, uint16_t wid);
void test_tt(void);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint8_t size,uint16_t color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2 ,u16 color);
void Draw_Circle(u16 x0,u16 y0,u8 r, u16 color);
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint16_t color);
u32 mypow(u8 m,u8 n);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u16 color);
void LCD_Clear(uint16_t color);
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);
void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t color);


#define Height 320
#define Width  240
#define RED    0xf800
#define GREEN  0x07e0
#define BLUE   0x001f
#define YELLOW 0xffe0
#define WHITE  0xffff
#define BLACK  0x0000
#define PURPLE 0xf81f


#endif


