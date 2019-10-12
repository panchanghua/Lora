#ifndef __MAX7Q_H_
#define __MAX7Q_H_

#include "stdint.h"
/**/
typedef uint8_t	u8 ;
typedef uint16_t u16;
typedef uint32_t u32;

#define GPS_ON  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET)
#define GPS_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET)


//GPS NMEA-0183Э����Ҫ�����ṹ�嶨�� 
 
//UTCʱ����Ϣ
typedef struct  
{										    
 	u16 year;	//���
	u8 month;	//�·�
	u8 date;	//����
	u8 hour; 	//Сʱ
	u8 min; 	//����
	u8 sec; 	//����
}nmea_utc_time;   	   
//NMEA 0183 Э����������ݴ�Žṹ��

typedef struct{		
	u8  Gps_Sta;
	nmea_utc_time UTC;			//UTCʱ��
	u32 Latitude;				//γ�� ������100000��,ʵ��Ҫ����100000
	u8 	NS;					//��γ/��γ,N:��γ;S:��γ				  
	u32 Longitude;			    //���� ������100000��,ʵ��Ҫ����100000
	u8 	EW;					//����/����,E:����;W:����				  
 	u8 	Posslnum;				//���ڶ�λ��������,0~12.
	int Altitude;			 	//���θ߶�,�Ŵ���10��,ʵ�ʳ���10.��λ:0.1m	  
}GPS_DATA; 
//////////////////////////////////////////////////////////////////////////////////////////////////// 	

 
				 
int NMEA_Str2num(u8 *buf,u8*dx);
void GPS_Analysis(GPS_DATA *gpsx,u8 *buf);
void NMEA_GPGGA_Analysis(GPS_DATA *gpsx,u8 *buf);
void NMEA_GPRMC_Analysis(GPS_DATA *gpsx,u8 *buf);
void GPS_Init(void);
#endif	