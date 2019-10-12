#include "lorawan.h"
#include "app.h"
#include "usart.h"
#include <stdarg.h>
#include <string.h>

extern uint8_t TimeOut_Sign;

Node_Info LoRa_Node;
Node_Info *LoRa_Node_str = &LoRa_Node;

Send_Info LoRa_Send;
Send_Info *LoRa_Send_str = &LoRa_Send;

extern uint8_t Send_Sate_Sign;

extern USART_RECEIVETYPE Usart2_RX;
extern USART_RECEIVETYPE Usart1_RX;
extern USART_RECEIVETYPE LPUsart1_RX;

#define RXLEN	256
uint8_t AT_Data_buf[RXLEN];

/* OTAA mode default setting */
//uint8_t devEui[] = "00 4A 77 00 66 FF FE CA";

uint8_t appEui[] = "11 22 33 44 55 66 77 88";

uint8_t appKey[] = "00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF";

/* ABP mode default setting */
uint8_t appSKey[] = "2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C";

uint8_t nwkSKey[] = "2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C";

uint8_t devAddr[] = "FF FF FE A6";


void LoraNode_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct; 
	//UART_HandleTypeDef UartHandle;	
	
	/* enable clock */
	__HAL_RCC_GPIOB_CLK_ENABLE();
		
	/* gpio config */
	GPIO_InitStruct.Pin 	= LORANODE_WAKE_PIN|LORANODE_MODE_PIN|LORANODE_NRST_PIN;
	GPIO_InitStruct.Mode	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull 	= GPIO_NOPULL;
	GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_VERY_HIGH;	
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin 	= LORANODE_STAT_PIN|LORANODE_BUSY_PIN;
	GPIO_InitStruct.Mode	= GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull 	= GPIO_NOPULL;
	GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void LoraNode_Init_Mode(LoraNode_Mode_T mode)
{
	LoraNode_Reset();

	LoraNode_Wake_Sleep(MODE_WAKEUP);
	
	LoraNode_Mode_Set(mode);	
}


//---------------------------------------------
//	@funtion:	set the lorawan module mode, command mode or transparent mode
//	@paramters:	MODE_CMD, or MODE_TRANSPARENT
//	@return:	void
//----------------------------------------------
void LoraNode_Mode_Set(LoraNode_Mode_T mode)
{
	if (mode == MODE_CMD )
		LORANODE_MODE_HIGH();
	if (mode == MODE_TRANSPARENT)
		LORANODE_MODE_LOW();
}

//----------------------------------------------------------------------
//	@funtion:	set the lorawan module mode, wakeup or sleep mode
//	@paramters:	MODE_WAKEUP, or MODE_SLEEP
//	@return:	void
//-------------------------------------------------------------------
void LoraNode_Wake_Sleep(LoraNode_SleepMode_T mode)
{
	if (mode == MODE_WAKEUP)
	{
		/* wake signal, high for module wakeup, low for module sleep */	
		if (LoraNode_IO_State(LORANODE_WAKE_PIN) != GPIO_PIN_SET)
		{
			/* wake module first, and wait 10 ms */
			LORANODE_WAKE_HIGH();
			
			LoraNode_Delay(20);				
		}
	}
	
	if (mode == MODE_SLEEP)
	{
		if (LoraNode_IO_State(LORANODE_WAKE_PIN) != GPIO_PIN_RESET)
		{
			LORANODE_WAKE_LOW();			
		}
	}
}




static void LoraNode_Reset(void)
{
	LORANODE_NRST_LOW();	
	
	LoraNode_Delay(15);	//15ms
	
	LORANODE_NRST_HIGH();	
}

//-------------------------------------------------------AT --------------------------------


//
//	@function:	get the lorawan module version
//	@return:	the point of version information
//
char *LoraNode_GetVer(void)
{
	char ASK_VER[] = "AT+VER?\r\n";
	char *temp = "+VER:";
	char *ptr = NULL;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(ASK_VER);
	
	LoraNode_Read(AT_Data_buf);	/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		return ptr;	
	}

	return ptr;
}

/*
**	@function:	get deveui 
**	@return:	the point of the deveui
*/
char DEVEUI[17] = {0};
void LoraNode_GetDevEUI(Node_Info *LoRa_temp)
{
	char GetDevEUI[] = "AT+DEVEUI?\r\n";
	char *temp = "+DEVEUI:";
	char *ptr = NULL;

	uint8_t temp1=0,temp2=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetDevEUI);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */	
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i=0;i<8;i++)
		{
			DEVEUI[2*i] = *((ptr + 8+(3*i))+1);
			DEVEUI[2*i+1] = *((ptr + 8+(3*i))+2);
		}
		for(uint8_t i=0;i<8;i++)
		{
			temp1 = *((ptr + 8+(3*i))+1);
			temp2 = *((ptr + 8+(3*i))+2);
			if(temp1 > 0x40)
			{
				temp1 = temp1 - 55;
			}else{
					temp1 = temp1 - 48;
				}
			if(temp2 > 0x40)
			{
				temp2 = temp2 - 55;
			}else{
					temp2 = temp2 - 48;
				}
			LoRa_temp->DevEUI[i] = temp1*16 + temp2;
		}
	}
	
}


