#include "lcd_app.h"
#include "ili9341.h"
#include "XPT2046.h"
#include "lorawan.h"
#include "app.h"



extern Node_Info LoRa_Node;
extern Node_Info *LoRa_Node_str;

extern DEVICE_SATE Device_Sate;
extern DEVICE_SATE *Device_Sate_str;

extern SENSOR Sensor_Data;
extern SENSOR *Sensor_Data_str;

extern DEBUG Debug_Data;
extern DEBUG *Debug_Data_str;


extern uint32_t EV_Send_Time;

extern uint32_t send_num ,send_ok,send_shuf ,Debug_shuf;

extern Pen_Holder Pen_Point;
float hh=0.0;
uint8_t Lcd_mode = 99; 
extern char DEVEUI[17];

extern uint32_t RX_SUM,TX_SUM ,RX_BB,TX_BB;

uint32_t Daiji_Time = 0,DAIJI_EN = 0;

	
void LCD_Adjust(void)
{
	uint8_t EN=0;
	uint8_t byte0,byte1,byte2,byte3;
	
	EN = *((__IO uint8_t*)(0x08080070));

	if(EN != 99)
	{
		DEBUG_Printf(" -->液晶触摸需要进行校准，请按照液晶提示进行操作\r\n");
		
		if(Touch_Adjust() == 1)
		{

			EEPROM_WriteOneByte(0x00000070,99);

			byte0=((u8 *)&Pen_Point.xfac)[0];
			byte1=((u8 *)&Pen_Point.xfac)[1];
			byte2=((u8 *)&Pen_Point.xfac)[2];
			byte3=((u8 *)&Pen_Point.xfac)[3];

			EEPROM_WriteOneByte(0x00000071,byte0);
			EEPROM_WriteOneByte(0x00000072,byte1);
			EEPROM_WriteOneByte(0x00000073,byte2);
			EEPROM_WriteOneByte(0x00000074,byte3);

			byte0=((u8 *)&Pen_Point.yfac)[0];
			byte1=((u8 *)&Pen_Point.yfac)[1];
			byte2=((u8 *)&Pen_Point.yfac)[2];
			byte3=((u8 *)&Pen_Point.yfac)[3];

			EEPROM_WriteOneByte(0x00000075,byte0);
			EEPROM_WriteOneByte(0x00000076,byte1);
			EEPROM_WriteOneByte(0x00000077,byte2);
			EEPROM_WriteOneByte(0x00000078,byte3);

			byte0=((u8 *)&Pen_Point.xoff)[0];
			byte1=((u8 *)&Pen_Point.xoff)[1];
			byte2=((u8 *)&Pen_Point.yoff)[0];
			byte3=((u8 *)&Pen_Point.yoff)[1];

			EEPROM_WriteOneByte(0x00000079,byte0);
			EEPROM_WriteOneByte(0x0000007a,byte1);
			EEPROM_WriteOneByte(0x0000007b,byte2);
			EEPROM_WriteOneByte(0x0000007c,byte3);
		}
	}
	else
		{
			byte0 = *((__IO uint8_t*)(0x08080071));
			byte1 = *((__IO uint8_t*)(0x08080072));
			byte2 = *((__IO uint8_t*)(0x08080073));
			byte3 = *((__IO uint8_t*)(0x08080074));

			((u8 *)&Pen_Point.xfac)[0]=byte0;//合并
			((u8 *)&Pen_Point.xfac)[1]=byte1;
			((u8 *)&Pen_Point.xfac)[2]=byte2;
			((u8 *)&Pen_Point.xfac)[3]=byte3;

			byte0 = *((__IO uint8_t*)(0x08080075));
			byte1 = *((__IO uint8_t*)(0x08080076));
			byte2 = *((__IO uint8_t*)(0x08080077));
			byte3 = *((__IO uint8_t*)(0x08080078));

			((u8 *)&Pen_Point.yfac)[0]=byte0;//合并
			((u8 *)&Pen_Point.yfac)[1]=byte1;
			((u8 *)&Pen_Point.yfac)[2]=byte2;
			((u8 *)&Pen_Point.yfac)[3]=byte3;

			byte0 = *((__IO uint8_t*)(0x08080079));
			byte1 = *((__IO uint8_t*)(0x0808007a));
			byte2 = *((__IO uint8_t*)(0x0808007b));
			byte3 = *((__IO uint8_t*)(0x0808007c));

			((u8 *)&Pen_Point.xoff)[0]=byte0;//合并
			((u8 *)&Pen_Point.xoff)[1]=byte1;
			((u8 *)&Pen_Point.yoff)[0]=byte2;
			((u8 *)&Pen_Point.yoff)[1]=byte3;
		}
}

void Touch_Key(uint16_t x,uint16_t y)
{
	if(Lcd_mode != 3)
	{
		if(y>290 && y<320)
		{
			//   模式一触摸取值范围 0< X <80
			if(x>0 && x<80)
			{
					Lcd_mode = 0;
					Device_Sate = CMD_MODE;
			}
			//   模式二触摸取值范围 80< X <160
			if(x>80 && x<160)
			{
					Lcd_mode = 1;
					Device_Sate = CMD_MODE;
			}
			//   模式三触摸取值范围 160< X <240
			if(x>160 && x<240)
			{
					Lcd_mode = 2;
					Device_Sate = CMD_MODE;
			}
		
		}

		if(Lcd_mode == 99)
		{
			Lcd_mode = 98;
		}
	}
		
}

