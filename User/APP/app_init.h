#ifndef __APP_INIT_H__
#define __APP_INIT_H__

#include "includes.h"


typedef struct 
{
	int32_t	    TouchXfac;      //x校准参数
	int32_t	    TouchYfac;      //y校准参数
	int16_t	    TouchXoff;      //x偏移量
	int16_t 	TouchYoff;      //y偏移量
    uint8_t     TouchStatus;    //校准标志  0x0A
    uint8_t     TouchType;      //屏幕类型
    uint8_t     Reserved[2];    //保留
}tTouchAdjust;


typedef struct
{
	uint32_t				Crc32;
  	uint16_t				Passwd;
	uint8_t					RfidNum;		//0-10	10:满	现有的ID数	
	uint8_t					CanFilterNum;	//0-14	14:满	现有的Filter数
	tTouchAdjust			TouchAdjust;
	uint32_t				RfidID[9];					//RfidNum指向空的位置
	CAN_FilterInitTypeDef	CAN_FilterInitStruct[14];	//CanFilterNum指向空的位置
} tAppEepromInit;


#define		TRUE	1
#define		FALSE	0


void APP_EEPROM_Init_Read(void);
void APP_EEPROM_Init_Write(void);
void APP_Set_TouchAdjust(tTouchAdjust TouchAdjust);
void APP_Set_Passwd(uint16_t Passwd);
void APP_Set_RfidID_Management(uint8_t RfidNum, uint32_t RfidID[]);
void APP_Set_CAN_FilterInitStructure(uint8_t CanFilterNum, CAN_FilterInitTypeDef *CAN_FilterInitStructure);
void APP_Init_ReSetting(void);


tTouchAdjust APP_Get_TouchAdjust(void);
uint32_t APP_Check_CRC(void);
uint32_t APP_Get_CRC(void);
uint16_t APP_Get_Passwd(void);
uint8_t  APP_Get_RfidID_Management(uint32_t RfidID[]);
uint8_t  APP_Get_CAN_FilterInitStructure(CAN_FilterInitTypeDef *CAN_FilterInitStructure);


#endif