/*
**	@function:	get appeui 
**	@return:	the point of the appeui
*/
void LoraNode_GetAppEUI(Node_Info *LoRa_temp)
{
	char GetAPPEUI[] = "AT+APPEUI?\r\n";
	char *temp = "+APPEUI:";
	char *ptr = NULL;

	uint8_t temp1=0,temp2=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetAPPEUI);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i=0;i<8;i++)
		{
			temp1 = *((ptr + 8+(3*i))+1);
			temp2 = *((ptr + 8+(3*i))+2);
			if(temp1 > 0x40)
			{
				temp1 = temp1 - 55;
			}else{
					temp1 = temp1 - 48;
				}
			if(temp2 > 0x40)
			{
				temp2 = temp2 - 55;
			}else{
					temp2 = temp2 - 48;
				}
			LoRa_temp->AppEUI[i] = temp1*16 + temp2;
		}
	}
	
}

/*
**	@function:	get appkey 
**	@return:	the point of the response of appkey
*/
void LoraNode_GetAppKey(Node_Info *LoRa_temp)
{
	char GetAPPKEY[] = "AT+APPKEY?\r\n";	
	char *temp = "+APPKEY:";
	char *ptr = NULL;	

	uint8_t temp1=0,temp2=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetAPPKEY);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i=0;i<16;i++)
		{
			temp1 = *((ptr + 8+(3*i))+1);
			temp2 = *((ptr + 8+(3*i))+2);
			if(temp1 > 0x40)
			{
				temp1 = temp1 - 55;
			}else{
					temp1 = temp1 - 48;
				}
			if(temp2 > 0x40)
			{
				temp2 = temp2 - 55;
			}else{
					temp2 = temp2 - 48;
				}
			LoRa_temp->AppKEY[i] = temp1*16 + temp2;
		}
	}
	
}

/*
**	@function:	get devaddr 
**	@return:	the point of the Devaddr
*/
void LoraNode_GetDevAddr(Node_Info *LoRa_temp)
{
	char GetDEVAddr[] = "AT+DEVADDR?\r\n";
	char *temp = "+DEVADDR:";
	char *ptr = NULL;

	uint8_t temp1=0,temp2=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetDEVAddr);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i=0;i<4;i++)
		{
			temp1 = *((ptr + 9+(3*i))+1);
			temp2 = *((ptr + 9+(3*i))+2);
			if(temp1 > 0x40)
			{
				temp1 = temp1 - 55;
			}else{
					temp1 = temp1 - 48;
				}
			if(temp2 > 0x40)
			{
				temp2 = temp2 - 55;
			}else{
					temp2 = temp2 - 48;
				}
			LoRa_temp->DevADDR[i] = temp1*16 + temp2;
		}
	}
	
}


/*
**	@function:	get appskey 
**	@return:	the point of the response of appskey
*/
void LoraNode_GetAppSKey(Node_Info *LoRa_temp)
{
	char GetAppSKey[] = "AT+APPSKEY?\r\n";
	char *temp = "+APPSKEY:";
	char *ptr = NULL;

	uint8_t temp1=0,temp2=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetAppSKey);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i=0;i<16;i++)
		{
			temp1 = *((ptr + 9+(3*i))+1);
			temp2 = *((ptr + 9+(3*i))+2);
			if(temp1 > 0x40)
			{
				temp1 = temp1 - 55;
			}else{
					temp1 = temp1 - 48;
				}
			if(temp2 > 0x40)
			{
				temp2 = temp2 - 55;
			}else{
					temp2 = temp2 - 48;
				}
			LoRa_temp->AppSKEY[i] = temp1*16 + temp2;
		}
	}
}


/*
**	@function:	get nwkskey 
**	@return:	the point of the response of nwkskey
*/
void LoraNode_GetNwkSKey(Node_Info *LoRa_temp)
{
	char GetNwkSKey[] = "AT+NWKSKEY?\r\n";	
	char *temp = "+NWKSKEY:";
	char *ptr = NULL;	

	uint8_t temp1=0,temp2=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetNwkSKey);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i=0;i<16;i++)
		{
			temp1 = *((ptr + 9+(3*i))+1);
			temp2 = *((ptr + 9+(3*i))+2);
			if(temp1 > 0x40)
			{
				temp1 = temp1 - 55;
			}else{
					temp1 = temp1 - 48;
				}
			if(temp2 > 0x40)
			{
				temp2 = temp2 - 55;
			}else{
					temp2 = temp2 - 48;
				}
			LoRa_temp->NwkSKEY[i] = temp1*16 + temp2;
		}
	}

}