void LCD_APP_INIT_one(void)
{
	LCD_Fill(15,312,65,317,0xd6db);
	LCD_Fill(95,312,145,317,0x0348);
	LCD_Fill(175,312,225,317,0x0348);
	
	LCD_Fill(0,0,240,290,BLACK);
	

	LCD_DrawRectangle(3,3,237,36,0x0348);
	LCD_DrawRectangle(3,39,237,72,0x0348);
	LCD_DrawRectangle(3,75,237,108,0x0348);
	LCD_DrawRectangle(3,111,237,144,0x0348);
	LCD_DrawRectangle(3,147,237,180,0x0348);
	LCD_DrawRectangle(3,183,237,216,0x0348);
	LCD_DrawRectangle(3,219,237,252,0x0348);
	
	LCD_Fill(44,257,95,287,0x0348);
	LCD_Fill(145,257,195,286,0x0348);
	LCD_ShowString(48,263,"Cancel",0xd6db);
	LCD_ShowString(154,263,"Save",0xd6db);

	LCD_ShowString(13,11,"ACT",0xd6db);
	LCD_ShowString(50,11,":",0xd6db);
	LCD_ShowString(13,47,"CFM",0xd6db);
	LCD_ShowString(50,47,":",0xd6db);
	LCD_ShowString(13,83,"ADR",0xd6db);
	LCD_ShowString(50,83,":",0xd6db);
	LCD_ShowString(13,119,"SF",0xd6db);
	LCD_ShowString(50,119,":",0xd6db);
	LCD_ShowString(13,155,"NBT",0xd6db);
	LCD_ShowString(50,155,":",0xd6db);
	LCD_ShowString(13,191,"BAND",0xd6db);
	LCD_ShowString(50,191,":",0xd6db);
	LCD_ShowString(13,227,"PWR",0xd6db);
	LCD_ShowString(50,227,":",0xd6db);

	//----------------------ENT---------------------------
	if(LoRa_Node.NET_Mode == 1) // ABP
	{
		LCD_Fill(88,10,123,29,0x0348);
		LCD_ShowString(90,11,"OTAA",0xd6db);
		LCD_ShowString(180,11,"ABP",0xd6db);
	}else if(LoRa_Node.NET_Mode == 0) 
		{
			LCD_ShowString(90,11,"OTAA",0xd6db);
			LCD_Fill(178,10,205,29,0x0348);
			LCD_ShowString(180,11,"ABP",0xd6db);
		}
		
	//----------------------CFM---------------------------
	if(LoRa_Node.Confirm == 1) //YES
	{
		LCD_Fill(88,46,115,65,0x0348);
		LCD_ShowString(90,47,"YES",0xd6db);
		LCD_ShowString(180,47,"NO",0xd6db);
	}else if(LoRa_Node.Confirm == 0) 
		{
			LCD_ShowString(90,47,"YES",0xd6db);
			LCD_Fill(178,46,197,65,0x0348);
			LCD_ShowString(180,47,"NO",0xd6db);
		}

	//----------------------ADR---------------------------
	if(LoRa_Node.ADR == 1) // YES
	{
		LCD_Fill(88,82,115,101,0x0348);
		LCD_ShowString(90,83,"YES",0xd6db);
		LCD_ShowString(180,83,"NO",0xd6db);
	}else if(LoRa_Node.ADR == 0) 
		{
			LCD_ShowString(90,83,"YES",0xd6db);
			LCD_Fill(178,82,197,101,0x0348);
			LCD_ShowString(180,83,"NO",0xd6db);
		}

	//----------------------SF---------------------------
	LCD_DrawRectangle(90,117,110,137,0x0348);
	LCD_ShowString(96,119,"-",0xd6db);
	if(12-LoRa_Node.SF > 9)
	{
		if(LoRa_Node.ADR == 1)
		{
			LCD_ShowNum(142,119,12-LoRa_Node.SF,2,RED);
		}
		else
			{
				LCD_ShowNum(142,119,12-LoRa_Node.SF,2,0xd6db);
			}
	}else
		{
			if(LoRa_Node.ADR == 1)
			{
				LCD_ShowNum(142,119,12-LoRa_Node.SF,1,RED);
			}
			else
				{
					LCD_ShowNum(142,119,12-LoRa_Node.SF,1,0xd6db);
				}
		}
	LCD_DrawRectangle(181,117,201,137,0x0348);
	LCD_ShowString(188,119,"+",0xd6db);

	//----------------------NBT---------------------------
	LCD_DrawRectangle(90,153,110,173,0x0348);
	LCD_ShowString(96,155,"-",0xd6db);
	
	if(LoRa_Node.Confirm == 1)
	{
		LCD_ShowNum(142,155,LoRa_Node.NBT,1,0xd6db);
	}
	else
		{
			LCD_ShowNum(142,155,LoRa_Node.NBT,1,RED);
		}
	LCD_DrawRectangle(181,153,201,173,0x0348);
	LCD_ShowString(188,155,"+",0xd6db);

	//----------------------BAND---------------------------
	LCD_DrawRectangle(90,189,110,209,0x0348);
	LCD_ShowString(96,191,"-",0xd6db);
	LCD_ShowNum(142,191,LoRa_Node.BAND,1,0xd6db);
	LCD_DrawRectangle(181,189,201,209,0x0348);
	LCD_ShowString(188,191,"+",0xd6db);

	//----------------------PW---------------------------
	LCD_DrawRectangle(90,225,110,245,0x0348);
	LCD_ShowString(96,227,"-",0xd6db);
	if(LoRa_Node.POWER> 9)
	{
		LCD_ShowNum(142,227,LoRa_Node.POWER,2,0xd6db);
	}else
		{
			LCD_ShowNum(142,227,LoRa_Node.POWER,1,0xd6db);
		}
	LCD_DrawRectangle(181,225,201,245,0x0348);
	LCD_ShowString(188,227,"+",0xd6db);
}

