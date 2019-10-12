#include"Production_Test.h"
#include "hdc1000.h"
#include "usart.h"
#include "opt3001.h"
#include "MPL3115.h"
#include "mma8451.h"
#include "lorawan.h"

extern Node_Info LoRa_Node;
extern Node_Info *LoRa_Node_str;

int8_t Error_num = 0;

void Test_task(void)
{
	HDC1000_Test();
	OPT3003_Test();
	MPL3115_Test();
	MMA8451_Test();
	LORA_NODE_Test();
}

void HDC1000_Test(void)
{
	uint16_t temper, humi;

	temper = HDC1000_Read_Temper();
	humi = HDC1000_Read_Humidi();

	if(Error_num == 0)
	{
		if(temper !=0 || humi!=0)
		{
			DEBUG_Printf("��ʪ�ȴ��������� �¶�: %.3f ��   ʪ��: %.3f % \r\n",(float)temper/1000.0,(float)humi/1000.0);
		}else
			{
				Error_num = -13;
				DEBUG_Printf("��ʪ�ȴ������쳣  error:d% \r\n",Error_num);
			}
	}else
		{
			DEBUG_Printf("��ʪ�ȴ������쳣  error:d% \r\n",Error_num);
		}

	Error_num = 0;
}

void OPT3003_Test(void)
{
	float lux;
	uint16_t result;
	 	
	result = OPT3001_Result();
	
	lux = 0.01*(1 << ((result & 0xF000) >> 12))*(result & 0xFFF);

	if(Error_num == 0)
	{
		if(lux !=0 )
		{
			DEBUG_Printf("�նȴ��������� �ն�: %.2f Lux \r\n",lux);
		}else
			{
				Error_num = -16;
				DEBUG_Printf("�նȴ������쳣  error:d% \r\n",Error_num);
			}
	}else
		{
			DEBUG_Printf("�նȴ������쳣  error:d% \r\n",Error_num);
		}

	Error_num = 0;
	
}

void MPL3115_Test(void)
{
	float pressure;

	pressure = MPL3115_ReadPressure();

	if(Error_num == 0)
	{
		if(pressure !=0 )
		{
			DEBUG_Printf("��ѹ���������� ��ѹ: %.2f Pa \r\n",pressure);
		}else
			{
				Error_num = -18;
				DEBUG_Printf("��ѹ�������쳣  error:d% \r\n",Error_num);
			}
	}else
		{
			DEBUG_Printf("��ѹ�������쳣  error:d% \r\n",Error_num);
		}

	Error_num = 0;
}


void MMA8451_Test(void)
{
	ACCELER_T tAccel;

	tAccel.accel_x = 999;
	tAccel.accel_y = 999;
	tAccel.accel_z = 999;
		
	tAccel = MMA8451_ReadAcceleration();

	if(Error_num == 0)
	{
		if(tAccel.accel_x !=999 || tAccel.accel_y !=999)
		{
			DEBUG_Printf("���ٶȴ��������� X: %d  Y: %d  Z: %d  \r\n",tAccel.accel_x,tAccel.accel_y,tAccel.accel_z);
		}else
			{
				Error_num = -20;
				DEBUG_Printf("���ٶȴ������쳣  error:d% \r\n",Error_num);
			}
	}else
		{
			DEBUG_Printf("���ٶȴ������쳣  error:d% \r\n",Error_num);
		}

	Error_num = 0;

}

void LORA_NODE_Test(void)
{
	GET_LoraNode_Info(LoRa_Node_str);
	Printf_LoRaNode_Info(LoRa_Node);
	LoraNode_SetGPIO(255,0);
	HAL_Delay(1000);
	LoraNode_SetGPIO(255,1);
	LoraNode_Save();
}