/*
**	@function:	get uplink value
**	@return:	the point of the uplink buffer
*/
void LoraNode_GetNetMode(Node_Info *LoRa_temp)
{
	char GetNetMode[] = "AT+OTAA?\r\n";
	char *temp = "+OTAA:";
	char *ptr = NULL;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetNetMode);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		if( *(ptr + 7) == 0x30)
		{
			LoRa_temp->NET_Mode = 0;
		}
		if(*(ptr + 7) == 0x31)
		{
			LoRa_temp->NET_Mode = 1;
		}
	}
	
}



void LoraNode_GetConfirm(Node_Info *LoRa_temp)
{
	char GetConfirm[] = "AT+CONFIRM?\r\n";
	char *temp = "+CONFIRM:";
	char *ptr = NULL;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetConfirm);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		if(*(ptr + 10) == 0x30)
		{
			LoRa_temp->Confirm= 0;
		}
		if(*(ptr + 10) == 0x31)
		{
			LoRa_temp->Confirm= 1;
		}
	}
	
}

void LoraNode_GetADR(Node_Info *LoRa_temp)
{
	char ADR[] = "AT+ADR?\r\n";
	char *temp = "+ADR:";
	char *ptr = NULL;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(ADR);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		if(*(ptr + 6) == 0x30)
		{
			LoRa_temp->ADR= 0;
		}
		if(*(ptr + 6) == 0x31)
		{
			LoRa_temp->ADR= 1;
		}
	}
	
}


void LoraNode_GetVER(Node_Info *LoRa_temp)
{
	char VER[] = "AT+VER?\r\n";
	char *temp = "CN470";
	char *temp1 = "CLAA";
	char *ptr = NULL;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(VER);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		LoRa_temp->ver = 2; // cn470
	}

	if((ptr = StringStr((char *)AT_Data_buf, temp1)) != NULL)
	{
		LoRa_temp->ver = 1; // claa
	}
	
}


void LoraNode_GetSF(Node_Info *LoRa_temp)
{
	char SF[] = "AT+DATARATE?\r\n";
	char *temp = "+DATARATE:";
	char *ptr = NULL;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SF);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		LoRa_temp->SF = (*(ptr + 11)) - 0x30;	
	}
	
}

void LoraNode_GetNBT(Node_Info *LoRa_temp)
{
	char GetNBTRIALS[] = "AT+NBTRIALS?\r\n";
	char *temp = "+NBTRIALS:";
	char *ptr = NULL;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetNBTRIALS);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		LoRa_temp->NBT = (*(ptr + 11)) - 0x30;	
	}
	
}

void LoraNode_GetBAND(Node_Info *LoRa_temp)
{
	char GetBAND[] = "AT+BAND?\r\n";
	char *temp = "+BAND:";
	char *ptr = NULL;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetBAND);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		LoRa_temp->BAND = (*(ptr + 7)) - 0x30;	
	}
	
}

void LoraNode_GetSNR(Node_Info *LoRa_temp)
{
	char GetSNR[] = "AT+SNR?\r\n";
	char *temp = ",";
	char *temp1 = "]";
	char *ptr = NULL;
	char *ptr1 = NULL;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetSNR);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr1 = StringStr((char *)AT_Data_buf, temp1)) != NULL)
	{
		for(uint8_t i = 0;i<5;i++)
		{
			if(*(ptr1+i+3) == 0x2c)
			{
				break;
			}else
				{
					LoRa_temp->P2P_RSSI[i] = *(ptr1+i+3);
				}
		}
	}

	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i = 0;i<5;i++)
		{
			if(*(ptr+i+2) == 0x0d)
			{
				break;
			}else
				{
					LoRa_temp->P2P_SNR[i] = *(ptr+i+2);
				}
		}
		
	}
	
}



void LoraNode_GetPOWER(Node_Info *LoRa_temp)
{
	char GetPOWER[] = "AT+POWER?\r\n";
	char *temp = "+POWER:";
	char *ptr = NULL;
	uint8_t code = 0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetPOWER);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		LoRa_temp->POWER_code = (*(ptr + 8)) - 0x30;	
		code = LoRa_temp->POWER_code;
	}

	if(code == 0)
	{
		LoRa_temp->POWER = 20;
	}

	if(code == 1)
	{
		LoRa_temp->POWER = 17;
	}

	if(code == 2)
	{
		LoRa_temp->POWER = 16;
	}

	if(code == 3)
	{
		LoRa_temp->POWER = 14;
	}

	if(code == 4)
	{
		LoRa_temp->POWER = 12;
	}

	if(code == 5)
	{
		LoRa_temp->POWER = 10;
	}

	if(code == 6)
	{
		LoRa_temp->POWER = 7;
	}

	if(code == 7)
	{
		LoRa_temp->POWER = 5;
	}

	if(code == 8)
	{
		LoRa_temp->POWER = 2;
	}
	
}