void LCD_MODE1_do(void)
{
	static uint8_t kee = 0;
	
	if(3<Pen_Point.Y0 && Pen_Point.Y0<36)   //第一项
	{
		if(80<Pen_Point.X0 && Pen_Point.X0<140)  //OTAA
		{
			LCD_Fill(88,10,123,29,0x0348);
			LCD_ShowString(90,11,"OTAA",0xd6db);

			LCD_Fill(178,10,205,29,BLACK);
			LCD_ShowString(180,11,"ABP",0xd6db);

			LoRa_Node.NET_Mode = 1;
		}

		if(170<Pen_Point.X0 && Pen_Point.X0<220)  //ABP
		{
			LCD_Fill(88,10,123,29,BLACK);
			LCD_ShowString(90,11,"OTAA",0xd6db);

			LCD_Fill(178,10,205,29,0x0348);
			LCD_ShowString(180,11,"ABP",0xd6db);

			LoRa_Node.NET_Mode = 0;
		}
	}

	if(39<Pen_Point.Y0 && Pen_Point.Y0<72)   //第二项 CFM 
	{
		if(80<Pen_Point.X0 && Pen_Point.X0<140)  //yes
		{
			LCD_Fill(88,46,115,65,0x0348);
			LCD_ShowString(90,47,"YES",0xd6db);

			LCD_Fill(178,46,197,65,BLACK);
			LCD_ShowString(180,47,"NO",0xd6db);

			LoRa_Node.Confirm = 1;

			LCD_Fill(142,155,158,171,BLACK);
			LCD_ShowNum(142,155,LoRa_Node.NBT,1,0xd6db);
	
		}

		if(170<Pen_Point.X0 && Pen_Point.X0<220)  //no
		{	
			LCD_Fill(88,46,115,65,BLACK);
			LCD_ShowString(90,47,"YES",0xd6db);

			LCD_Fill(178,46,197,65,0x0348);
			LCD_ShowString(180,47,"NO",0xd6db);

			LoRa_Node.Confirm = 0;
			LCD_Fill(142,155,158,171,BLACK);
			LCD_ShowNum(142,155,LoRa_Node.NBT,1,RED);
		}
	}

	
	if(75<Pen_Point.Y0 && Pen_Point.Y0<108)	 //第三项 ADR
	{
		if(80<Pen_Point.X0 && Pen_Point.X0<140)  //yes
		{
			LCD_Fill(88,82,115,101,0x0348);
			LCD_ShowString(90,83,"YES",0xd6db);

			LCD_Fill(178,82,197,101,BLACK);
			LCD_ShowString(180,83,"NO",0xd6db);

			LoRa_Node.ADR = 1;
			
			LCD_Fill(142,119,158,135,BLACK);
			if(12-LoRa_Node.SF > 9)
			{
				LCD_ShowNum(142,119,12-LoRa_Node.SF,2,RED);
			}else
				{	
					LCD_ShowNum(142,119,12-LoRa_Node.SF,1,RED);
				}
	
		}

		if(170<Pen_Point.X0 && Pen_Point.X0<220)  //no
		{
			LCD_Fill(88,82,115,101,BLACK);
			LCD_ShowString(90,83,"YES",0xd6db);

			LCD_Fill(178,82,197,101,0x0348);
			LCD_ShowString(180,83,"NO",0xd6db);

			LoRa_Node.ADR = 0;

			LCD_Fill(142,119,158,135,BLACK);
			if(12-LoRa_Node.SF > 9)
			{
				LCD_ShowNum(142,119,12-LoRa_Node.SF,2,0xd6db);
			}else
				{	
					LCD_ShowNum(142,119,12-LoRa_Node.SF,1,0xd6db);
				}

		}
	}


	////////////////////////////在ADR关闭的情况下可以进行设置//////////////////////////////////////

	if(LoRa_Node.ADR == 0)
	{
		if(111<Pen_Point.Y0 && Pen_Point.Y0<144)	 //第四项 SF
		{
			if(90<Pen_Point.X0 && Pen_Point.X0<110)  // -
			{
				LoRa_Node.SF ++;

				if(LoRa_Node.SF > 5)
				{
					LoRa_Node.SF = 0;
				}
				
				LCD_Fill(142,119,158,135,BLACK);
				if(12-LoRa_Node.SF > 9)
				{
					LCD_ShowNum(142,119,12-LoRa_Node.SF,2,0xd6db);
				}else
					{
						LCD_ShowNum(142,119,12-LoRa_Node.SF,1,0xd6db);
					}
		
			}

			if(181<Pen_Point.X0 && Pen_Point.X0<201)  // +
			{
				if(LoRa_Node.SF ==0)
				{
					LoRa_Node.SF = 5;
				}else
					{
						LoRa_Node.SF --;
					}
				
				LCD_Fill(142,119,158,135,BLACK);
				if(12-LoRa_Node.SF > 9)
				{
					LCD_ShowNum(142,119,12-LoRa_Node.SF,2,0xd6db);
				}else
					{
						LCD_ShowNum(142,119,12-LoRa_Node.SF,1,0xd6db);
					}

			}
		}
	}


///////////////////////////在确认帧的情况下可以进行设置////////////////////////////////////

	if(LoRa_Node.Confirm == 1)
	{
		if(147<Pen_Point.Y0 && Pen_Point.Y0<180)	 //第五项 NBT
		{
			if(90<Pen_Point.X0 && Pen_Point.X0<110)  // -
			{
				if(LoRa_Node.NBT ==1)
				{
					LoRa_Node.NBT = 8;
				}else
					{
						LoRa_Node.NBT --;
					}
				
				LCD_Fill(142,155,158,171,BLACK);
				LCD_ShowNum(142,155,LoRa_Node.NBT,1,0xd6db);
		
			}

			if(181<Pen_Point.X0 && Pen_Point.X0<201)  // +
			{
				LoRa_Node.NBT++;

				if(LoRa_Node.NBT> 8)
				{
					LoRa_Node.NBT = 1;
				}
				
				LCD_Fill(142,155,158,171,BLACK);
				LCD_ShowNum(142,155,LoRa_Node.NBT,1,0xd6db);
		
			}
		}
	}

	if(183<Pen_Point.Y0 && Pen_Point.Y0<216)	 //第六项 band
	{
		if(90<Pen_Point.X0 && Pen_Point.X0<110)  // -
		{
			if(LoRa_Node.BAND==1)
			{
				LoRa_Node.BAND = 5;
			}else
				{
					LoRa_Node.BAND --;
				}
			
			LCD_Fill(142,191,158,207,BLACK);
			LCD_ShowNum(142,191,LoRa_Node.BAND,1,0xd6db);
	
		}

		if(181<Pen_Point.X0 && Pen_Point.X0<201)  // +
		{
			LoRa_Node.BAND++;

			if(LoRa_Node.BAND> 5)
			{
				LoRa_Node.BAND = 1;
			}
			
			LCD_Fill(142,191,158,207,BLACK);
			LCD_ShowNum(142,191,LoRa_Node.BAND,1,0xd6db);
		}
	}
	

	if(219<Pen_Point.Y0 && Pen_Point.Y0<252)	 //第七项 pw
	{
		if(90<Pen_Point.X0 && Pen_Point.X0<110)  // -
		{
			LoRa_Node.POWER_code++;

			if(LoRa_Node.POWER_code> 8)
			{
				LoRa_Node.POWER_code = 1;
			}
			kee = 1;
		}

		if(181<Pen_Point.X0 && Pen_Point.X0<201)  // +
		{
			if(LoRa_Node.POWER_code == 0)
			{
				LoRa_Node.POWER_code = 8;
			}else
				{
					LoRa_Node.POWER_code --;
				}
			kee = 1;
		}

		if(kee == 1)
		{
			kee = 0;
			if(LoRa_Node.POWER_code == 0)
			{
				LoRa_Node.POWER = 20;
			}

			if(LoRa_Node.POWER_code == 1)
			{
				LoRa_Node.POWER = 17;
			}

			if(LoRa_Node.POWER_code == 2)
			{
				LoRa_Node.POWER = 16;
			}

			if(LoRa_Node.POWER_code == 3)
			{
				LoRa_Node.POWER = 14;
			}

			if(LoRa_Node.POWER_code == 4)
			{
				LoRa_Node.POWER = 12;
			}

			if(LoRa_Node.POWER_code == 5)
			{
				LoRa_Node.POWER = 10;
			}

			if(LoRa_Node.POWER_code == 6)
			{
				LoRa_Node.POWER = 7;
			}

			if(LoRa_Node.POWER_code == 7)
			{
				LoRa_Node.POWER = 5;
			}

			if(LoRa_Node.POWER_code == 8)
			{
				LoRa_Node.POWER = 2;
			}

			LCD_Fill(142,227,158,243,BLACK);
			if(LoRa_Node.POWER> 9)
			{
				LCD_ShowNum(142,227,LoRa_Node.POWER,2,0xd6db);
			}else
				{
					LCD_ShowNum(142,227,LoRa_Node.POWER,1,0xd6db);
				}
		}
	}

	if(257<Pen_Point.Y0 && Pen_Point.Y0<286)	 //取消
	{
		if(44<Pen_Point.X0 && Pen_Point.X0<98)  // -
		{
			LCD_Fill(44,257,95,287,0xed68);
			LCD_ShowString(48,263,"Cancel",0xd6db);

			LoraNode_GetNetMode(LoRa_Node_str);
			LoraNode_GetConfirm(LoRa_Node_str);
			LoraNode_GetADR(LoRa_Node_str);
			LoraNode_GetSF(LoRa_Node_str);
			LoraNode_GetNBT(LoRa_Node_str);
			LoraNode_GetBAND(LoRa_Node_str);
			LoraNode_GetPOWER(LoRa_Node_str);

			//----------------------ENT---------------------------
			if(LoRa_Node.NET_Mode == 1) // ABP
			{
				LCD_Fill(88,10,123,29,0x0348);
				LCD_ShowString(90,11,"OTAA",0xd6db);

				LCD_Fill(178,10,205,29,BLACK);
				LCD_ShowString(180,11,"ABP",0xd6db);
			}else if(LoRa_Node.NET_Mode == 0) 
				{
					LCD_Fill(88,10,123,29,BLACK);
					LCD_ShowString(90,11,"OTAA",0xd6db);

					LCD_Fill(178,10,205,29,0x0348);
					LCD_ShowString(180,11,"ABP",0xd6db);
				}
		
			//----------------------CFM---------------------------
			if(LoRa_Node.Confirm == 1) //YES
			{
				LCD_Fill(88,46,115,65,0x0348);
				LCD_ShowString(90,47,"YES",0xd6db);

				LCD_Fill(178,46,197,65,BLACK);
				LCD_ShowString(180,47,"NO",0xd6db);
			}else if(LoRa_Node.Confirm == 0) 
				{
					LCD_Fill(88,46,115,65,BLACK);
					LCD_ShowString(90,47,"YES",0xd6db);

					LCD_Fill(178,46,197,65,0x0348);
					LCD_ShowString(180,47,"NO",0xd6db);
				}

			//----------------------ADR---------------------------
			if(LoRa_Node.ADR == 1) // YES
			{
				LCD_Fill(88,82,115,101,0x0348);
				LCD_ShowString(90,83,"YES",0xd6db);

				LCD_Fill(178,82,197,101,BLACK);
				LCD_ShowString(180,83,"NO",0xd6db);
			}else if(LoRa_Node.ADR == 0) 
				{
					LCD_Fill(88,82,115,101,BLACK);
					LCD_ShowString(90,83,"YES",0xd6db);

					LCD_Fill(178,82,197,101,0x0348);
					LCD_ShowString(180,83,"NO",0xd6db);
				}

			//----------------------SF---------------------------

			LCD_Fill(142,119,158,135,BLACK);
			if(12-LoRa_Node.SF > 9)
			{
				LCD_ShowNum(142,119,12-LoRa_Node.SF,2,0xd6db);
			}else
				{
					LCD_ShowNum(142,119,12-LoRa_Node.SF,1,0xd6db);
				}

			//----------------------NBT---------------------------
			LCD_Fill(142,155,158,171,BLACK);
			LCD_ShowNum(142,155,LoRa_Node.NBT,1,0xd6db);

			//----------------------BAND---------------------------
			LCD_Fill(142,191,158,207,BLACK);
			LCD_ShowNum(142,191,LoRa_Node.BAND,1,0xd6db);
			//----------------------PW---------------------------

			LCD_Fill(142,227,158,243,BLACK);
			if(LoRa_Node.POWER> 9)
			{
				LCD_ShowNum(142,227,LoRa_Node.POWER,2,0xd6db);
			}else
				{
					LCD_ShowNum(142,227,LoRa_Node.POWER,1,0xd6db);
				}

			

			LCD_Fill(44,257,95,287,0x0348);
			LCD_ShowString(48,263,"Cancel",0xd6db);
		}

		if(145<Pen_Point.X0 && Pen_Point.X0<195)  // save
		{
			LCD_Fill(145,257,195,286,0xed68);
			LCD_ShowString(154,263,"Save",0xd6db);

			LoraNode_SetBAND(LoRa_Node.BAND);
			LoraNode_Delay(100);
			LoraNode_SetNBT(LoRa_Node.NBT);
			LoraNode_Delay(100);
			LoraNode_SetSF(LoRa_Node.SF);
			LoraNode_Delay(100);
			LoraNode_SetADR(LoRa_Node.ADR);
			LoraNode_Delay(100);
			LoraNode_SetCFM(LoRa_Node.Confirm);
			LoraNode_Delay(100);
			LoraNode_SetNetMode(LoRa_Node.NET_Mode);
			LoraNode_Delay(100);
			LoraNode_SetPOWER(LoRa_Node.POWER_code);
			LoraNode_Delay(100);
			LoraNode_Save();

			LCD_Fill(145,257,195,286,0x0348);
			LCD_ShowString(154,263,"Save",0xd6db);
		}
	}
}




