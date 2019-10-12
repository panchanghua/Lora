#include "key.h"
#include "XPT2046.h"
#include "lcd_app.h"


KEY App_Key;
KEY *App_Key_str = &App_Key;

extern DEVICE_SATE Device_Sate;
extern DEVICE_SATE *Device_Sate_str;

extern uint8_t Send_Sate_Sign,LCD_EN,Light;

extern Pen_Holder Pen_Point;

extern uint32_t Daiji_Time ,DAIJI_EN ;



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint16_t tt = 0;

	DAIJI_EN = 0;
	Light = 70;
	Daiji_Time = 0;

 	if(HAL_GPIO_ReadPin(GPIOC,A_K1) == 0)
 	{
 		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(GPIOC,A_K1) == 0)
	 	{
	 		App_Key.A_KEY1 = 1;
	 		while(HAL_GPIO_ReadPin(GPIOC,A_K1) == 0)
	 		{
	 			HAL_Delay(1);
	 			if(HAL_GPIO_ReadPin(GPIOC,A_K2) == 0)
 				{
 					tt ++;
					if(tt > 400)    //  组合按键超过3s
				 	{
						App_Key.A_KEY1_KEY2 = 1;
						App_Key.A_KEY1 = 0;
					}
	 			}
	 		}
	 	}
 	}

	if(HAL_GPIO_ReadPin(GPIOC,A_K2) == 0)
 	{
 		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(GPIOC,A_K2) == 0)
	 	{
	 		App_Key.A_KEY2 = 1;
	 	}
 	}

	KEY_DO(App_Key_str,Device_Sate_str);


	if(LCD_EN ==1)
	{
		if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2) == 0)
	 	{
			 HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);

			 if(Pen_Point.Key_Sta == 0)
			 {
				 Read_ADS2(&Pen_Point.X,&Pen_Point.Y);
				 Pen_Point.Key_Sta = 1;

				 Pen_Point.X0=Pen_Point.xfac*Pen_Point.X+Pen_Point.xoff;
				 Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y+Pen_Point.yoff; 

				 Touch_Key(Pen_Point.X0,Pen_Point.Y0);
			 }

			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
			delay_10ms(15);  //???
			HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
		}
	}

	
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4) == 1)
 	{
 		HAL_Delay(20);
		if(HAL_GPIO_ReadPin(GPIOC,A_K2) == 1)
	 	{
			if(Send_Sate_Sign == 1)
			{
				Send_Sate_Sign = 13;
			}
		}
	}

}


void KEY_DO(KEY *key_temp,DEVICE_SATE *Device_Sate_temp)
{
	if(key_temp->A_KEY1 == 1 )
	{
		switch(*Device_Sate_temp)
		{		
			case START_MODE:
				*Device_Sate_temp = CMD_MODE;
				break;

			case CMD_MODE:
				*Device_Sate_temp = TRANSPARENT_MODE;
				break;

			case TRANSPARENT_MODE:
				*Device_Sate_temp = TIMESEND_MODE;
				break;

			case TIMESEND_MODE:
				*Device_Sate_temp = LOWPOWER_MODE;
				break;

			case LOWPOWER_MODE:
				*Device_Sate_temp = CMD_MODE;
				break;

			case TRANSPARENT_DEBUG_MODE:
				*Device_Sate_temp = TIMESEND_MODE;
				break;

			case TIMESEND_DEBUG_MODE:
				*Device_Sate_temp = LOWPOWER_MODE;
				break;
			
			case LOWPOWER_DEBUG_MODE:
				*Device_Sate_temp = CMD_MODE;
				break;
		}

		key_temp->A_KEY1 = 0;	
	}

	if(key_temp->A_KEY2 == 1)
	{
		if(*Device_Sate_temp == TRANSPARENT_MODE)
		{
			*Device_Sate_temp = TRANSPARENT_DEBUG_MODE;
		}else if(*Device_Sate_temp == TRANSPARENT_DEBUG_MODE)
			{
				*Device_Sate_temp = TRANSPARENT_MODE;
			}

		
		if(*Device_Sate_temp == TIMESEND_MODE)
		{
			*Device_Sate_temp = TIMESEND_DEBUG_MODE;
		}else if(*Device_Sate_temp == TIMESEND_DEBUG_MODE)
			{
				*Device_Sate_temp = TIMESEND_MODE;
			}

		key_temp->A_KEY2 = 0;
	}

	if(key_temp->A_KEY1_KEY2 == 1)
	{
		if(*Device_Sate_temp == START_MODE)
		{
			*Device_Sate_temp = TEST_MODE;
		}

		key_temp->A_KEY1_KEY2 = 0;
	}
}