void LoraNode_GetState(Send_Info *LoRa_temp)
{
	char GetConfirm[] = "AT+STATUS?\r\n";
	char *temp = "+STATUS:";
	char *ptr = NULL;
	uint8_t dec=0,dec1=0,dec2=0,dec3=0,dec4=0,dec5=0,dec6=0,dec7=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetConfirm);
	HAL_Delay(200);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		dec =StrToHex( *(ptr + 9));
		dec1 = StrToHex( *(ptr + 10));

		LoRa_temp->Up_Result = dec*16 + dec1;

		dec = StrToHex( *(ptr + 12));
		dec1 = StrToHex( *(ptr + 13));

		LoRa_temp->Up_CH = dec*16 + dec1;

		dec = StrToHex( *(ptr + 15));
		dec1 = StrToHex( *(ptr + 16));

		LoRa_temp->Up_RATE = dec*16 + dec1;

		dec = StrToHex( *(ptr + 18));
		dec1 = StrToHex( *(ptr + 19));

		LoRa_temp->Up_DB = dec*16 + dec1;

		dec = StrToHex( *(ptr + 66));
		dec1 = StrToHex( *(ptr + 67));
		dec2 = StrToHex( *(ptr + 69));
		dec3 = StrToHex( *(ptr + 70));

		LoRa_temp->Up_Cache = (dec*16+ dec1) +(dec2*16 + dec3)*256 ;

		dec = StrToHex( *(ptr + 72));
		dec1 = StrToHex( *(ptr + 73));
		dec2 = StrToHex( *(ptr + 75));
		dec3 = StrToHex( *(ptr + 76));

		LoRa_temp->Resend = (dec*16+ dec1) +(dec2*16 + dec3)*256 ;

		dec = StrToHex( *(ptr + 117));
		dec1 = StrToHex( *(ptr + 118));
		dec2 = StrToHex( *(ptr + 120));
		dec3 = StrToHex( *(ptr + 121));
		dec4 = StrToHex( *(ptr + 123));
		dec5 = StrToHex( *(ptr + 124));
		dec6 = StrToHex( *(ptr + 126));
		dec7 = StrToHex( *(ptr + 127));

		LoRa_temp->Up_Link = (dec*16+ dec1) +(dec2*16 + dec3)*256 + (dec4*16+ dec5)*65536 +(dec6*16 + dec7)*256*65536 ;

		
	}
	
}


/*
**	@function:	set gpio state
**	@paramters:	pin, 0, 1, 2, 3, 255; state 0, 1
**	@return:	0 success, 1 bad paramter
*/
uint32_t LoraNode_SetGPIO(uint32_t pin, uint32_t state)
{	
	uint8_t GPIO[20] = "AT+GPIO=";
	uint8_t buf[5] = {0};
	char *temp = "OK";
	char *ptr = (char*)GPIO;
	
	Int2Str(buf, pin);
	strcat((char *)GPIO, (char *)buf);
	
	while(*++ptr);	//get the end
	*ptr++ = ',';

	memset(buf,0,5);
	Int2Str(buf, state);
	StringCat(GPIO, buf);
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GPIO);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		return 0;
	}
	
	return -1;
}