void LCD_APP_INIT_two(void)
{
	LCD_Fill(15,312,65,317,0x0348);
	LCD_Fill(95,312,145,317,0xd6db);
	LCD_Fill(175,312,225,317,0x0348);

	LCD_Fill(0,0,240,290,BLACK);

	LCD_DrawRectangle(3,3,237,36,0x0348);
	LCD_DrawRectangle(3,39,237,72,0x0348);
	LCD_DrawRectangle(3,75,237,108,0x0348);
	LCD_DrawRectangle(3,111,237,144,0x0348);
	LCD_DrawRectangle(3,147,237,180,0x0348);
	LCD_DrawRectangle(3,183,237,216,0x0348);
	LCD_DrawRectangle(3,219,237,252,0x0348);
	
	LCD_Fill(44,257,95,286,0x0348);
	LCD_Fill(145,257,195,286,0x0348);
	LCD_ShowString(54,263,"STOP",0xd6db);
	LCD_ShowString(150,263,"START",0xd6db);

	LCD_ShowString(13,11,"DEVEUI",0xd6db);
	LCD_ShowString(63,11,":",0xd6db);
	LCD_ShowString(71,11,DEVEUI,0xd6db);
	
	LCD_ShowString(13,47,"TEMP",0xd6db);
	LCD_ShowString(50,47,":",0xd6db);
	LCD_ShowString(130,47,"HUM",0xd6db);
	LCD_ShowString(160,47,":",0xd6db);

	
	LCD_ShowString(13,83,"LUX",0xd6db);
	LCD_ShowString(50,83,":",0xd6db);
	LCD_ShowString(130,83,"PRE",0xd6db);
	LCD_ShowString(160,83,":",0xd6db);

	LCD_ShowString(13,119,"X",0xd6db);
	LCD_ShowString(29,119,":",0xd6db);
	LCD_ShowString(89,119,"Y",0xd6db);
	LCD_ShowString(105,119,":",0xd6db);
	LCD_ShowString(165,119,"Z",0xd6db);
	LCD_ShowString(181,119,":",0xd6db);

	
	LCD_ShowString(13,155,"SEND",0xd6db);
	LCD_ShowString(50,155,":",0xd6db);
	LCD_ShowString(130,155,"SUCC",0xd6db);
	LCD_ShowString(160,155,":",0xd6db);

	
	LCD_ShowString(13,191,"Interval Time",0xd6db);
	LCD_ShowString(123,191,":",0xd6db);

	LCD_DrawRectangle(130,189,150,209,0x0348);
	LCD_ShowString(136,191,"-",0xd6db);
	
	LCD_ShowNum(166,191,EV_Send_Time/1000,2,0xd6db);
	
	LCD_DrawRectangle(201,189,221,209,0x0348);
	LCD_ShowString(208,191,"+",0xd6db);

	
	LCD_ShowString(60,227,"waitting sent . . .",0xd6db);

}