/*
**	@function:	set net mode
**	@paramters: 0, abp mode; 1, otaa mode
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetNetMode(LoraNode_NetMode_T mode)
{	
	uint8_t NetMode[20] = "AT+OTAA=";
	uint8_t buf[10] = {0};
	char *temp = "OK";
	
	Int2Str(buf, mode);
	StringCat(NetMode, buf);

	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(NetMode);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}

uint32_t LoraNode_SetCFM(uint8_t nu)
{	
	uint8_t CFM[20] = "AT+CONFIRM=";
	uint8_t buf[5] = {0};
	char *temp = "OK";
	
	Int2Str(buf, nu);
	StringCat(CFM, buf);

	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(CFM);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}


uint32_t LoraNode_SetADR(uint8_t nu)
{	
	uint8_t ADR[20] = "AT+ADR=";
	uint8_t buf[5] = {0};
	char *temp = "OK";
	
	Int2Str(buf, nu);
	StringCat(ADR, buf);

	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(ADR);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}


uint32_t LoraNode_SetSF(uint8_t nu)
{	
	uint8_t SF[20] = "AT+DATARATE=";
	uint8_t buf[5] = {0};
	char *temp = "OK";
	
	Int2Str(buf, nu);
	StringCat(SF, buf);

	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SF);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}


uint32_t LoraNode_SetNBT(uint8_t nu)
{	
	uint8_t NBT[20] = "AT+NBTRIALS=";
	uint8_t buf[5] = {0};
	char *temp = "OK";
	
	Int2Str(buf, nu);
	StringCat(NBT, buf);

	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(NBT);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}

uint32_t LoraNode_SetBAND(uint8_t nu)
{	
	uint8_t BAND[20] = "AT+band=";
	uint8_t buf[5] = {0};
	char *temp = "OK";
	
	Int2Str(buf, nu);
	StringCat(BAND, buf);

	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(BAND);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}


uint32_t LoraNode_SetPOWER(uint8_t nu)
{	
	uint8_t POWER[20] = "AT+POWER=";
	uint8_t buf[5] = {0};
	char *temp = "OK";
	
	Int2Str(buf, nu);
	StringCat(POWER, buf);

	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(POWER);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}



/*
**	@function:	set debug value
**	@paramters: 0, release; 1, debug
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetDebug(uint32_t value)
{
	uint8_t SetDebug[20] = "AT+DEBUG=";
	uint8_t buf[10] = {0};
	char *temp = "OK";
	
	Int2Str(buf, value);
	StringCat(SetDebug, buf);
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SetDebug);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}


uint32_t LoraNode_SetRADIO(uint32_t f)
{
	static uint8_t SetRADIO[40] = "AT+RADIO=";
	uint8_t buf[10] = {0}; 
	
	char *temp = "OK";
	
	Int2Str(buf, f);
	StringCat(SetRADIO, buf);

	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SetRADIO);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}


uint32_t LoraNode_SetP2P(uint32_t f,uint8_t a,uint8_t b,uint8_t c,uint8_t d,uint8_t e,uint8_t ff,uint8_t g,uint16_t h)
{
	static uint8_t SetDebug[50] = "AT+RADIO=";
	uint8_t buf[10] = {0}; 
	uint8_t buf1[10] = {0}; 
	uint8_t buf2[10] = {0}; 
	uint8_t buf3[10] = {0}; 
	uint8_t buf4[10] = {0}; 
	uint8_t buf5[10] = {0}; 
	uint8_t buf6[10] = {0}; 
	uint8_t buf7[10] = {0}; 
	uint8_t buf8[10] = {0}; 
	
	uint8_t dou[2] = ",";
	char *temp = "OK";
	
	Int2Str(buf, f);
	StringCat2(SetDebug, buf);
	StringCat2(SetDebug, dou);

	Int2Str(buf1, a);
	StringCat2(SetDebug, buf1);
	StringCat2(SetDebug, dou);

	Int2Str(buf2, b);
	StringCat2(SetDebug, buf2);
	StringCat2(SetDebug, dou);

	Int2Str(buf3, c);
	StringCat2(SetDebug, buf3);
	StringCat2(SetDebug, dou);

	Int2Str(buf4, d);
	StringCat2(SetDebug, buf4);
	StringCat2(SetDebug, dou);

	Int2Str(buf5, e);
	StringCat2(SetDebug, buf5);
	StringCat2(SetDebug, dou);

	Int2Str(buf6, ff);
	StringCat2(SetDebug, buf6);
	StringCat2(SetDebug, dou);

	Int2Str(buf7, g);
	StringCat2(SetDebug, buf7);
	StringCat2(SetDebug, dou);

	Int2Str(buf8, h);
	StringCat(SetDebug, buf8);
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SetDebug);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}


/*
**	@function:	set debug value
**	@paramters: 0, release; 1, debug
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetMINI_RF(uint32_t value)
{
	uint8_t MINI[20] = "AT+MINIRF=";
	uint8_t buf[10] = {0}; 
	char *temp = "OK";
	
	Int2Str(buf, value);
	StringCat(MINI, buf);
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(MINI);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}



/*
**	@function:	set appeui 
**	@paramters: appeui 8 bytes
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetAppEUI(uint8_t *dev)
{
	uint8_t SetAppEUI[50] = "AT+APPEUI=";
	char *temp = "OK";
	
	StringCat(SetAppEUI, dev);
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SetAppEUI);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;

}

/*
**	@function:	set appkey, only in otaa mode
**	@paramters: appkey 16 bytes
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetAppKey(uint8_t *key)
{
	uint8_t SetAppKey[80] = "AT+APPKEY="; 
	char *temp = "OK";
	
	StringCat(SetAppKey, key);
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SetAppKey);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}

/*
**	@function:	set appskey, only in abp mode
**	@paramters: appskey 16 bytes
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetAppSKey(uint8_t *skey)
{
	uint8_t SetAppSKey[80] = "AT+APPSKEY=";
	char *temp = "OK";
	
	StringCat(SetAppSKey, skey);
		
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SetAppSKey);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}

/*
**	@function:	set nwkskey, only in abp mode
**	@paramters: appskey 16 bytes
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetNwkSKey(uint8_t *nwk_skey)
{
	uint8_t SetNwkSKey[80] = "AT+NWKSKEY=";
	char *temp = "OK";
	
	StringCat(SetNwkSKey, nwk_skey);
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SetNwkSKey);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}


/*
**	@function:	save config
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_Save(void)
{
	char SAVE[] = "AT+SAVE\n\r";
	char *temp = "OK";
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SAVE);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{	
		return 0;
	}
	
	return -1;
}



void LoraNode_Send_AT(char *at_buf)
{
	LPUART1_SendString(at_buf);
}
		
void LoraNode_Read(uint8_t *str)
{
	uint32_t i = 0;

	LoraNode_Delay(200);

	if(LPUsart1_RX.receive_flag == 1)
	{
		for(i=0 ; i < LPUsart1_RX.rx_len; i++)
		{
			str[i] = LPUsart1_RX.RX_Buf[i];
		}
		
		LPUsart1_RX.receive_flag = 0;
		LPUsart1_RX.rx_len = 0;
	}
}


//-------------------------------------------------------------------------------------------

int8_t LoRaNode_LowPower_Send(uint8_t *buffer, int32_t len,uint8_t CONFIRM)
{
	int8_t ret = 0;
	
	//----����ģ�� ----
	LoraNode_Wake_Sleep(MODE_WAKEUP);

	LoraNode_Delay(10);

	//----�ж�ģ�鴦��͸��ģʽ
	if(LORANODE_MODE_STATUS == 0)	
	{
		//----�ȴ� BUSY ���Ŵ��ڿ���״̬(�ߵ�ƽ)���ȴ�ʱ���Լӳ�ʱ�ж�
		while(LoraNode_IO_State(LORANODE_BUSY_PIN) == GPIO_PIN_RESET)
		{
			if(Time_Out_Break(2000) == 1)
			{
				ret = -1;             // ���� -1  : ģ��û��׼����
				return ret;
			}
		}
		TimeOut_Sign = 0;

		//----ͨ������1������Ҫ���͵�����
		LPUsart1SendData(buffer, len);

		//----�ȴ� BUSY ���Ŵ��ڹ���״̬(�͵�ƽ)���ȴ�ʱ���Լӳ�ʱ�ж�
		while(LoraNode_IO_State(LORANODE_BUSY_PIN) == GPIO_PIN_SET)
		{
			if(Time_Out_Break(2000) == 1)
			{
				ret = -2;             // ���� -2  : ģ��û�н��յ����ڷ��͵����ݣ�����ģ��û�й���
				return ret;
			}
		}
		TimeOut_Sign = 0;

		//-----ϵͳ�ڴ�ʱ��������----------
		Send_Sate_Sign = 1;          //����־λ���ɷ���״̬
		Set_RTC_WakeUp(60);
		LoRaNode_Send_To_Sleep();

		if(Send_Sate_Sign == 11)
		{
			Send_Sate_Sign = 0;
			ret = -3;             // ���� -2  : ģ�鷢�����ݳ����쳣
			return ret;
		}

		if(Send_Sate_Sign == 12)
		{
			Send_Sate_Sign = 0;
			ret = 2;             // ģ���н��յ�����������������
			return ret;
		}

		if(Send_Sate_Sign == 13)
		{
			Send_Sate_Sign = 0;

			if(CONFIRM == 0)
			{
				ret = 1;             // ���ͳɹ�
				return ret;
			}else
				{
					while(LoraNode_IO_State(LORANODE_STAT_PIN) == GPIO_PIN_RESET)
					{
						if(Time_Out_Break(1000) == 1)
						{
							ret = -4;             // ���� -4  : ģ��û�н��յ����ڷ��͵����ݣ�����ģ��û�й���
							return ret;
						}
					}
					TimeOut_Sign = 0;
					ret = 1;
					return ret;
				}
			
		}
		
	}
}

//--------------------------------���ܺ���---------------------------------------------------

/*--------------------���ͺͽ��պ���-----------------------
// ������� :
//           *buffer  ---   Ҫ���͵�����
//           len      ---   Ҫ�������ݵĳ���
//           confirm  ---   �Ƿ���Ҫȷ��֡
// ����״̬��
//           1 --  ���ݷ��ͳɹ�
//			-1 --  ģ����æ��BUSY=0		
//	        -2 --  ��ģ�鴮�ڷ������ݺ�busy�ܽ�û�б��
//			-3 --  ģ�鷢�ͳ�ʱ			
//			-4 --  ģ��û���յ��ظ�	
*/
uint32_t LoraNode_Write_Receive_Bytes(uint8_t *buffer, int32_t len,uint8_t CONFIRM)
{
	uint32_t ret = 0;
	
	//������ձ�־
	//Receive_Log = 0; 
	
	//----����ģ�� ----
	LoraNode_Wake_Sleep(MODE_WAKEUP);

	LoraNode_Delay(10);

	//----�ж�ģ�鴦��͸��ģʽ
	if(LORANODE_MODE_STATUS == 0)	
	{
		//----�ȴ� BUSY ���Ŵ��ڿ���״̬(�ߵ�ƽ)���ȴ�ʱ���Լӳ�ʱ�ж�
		while(LoraNode_IO_State(LORANODE_BUSY_PIN) == GPIO_PIN_RESET)
		{
			if(Time_Out_Break(2000) == 1)
			{
				ret = -1;             // ���� -1  : ģ��û��׼����
				return ret;
			}
		}
		TimeOut_Sign = 0;

		//----ͨ������1������Ҫ���͵�����
		LPUsart1SendData(buffer, len);

		//----�ȴ� BUSY ���Ŵ���æ״̬(�͵�ƽ)���ȴ�ʱ���Լӳ�ʱ�ж�
		while(LoraNode_IO_State(LORANODE_BUSY_PIN) == GPIO_PIN_SET)
		{
			if(Time_Out_Break(2000) == 1)
			{
				ret = -2;             // ���� -2  : ģ��û�н��յ����ڷ��͵����ݣ�����ģ��û�й���
				return ret;
			}
		}
		TimeOut_Sign = 0;

		//���ձ�־ ��1 ��ʼ�ȴ����մ��ڽ�������
		//Receive_Log = 1;
		//Receive_Lenth = 0;

		//----�ȴ����ݷ������  BUSY ���Żص�����״̬(�ߵ�ƽ)���ȴ�ʱ���Լӳ�ʱ�ж�
		while(LoraNode_IO_State(LORANODE_BUSY_PIN) == GPIO_PIN_RESET)
		{
			if(Time_Out_Break(60000) == 1)
			{
				ret = -3;             // ���� -2  : ģ��û�н��յ����ڷ��͵����ݣ�����ģ��û�й���
				return ret;
			}
		}
		TimeOut_Sign = 0;

		if(CONFIRM == 1)
		{
			//�ж�������״̬��ȷ�������ǳɹ���
			while(LoraNode_IO_State(LORANODE_STAT_PIN) == GPIO_PIN_RESET)
			{
				if(Time_Out_Break(2000) == 1)
				{
					ret = -4;             // ���� -2  : ģ��û�н��յ����ڷ��͵����ݣ�����ģ��û�й���
					return ret;
				}
			}
			TimeOut_Sign = 0;
		}
	}

	ret = 1;
	return ret;
}

/* Over-the-Air Activation */
void LoraNode_OTAA_Config(void)
{
	//	����ģʽ�л����� ��ģ���л�������ģʽ����
	LoraNode_Mode_Set(MODE_CMD);
	
	//��������ģʽ���ú��� ��ģ�����ó� OTAA ��ʽ����
	LoraNode_SetNetMode(NET_OTAA);

	//���ú�������ģ��� APPEUI
	LoraNode_SetAppEUI(appEui);
	
	LoraNode_Delay(30);
	
	//���ú�������ģ��� APPKEY
	LoraNode_SetAppKey(appKey);
	
	LoraNode_Delay(30);
	
	//���ñ������ú���
	LoraNode_Save();
	
	LoraNode_Delay(500);
	
	//��������ģ�麯��
	LoraNode_Reset();
	
	LoraNode_Delay(500);
	
//	Uart_Printf("\n ��������ɣ��������鿴ģ����Ϣ�Ƿ���Ч \n");
	
}

void LoraNode_ABP_Config(void)
{	
	//mode pin high
	LoraNode_Mode_Set(MODE_CMD);
	
	//at+otaa=0
	LoraNode_SetNetMode(NET_ABP);
	
	LoraNode_Delay(30);
	
	//appskey
	LoraNode_SetAppSKey(appSKey);	
	
	LoraNode_Delay(30);
	
	//nwk_skey
	LoraNode_SetNwkSKey(nwkSKey);
	
	LoraNode_Delay(30);
	
	//devaddr
	//LoraNode_SetDevAddr(devAddr);
	
	LoraNode_Delay(50);
	
	//save
	LoraNode_Save();
	
	/*delay for the whole response by USART2 */
	LoraNode_Delay(500);
	
	//reset
	LoraNode_Reset();

	LoraNode_Delay(500);

}