void LCD_MODE2_do(void)
{
	if(183<Pen_Point.Y0 && Pen_Point.Y0<216)	 //第五项 NBT
	{
		if(130<Pen_Point.X0 && Pen_Point.X0<150)  // -
		{
			if(EV_Send_Time == 10000)
			{
				EV_Send_Time = 60000;
			}else
				{
					EV_Send_Time = EV_Send_Time - 10000;
				}
			
			LCD_Fill(162,191,178,207,BLACK);
			LCD_ShowNum(166,191,EV_Send_Time/1000,2,0xd6db);
	
		}

		if(201<Pen_Point.X0 && Pen_Point.X0<221)  // +
		{
			EV_Send_Time = EV_Send_Time + 10000;

			if(EV_Send_Time > 60000)
			{
				EV_Send_Time = 10000;
			}
			
			LCD_Fill(162,191,178,207,BLACK);
			LCD_ShowNum(166,191,EV_Send_Time/1000,2,0xd6db);
	
		}
	}

	if(257<Pen_Point.Y0 && Pen_Point.Y0<286)	 //
	{
		if(44<Pen_Point.X0 && Pen_Point.X0<95)  //stop
		{
			Device_Sate = CMD_MODE;
			LCD_Fill(60,227,220,243,BLACK);
			LCD_ShowString(60,227,"waitting sent . . .",0xd6db);

			LCD_Fill(145,257,195,286,0x0348);
			LCD_ShowString(150,263,"START",0xd6db);
		}

		if(145<Pen_Point.X0 && Pen_Point.X0<195)  // start
		{
			Device_Sate = TIMESEND_MODE;
			
			LCD_Fill(145,257,195,286,0xed68);
			LCD_ShowString(150,263,"START",0xd6db);
	
		}
	}

}






void LCD_APP_INIT_tre(void)
{
	LCD_Fill(15,312,65,317,0x0348);
	LCD_Fill(95,312,145,317,0x0348);
	LCD_Fill(175,312,225,317,0xd6db);

	LCD_Fill(0,0,240,290,BLACK);

	LCD_DrawRectangle(3,3,237,36,0x0348);
	LCD_DrawRectangle(3,39,237,72,0x0348);
	LCD_DrawRectangle(3,75,237,108,0x0348);
	LCD_DrawRectangle(3,111,237,144,0x0348);
	LCD_DrawRectangle(3,147,237,180,0x0348);
	LCD_DrawRectangle(3,183,237,216,0x0348);
	//LCD_DrawRectangle(3,219,237,252,0x0348);
	
	LCD_Fill(44,221,95,250,0x0348);
	LCD_Fill(145,221,195,250,0x0348);
	LCD_ShowString(54,227,"STOP",0xd6db);
	LCD_ShowString(150,227,"START",0xd6db);


	LCD_ShowString(13,11,"UPCNT",0xd6db);
	LCD_ShowString(50,11,":",0xd6db);
	LCD_ShowString(130,11,"ACKCNT",0xd6db);
	LCD_ShowString(176,11,":",0xd6db);

	LCD_ShowString(13,47,"TOA",0xd6db);
	LCD_ShowString(50,47,":",0xd6db);
	LCD_ShowString(130,47,"NBT",0xd6db);
	LCD_ShowString(160,47,":",0xd6db);
	
	LCD_ShowString(13,83,"CH",0xd6db);
	LCD_ShowString(50,83,":",0xd6db);
	LCD_ShowString(130,83,"SF",0xd6db);
	LCD_ShowString(160,83,":",0xd6db);
	
	LCD_ShowString(13,119,"PWR",0xd6db);
	LCD_ShowString(50,119,":",0xd6db);
	LCD_ShowString(130,119,"PER",0xd6db);
	LCD_ShowString(160,119,":",0xd6db);

	LCD_ShowString(13,155,"RSSI",0xd6db);
	LCD_ShowString(50,155,":",0xd6db);
	LCD_ShowString(130,155,"SNR",0xd6db);
	LCD_ShowString(160,155,":",0xd6db);

	LCD_ShowString(13,191,"TOAL",0xd6db);
	LCD_ShowString(50,191,":",0xd6db);
	LCD_ShowString(130,191,"SUCC",0xd6db);
	LCD_ShowString(160,191,":",0xd6db);

	LCD_Fill(20,257,220,279,0x0348);
	LCD_ShowString(82,260,"GO TO P2P",0xd6db);

}

void LCD_MODE3_do(void)
{
	if(257<Pen_Point.Y0 && Pen_Point.Y0<279)   //第二项 CFM 
	{
		if(20<Pen_Point.X0 && Pen_Point.X0<220)  //yes
		{
			Lcd_mode = 3 ;

			LoRa_Node.P2P_Mode = 1;
			LoRa_Node.P2P_PWR = 20;
			LoRa_Node.P2P_PWR_code = 0;
			LoRa_Node.P2P_SF = 0;
			LoRa_Node.P2P_TIME = 10;
			LoRa_Node.P2P_FRQ= 4700;
			LoRa_Node.P2P_FRQ_step = 1;
		}

	}

	if(221<Pen_Point.Y0 && Pen_Point.Y0<250)	 //第五项 NBT
	{
		if(44<Pen_Point.X0 && Pen_Point.X0<95)  // -
		{
			Device_Sate = CMD_MODE;
			
			LCD_Fill(145,221,195,250,0x0348);
			LCD_ShowString(150,227,"START",0xd6db);
		}

		if(145<Pen_Point.X0 && Pen_Point.X0<195)  // +
		{
			Device_Sate = TIMESEND_DEBUG_MODE;
			LCD_Fill(145,221,195,250,0xed68);
			LCD_ShowString(150,227,"START",0xd6db);
	
		}
	}
}