void GET_LoraNode_Info(Node_Info *lora_temp)
{
	LoraNode_Mode_Set(MODE_CMD);	
	
	LoraNode_Wake_Sleep(MODE_WAKEUP);
	
	LoraNode_Delay(100);
	
	/* read some info after lora module initial */	
	LoraNode_GetDevEUI(lora_temp);
	
	LoraNode_Delay(10);
	
	LoraNode_GetAppEUI(lora_temp);	
	
	LoraNode_Delay(10);
	
	LoraNode_GetAppKey(lora_temp);	

	LoraNode_Delay(10);
	
	LoraNode_GetDevAddr(lora_temp);	

	LoraNode_Delay(10);	
	
	LoraNode_GetAppSKey(lora_temp);

	LoraNode_Delay(10);	
	
	LoraNode_GetNwkSKey(lora_temp);

	LoraNode_Delay(10);	

}


void LoraNode_STA_Check(Node_Info *LoRa_temp)
{
	static uint8_t online_log = 0;
	if(LORANODE_STAT_STATUS == 0)
	{
		LoRa_temp->ONline = 0;
	}else
		{
			LoRa_temp->ONline = 1;
		}

	if(LORANODE_BUSY_STATUS == 0)
	{
		LoRa_temp->BUSY = 0;
	}else
		{
			LoRa_temp->BUSY = 1;
		}

	if(LoRa_temp->NET_Mode == 1)
	{
		if(LoRa_temp->ONline == 1)
		{
			if(LoRa_temp->BUSY == 1)
			{
				LoRa_temp->Join = 1;
			}
		}else{
				LoRa_temp->Join = 0;
			 }
	}else if(LoRa_temp->NET_Mode == 0)
		{
			if(LoRa_temp->BUSY == 1)
			{
				LoRa_temp->Join = 1;
			}else{
				LoRa_temp->Join = 0;
			 }
		}

	if(LoRa_temp->Join != online_log )
	{
		if(LoRa_temp->Join == 1)
		{
			DEBUG_Printf("\r\n--> ����������\r\n");
		}else{
				//Uart_Printf("\r\n �����ѶϿ����� \r\n");
			}
	}
			
		online_log = LoRa_temp->Join;
}