void LCD_APP_INIT_P2P(void)
{
	LCD_Fill(0,0,240,320,BLACK);

	//LCD_DrawRectangle(3,3,237,36,0x0348);
	LCD_ShowString(33,9,"-----  P2P MODE  -----",0xd6db);
	LCD_DrawRectangle(3,39,237,72,0x0348);
	LCD_DrawRectangle(3,75,237,108,0x0348);
	LCD_DrawRectangle(3,111,237,144,0x0348);
	LCD_DrawRectangle(3,147,237,180,0x0348);
	LCD_DrawRectangle(3,183,237,216,0x0348);
	LCD_DrawRectangle(3,219,237,252,0x0348);
	
	LCD_Fill(44,280,95,300,0x0348);
	LCD_Fill(145,280,195,300,0x0348);
	LCD_ShowString(54,282,"BACK",0xd6db);
	LCD_ShowString(150,282,"START",0xd6db);

	//----------------------MODE---------------------------

	LCD_ShowString(13,47,"MODE",0xd6db);
	LCD_ShowString(50,47,":",0xd6db);
	
	if(LoRa_Node.P2P_Mode == 1) 
	{
		LCD_Fill(88,46,140,65,0x0348);
		LCD_ShowString(90,47,"Master",0xd6db);
		LCD_ShowString(180,47,"Slave",0xd6db);

		
	}else if(LoRa_Node.P2P_Mode == 0) 
		{
			LCD_ShowString(90,47,"Master",0xd6db);
			LCD_Fill(178,46,222,65,0x0348);
			LCD_ShowString(180,47,"Slave",0xd6db);
		}

	//---------------------PWR-------------------------------

	LCD_ShowString(13,83,"PWR",0xd6db);
	LCD_ShowString(50,83,":",0xd6db);

	LCD_DrawRectangle(90,81,110,101,0x0348);
	LCD_ShowString(96,83,"-",0xd6db);
	if(LoRa_Node.P2P_PWR> 9)
	{
		LCD_ShowNum(142,83,LoRa_Node.P2P_PWR,2,0xd6db);
	}else
		{
			LCD_ShowNum(142,83,LoRa_Node.P2P_PWR,1,0xd6db);
		}
	LCD_DrawRectangle(181,81,201,101,0x0348);
	LCD_ShowString(188,83,"+",0xd6db);

	//---------------------dr----------------------------
	
	LCD_ShowString(13,119,"SF",0xd6db);
	LCD_ShowString(50,119,":",0xd6db);
	
	LCD_DrawRectangle(90,117,110,137,0x0348);
	LCD_ShowString(96,119,"-",0xd6db);
	if( 12-LoRa_Node.P2P_SF> 9)
	{
		LCD_ShowNum(142,119,12-LoRa_Node.P2P_SF,2,0xd6db);
	}else
		{
			LCD_ShowNum(142,119,12-LoRa_Node.P2P_SF,1,0xd6db);
		}
	LCD_DrawRectangle(181,117,201,137,0x0348);
	LCD_ShowString(188,119,"+",0xd6db);

	if(LoRa_Node.P2P_SF == 0) //12
	{
		LoRa_Node.P2P_TIME = 5;
	}

	if(LoRa_Node.P2P_SF == 1) //11
	{
		LoRa_Node.P2P_TIME = 4;
	}

	if(LoRa_Node.P2P_SF == 2) //10
	{
		LoRa_Node.P2P_TIME = 3;
	}

	if(LoRa_Node.P2P_SF == 3) //9
	{
		LoRa_Node.P2P_TIME = 3;
	}

	if(LoRa_Node.P2P_SF == 4) //8
	{
		LoRa_Node.P2P_TIME = 2;
	}

	if(LoRa_Node.P2P_SF == 5) //7
	{
		LoRa_Node.P2P_TIME = 2;
	}


	//-------------------time-----------------------------

	LCD_ShowString(13,155,"FRQ",0xd6db);
	LCD_ShowString(50,155,":",0xd6db);

	LCD_DrawRectangle(90,153,110,173,0x0348);
	LCD_ShowString(96,155,"-",0xd6db);
	
	LCD_ShowNum(126,155,LoRa_Node.P2P_FRQ/10,3,0xd6db);

	LCD_ShowString(150,155,".",0xd6db);

	LCD_ShowNum(158,155,LoRa_Node.P2P_FRQ%10,1,0xd6db);
	
	LCD_DrawRectangle(181,153,201,173,0x0348);
	LCD_ShowString(188,155,"+",0xd6db);

	LCD_DrawRectangle(211,153,231,173,0x0348);
	LCD_ShowString(217,155,"S",0xd6db);


	LCD_ShowString(13,191,"RSSI",0xd6db);
	LCD_ShowString(50,191,":",0xd6db);
	LCD_ShowString(130,191,"SNR",0xd6db);
	LCD_ShowString(160,191,":",0xd6db);

	LCD_ShowString(13,227,"TX",0xd6db);
	LCD_ShowString(50,227,":",0xd6db);
	LCD_ShowString(130,227,"RX",0xd6db);
	LCD_ShowString(160,227,":",0xd6db);

}