//--------------------------------------------------------------------------------------------

uint32_t LoraNode_IO_State(uint32_t pin)
{
	return HAL_GPIO_ReadPin(LORANODE_GPIO_PORT, pin);	
}

void LoraNode_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}


char *StringStr(char *str, char *dest)
{
#define STR_BUFF_LEN	0x100
	int i = STR_BUFF_LEN;
	char *cp = str;
	char *s1, *s2;
	
	if (*dest == 0)
	{
		return str;
	}
	
//	while(*str)
	while(i--)
	{		
		s1 = cp;
		s2 = dest;
		
		while((*s1 == *s2) && *s1 && *s2)
		{
			s1++;
			s2++;
		}
		if(!*s2)
			return cp;
		cp++;
	}
	
	return NULL;
}

uint8_t StrToHex(uint8_t temp)
{
	uint8_t ret=0;
	if(temp>=48 && temp<=57)
	{
		ret = temp - 48;
		return ret;
	}

	if(temp>=65 && temp<=70)
	{
		ret = temp - 55;
		return ret;
	}

	if(temp>=97 && temp<=102)
	{
		ret = temp - 87;
		return ret;
	}
}




///-----------------------------------------------
void Printf_LoRaNode_Info(Node_Info LoRa_temp)
{
	DEBUG_Printf("DEVEUI: %x %x %x %x %x %x %x %x \r\n",LoRa_temp.DevEUI[0],LoRa_temp.DevEUI[1],LoRa_temp.DevEUI[2],LoRa_temp.DevEUI[3],LoRa_temp.DevEUI[4],LoRa_temp.DevEUI[5],LoRa_temp.DevEUI[6],LoRa_temp.DevEUI[7]);
	DEBUG_Printf("APPEUI: %x %x %x %x %x %x %x %x \r\n",LoRa_temp.AppEUI[0],LoRa_temp.AppEUI[1],LoRa_temp.AppEUI[2],LoRa_temp.AppEUI[3],LoRa_temp.AppEUI[4],LoRa_temp.AppEUI[5],LoRa_temp.AppEUI[6],LoRa_temp.AppEUI[7]);
	DEBUG_Printf("APPKEY: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \r\n",LoRa_temp.AppKEY[0],LoRa_temp.AppKEY[1],LoRa_temp.AppKEY[2],LoRa_temp.AppKEY[3],LoRa_temp.AppKEY[4],LoRa_temp.AppKEY[5],LoRa_temp.AppKEY[6],LoRa_temp.AppKEY[7],
																			 		   LoRa_temp.AppKEY[8],LoRa_temp.AppKEY[9],LoRa_temp.AppKEY[10],LoRa_temp.AppKEY[11],LoRa_temp.AppKEY[12],LoRa_temp.AppKEY[13],LoRa_temp.AppKEY[14],LoRa_temp.AppKEY[15]);
	DEBUG_Printf("APPSKEY: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \r\n",LoRa_temp.AppSKEY[0],LoRa_temp.AppSKEY[1],LoRa_temp.AppSKEY[2],LoRa_temp.AppSKEY[3],LoRa_temp.AppSKEY[4],LoRa_temp.AppSKEY[5],LoRa_temp.AppSKEY[6],LoRa_temp.AppSKEY[7],
																			 		   LoRa_temp.AppSKEY[8],LoRa_temp.AppSKEY[9],LoRa_temp.AppSKEY[10],LoRa_temp.AppSKEY[11],LoRa_temp.AppSKEY[12],LoRa_temp.AppSKEY[13],LoRa_temp.AppSKEY[14],LoRa_temp.AppSKEY[15]);
	DEBUG_Printf("NWKSKEY: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \r\n",LoRa_temp.NwkSKEY[0],LoRa_temp.NwkSKEY[1],LoRa_temp.NwkSKEY[2],LoRa_temp.NwkSKEY[3],LoRa_temp.NwkSKEY[4],LoRa_temp.NwkSKEY[5],LoRa_temp.NwkSKEY[6],LoRa_temp.NwkSKEY[7],
																			 		   LoRa_temp.NwkSKEY[8],LoRa_temp.NwkSKEY[9],LoRa_temp.NwkSKEY[10],LoRa_temp.NwkSKEY[11],LoRa_temp.NwkSKEY[12],LoRa_temp.NwkSKEY[13],LoRa_temp.NwkSKEY[14],LoRa_temp.NwkSKEY[15]);
	DEBUG_Printf("DEVADDR: %x %x %x %x \r\n",LoRa_temp.DevADDR[0],LoRa_temp.DevADDR[1],LoRa_temp.DevADDR[2],LoRa_temp.DevADDR[3]);
}
//-------------------------------------------------