void LCD_MODE4_do(void)
{
	static uint8_t kee=0;
	if(39<Pen_Point.Y0 && Pen_Point.Y0<72)	//第一项
	{
		if(80<Pen_Point.X0 && Pen_Point.X0<140)  //lora
		{
			LCD_Fill(88,46,140,65,0x0348);
			LCD_ShowString(90,47,"Master",0xd6db);

			LCD_Fill(178,46,222,65,BLACK);
			LCD_ShowString(180,47,"Slave",0xd6db);

			LoRa_Node.P2P_Mode = 1;
		}

		if(170<Pen_Point.X0 && Pen_Point.X0<220)  //fsk
		{
			LCD_Fill(88,46,140,65,BLACK);
			LCD_ShowString(90,47,"Master",0xd6db);

			LCD_Fill(178,46,222,65,0x0348);
			LCD_ShowString(180,47,"Slave",0xd6db);

			LoRa_Node.P2P_Mode = 0;
		}
	}

	if(75<Pen_Point.Y0 && Pen_Point.Y0<108)	 //第二项 pwr
	{
		if(90<Pen_Point.X0 && Pen_Point.X0<110)  // -
		{
			LoRa_Node.P2P_PWR_code++;

			if(LoRa_Node.P2P_PWR_code> 8)
			{
				LoRa_Node.P2P_PWR_code = 1;
			}
			kee = 1;
		}

		if(181<Pen_Point.X0 && Pen_Point.X0<201)  // +
		{
			if(LoRa_Node.P2P_PWR_code == 0)
			{
				LoRa_Node.P2P_PWR_code = 8;
			}else
				{
					LoRa_Node.P2P_PWR_code --;
				}
			kee = 1;
		}

		if(kee == 1)
		{
			kee = 0;
			if(LoRa_Node.P2P_PWR_code == 0)
			{
				LoRa_Node.P2P_PWR = 20;
			}

			if(LoRa_Node.P2P_PWR_code == 1)
			{
				LoRa_Node.P2P_PWR = 17;
			}

			if(LoRa_Node.P2P_PWR_code == 2)
			{
				LoRa_Node.P2P_PWR = 16;
			}

			if(LoRa_Node.P2P_PWR_code == 3)
			{
				LoRa_Node.P2P_PWR = 14;
			}

			if(LoRa_Node.P2P_PWR_code == 4)
			{
				LoRa_Node.P2P_PWR = 12;
			}

			if(LoRa_Node.P2P_PWR_code == 5)
			{
				LoRa_Node.P2P_PWR = 10;
			}

			if(LoRa_Node.P2P_PWR_code == 6)
			{
				LoRa_Node.P2P_PWR = 7;
			}

			if(LoRa_Node.P2P_PWR_code == 7)
			{
				LoRa_Node.P2P_PWR = 5;
			}

			if(LoRa_Node.P2P_PWR_code == 8)
			{
				LoRa_Node.P2P_PWR = 2;
			}

			LCD_Fill(142,83,158,99,BLACK);
			if(LoRa_Node.P2P_PWR> 9)
			{
				LCD_ShowNum(142,83,LoRa_Node.P2P_PWR,2,0xd6db);
			}else
				{
					LCD_ShowNum(142,83,LoRa_Node.P2P_PWR,1,0xd6db);
				}
		}
	}

	if(111<Pen_Point.Y0 && Pen_Point.Y0<144)	 //第四项 SF
	{
		if(90<Pen_Point.X0 && Pen_Point.X0<110)  // -
		{
			LoRa_Node.P2P_SF++;

			if(LoRa_Node.P2P_SF > 5)
			{
				LoRa_Node.P2P_SF = 0;
			}
			
			LCD_Fill(142,119,158,135,BLACK);
			if(12-LoRa_Node.P2P_SF > 9)
			{
				LCD_ShowNum(142,119,12-LoRa_Node.P2P_SF,2,0xd6db);
			}else
				{
					LCD_ShowNum(142,119,12-LoRa_Node.P2P_SF,1,0xd6db);
				}
	
		}

		if(181<Pen_Point.X0 && Pen_Point.X0<201)  // +
		{
			if(LoRa_Node.P2P_SF ==0)
			{
				LoRa_Node.P2P_SF = 5;
			}else
				{
					LoRa_Node.P2P_SF --;
				}
			
			LCD_Fill(142,119,158,135,BLACK);
			if(12-LoRa_Node.P2P_SF > 9)
			{
				LCD_ShowNum(142,119,12-LoRa_Node.P2P_SF,2,0xd6db);
			}else
				{
					LCD_ShowNum(142,119,12-LoRa_Node.P2P_SF,1,0xd6db);
				}

		}

		if(LoRa_Node.P2P_SF == 0) //12
		{
			LoRa_Node.P2P_TIME = 5;
		}

		if(LoRa_Node.P2P_SF == 1) //11
		{
			LoRa_Node.P2P_TIME = 4;
		}

		if(LoRa_Node.P2P_SF == 2) //10
		{
			LoRa_Node.P2P_TIME = 3;
		}

		if(LoRa_Node.P2P_SF == 3) //9
		{
			LoRa_Node.P2P_TIME = 2;
		}

		if(LoRa_Node.P2P_SF == 4) //8
		{
			LoRa_Node.P2P_TIME = 2;
		}

		if(LoRa_Node.P2P_SF == 5) //7
		{
			LoRa_Node.P2P_TIME = 1;
		}
		
	}

	if(147<Pen_Point.Y0 && Pen_Point.Y0<180)	 //第四项 SF
	{
		if(90<Pen_Point.X0 && Pen_Point.X0<110)  // -
		{
			if(LoRa_Node.P2P_FRQ <= 4330)
			{
				LoRa_Node.P2P_FRQ = 5100;
			}else
				{
					LoRa_Node.P2P_FRQ = LoRa_Node.P2P_FRQ - LoRa_Node.P2P_FRQ_step;
				}

			LCD_Fill(126,155,166,171,BLACK);
			LCD_ShowNum(126,155,LoRa_Node.P2P_FRQ/10,3,0xd6db);
			LCD_ShowString(150,155,".",0xd6db);
			LCD_ShowNum(158,155,LoRa_Node.P2P_FRQ%10,1,0xd6db);
	
		}

		if(181<Pen_Point.X0 && Pen_Point.X0<201)  // +
		{
			
			LoRa_Node.P2P_FRQ = LoRa_Node.P2P_FRQ + LoRa_Node.P2P_FRQ_step;

			if(LoRa_Node.P2P_FRQ > 5100)
			{
				LoRa_Node.P2P_FRQ = 4330;
			}
			
			LCD_Fill(126,155,166,171,BLACK);
			LCD_ShowNum(126,155,LoRa_Node.P2P_FRQ/10,3,0xd6db);
			LCD_ShowString(150,155,".",0xd6db);
			LCD_ShowNum(158,155,LoRa_Node.P2P_FRQ%10,1,0xd6db);

		}

		if(211<Pen_Point.X0 && Pen_Point.X0<231)  // +
		{
			switch(LoRa_Node.P2P_FRQ_step)
			{
				case 1:
				{
					LoRa_Node.P2P_FRQ_step = 10;
					LCD_Fill(216,155,226,171,BLACK);
					LCD_ShowString(217,155,"M",0xd6db);
				}
				break;

				case 10:
				{
					LoRa_Node.P2P_FRQ_step = 100;
					LCD_Fill(216,155,226,171,BLACK);
					LCD_ShowString(217,155,"L",0xd6db);
				}
				break;

				case 100:
				{
					LoRa_Node.P2P_FRQ_step = 1;
					LCD_Fill(216,155,226,171,BLACK);
					LCD_ShowString(217,155,"S",0xd6db);
				}
				break;
			}
		}
		
	}

	if(280<Pen_Point.Y0 && Pen_Point.Y0<300)	 //
	{
		if(44<Pen_Point.X0 && Pen_Point.X0<95)  // back
		{
			Lcd_mode = 0;
			Device_Sate = CMD_MODE;
			LCD_Fill(0,290,80,320,0x0348);
			LCD_ShowString(17,295,"Config",WHITE);
			
			LCD_Fill(80,290,160,320,0x0348);
			LCD_ShowString(105,295,"Send",WHITE);
			
			LCD_Fill(160,290,240,320,0x0348);
			LCD_ShowString(180,295,"Debug",WHITE);

			LCD_DrawLine(80,290,80,320,0xd6db);
			LCD_DrawLine(160,290,160,320,0xd6db);
		}

		if(145<Pen_Point.X0 && Pen_Point.X0<195)  // send
		{
			Device_Sate = P2P_MODE;
			LCD_Fill(145,280,195,300,0xed68);
			LCD_ShowString(150,282,"START",0xd6db);
		}
	}
	
}


extern uint8_t Light;
void LCD_APP_Mode(void)
{	
	if(Pen_Point.Key_Sta == 0)
	{
		Daiji_Time++;
		if(Daiji_Time >=1500000)
		{
			Daiji_Time = 1500000;
			DAIJI_EN =1;
			Light = 10;
		}
	}

	static uint8_t log = 0,ago1 = 0;
	switch(Lcd_mode)
	{
		case 0:
			{
				if(log!=1)
				{
					log = 1;	
					LCD_APP_INIT_one();
					Pen_Point.Key_Sta = 0;
				}

				if(Pen_Point.Key_Sta == 1)
				{
					LCD_MODE1_do();
					Pen_Point.Key_Sta = 0;
				}
			}
		break;

		case 1:
			{
				if(log!=2)
				{
					log = 2;
					LCD_APP_INIT_two();
					Pen_Point.Key_Sta = 0;
				}

				if(Pen_Point.Key_Sta == 1)
				{
					LCD_MODE2_do();
					Pen_Point.Key_Sta = 0;
				}

				
				if(Device_Sate == TIMESEND_MODE)
				{
					if(LoRa_Node.Join == 0 && ago1 != 1)
					{
						ago1 = 1;
						LCD_Fill(60,227,220,243,BLACK);
						LCD_ShowString(60,227,"Being join . . .",0xd6db);
					}

					if(LoRa_Node.Join == 1 && ago1 != 2)
					{
						ago1 =  2;
						LCD_Fill(60,227,220,243,BLACK);
						LCD_ShowString(60,227,"Being send. . .",0xd6db);
					}

					if(send_shuf ==1)
					{
						send_shuf = 0;
						LCD_Fill(59,47,75,63,BLACK);
						LCD_ShowNum(59,47,Sensor_Data.Tempter/100,2,0xd6db);
						LCD_Fill(169,47,185,63,BLACK);
						LCD_ShowNum(169,47,Sensor_Data.Humidi,2,0xd6db);
						
						LCD_Fill(59,83,83,99,BLACK);
						LCD_ShowNum(59,83,Sensor_Data.Lux/100,3,0xd6db);
						LCD_Fill(169,83,193,99,BLACK);
						LCD_ShowNum(169,83,Sensor_Data.Pressure,4,0xd6db);

						LCD_Fill(37,119,69,135,BLACK);
						if(Sensor_Data.acc.accel_x>=0)
						{
							LCD_ShowNum(37,119,Sensor_Data.acc.accel_x,3,0xd6db);
						}else
							{
								LCD_ShowString(37,119,"-",0xd6db);
								LCD_ShowNum(45,119,(256-(uint8_t)Sensor_Data.acc.accel_x),3,0xd6db);
							}

						LCD_Fill(113,119,145,135,BLACK);
						if(Sensor_Data.acc.accel_y>=0)
						{
							LCD_ShowNum(113,119,Sensor_Data.acc.accel_y,3,0xd6db);
						}else
							{
								LCD_ShowString(113,119,"-",0xd6db);
								LCD_ShowNum(121,119,(256-(uint8_t)Sensor_Data.acc.accel_y),3,0xd6db);
							}

						LCD_Fill(189,119,221,135,BLACK);
						if(Sensor_Data.acc.accel_z>=0)
						{
							LCD_ShowNum(189,119,Sensor_Data.acc.accel_z,3,0xd6db);
						}else
							{
								LCD_ShowString(189,119,"-",0xd6db);
								LCD_ShowNum(197,119,(256-(uint8_t)Sensor_Data.acc.accel_z),3,0xd6db);
							}

						LCD_Fill(59,155,91,171,BLACK);
						LCD_ShowNum(59,155,send_num,4,0xd6db);
						LCD_Fill(169,155,201,171,BLACK);
						LCD_ShowNum(169,155,send_ok,4,0xd6db);
						
					}
				}
			}
		break;

		case 2:
			{
				if(log!=3)
				{
					log = 3;
					LCD_APP_INIT_tre();
					Pen_Point.Key_Sta = 0;
		
				}

				if(Pen_Point.Key_Sta == 1)
				{
					LCD_MODE3_do();
					Pen_Point.Key_Sta = 0;
				}

				if(Device_Sate == TIMESEND_DEBUG_MODE)
				{
					if(Debug_shuf == 1)
					{
						Debug_shuf = 0;

						LCD_Fill(58,11,90,27,BLACK);
						LCD_ShowString(58,11,Debug_Data.upcnt,0xd6db);
						LCD_Fill(184,11,216,27,BLACK);
						LCD_ShowString(184,11,Debug_Data.ackcnt,0xd6db);

						LCD_Fill(58,47,114,63,BLACK);
						LCD_ShowString(58,47,Debug_Data.toa,0xd6db);
						LCD_Fill(168,47,200,63,BLACK);
						LCD_ShowString(168,47,Debug_Data.nbt,0xd6db);

						LCD_Fill(58,83,114,99,BLACK);
						LCD_ShowString(58,83,Debug_Data.ch,0xd6db);
						LCD_Fill(168,83,200,99,BLACK);
						LCD_ShowString(168,83,Debug_Data.sf,0xd6db);

						LCD_Fill(58,119,114,135,BLACK);
						LCD_ShowString(58,119,Debug_Data.pwr,0xd6db);
						LCD_Fill(168,119,200,135,BLACK);
						LCD_ShowString(168,119,Debug_Data.per,0xd6db);

						LCD_Fill(58,155,114,171,BLACK);
						LCD_ShowString(58,155,Debug_Data.rssi,0xd6db);
						LCD_Fill(168,155,200,171,BLACK);
						LCD_ShowString(168,155,Debug_Data.snr,0xd6db);

						LCD_Fill(58,191,114,207,BLACK);
						LCD_ShowNum(58,191,send_num,4,0xd6db);
						LCD_Fill(168,191,200,207,BLACK);
						LCD_ShowNum(168,191,send_ok,4,0xd6db);

						
					}
				}
			}
		break;


		case 3:
			{
				if(log!=4)
				{
					log = 4;
					LCD_APP_INIT_P2P();
					Pen_Point.Key_Sta = 0;
		
				}

				if(Pen_Point.Key_Sta == 1)
				{
					LCD_MODE4_do();
					Pen_Point.Key_Sta = 0;
				}

				if(RX_BB == 1)
				{
					RX_BB = 0;

					
					LCD_ShowString(13,191,"RSSI",0xd6db);
					LCD_ShowString(50,191,":",0xd6db);
					LCD_ShowString(130,191,"SNR",0xd6db);
					LCD_ShowString(160,191,":",0xd6db);

					LCD_Fill(66,191,98,207,BLACK);
					LCD_ShowString(66,191,LoRa_Node.P2P_RSSI,0xd6db);

					LCD_Fill(176,191,208,207,BLACK);
					LCD_ShowString(176,191,LoRa_Node.P2P_SNR,0xd6db);
					
					LCD_Fill(168,227,200,243,BLACK);
					LCD_ShowNum(168,227,RX_SUM,4,0xd6db);

					
				}

				if(TX_BB == 1)
				{
					TX_BB = 0;
					
					LCD_Fill(58,227,90,243,BLACK);
					LCD_ShowNum(58,227,TX_SUM,4,0xd6db);
				}
			}
		break;

		case 98:
			{
					LCD_Fill(0,290,80,320,0x0348);
					LCD_ShowString(17,295,"Config",WHITE);
					
					LCD_Fill(80,290,160,320,0x0348);
					LCD_ShowString(105,295,"Send",WHITE);
					
					LCD_Fill(160,290,240,320,0x0348);
					LCD_ShowString(180,295,"Debug",WHITE);

					LCD_DrawLine(80,290,80,320,0xd6db);
					LCD_DrawLine(160,290,160,320,0xd6db);

					Lcd_mode = 0;
			}
		break;
	}
}
