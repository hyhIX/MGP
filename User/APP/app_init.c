#include "includes.h"
#include "app_init.h"
#include "./EEPROM/bsp_eeprom.h"
#include "./CRC/bsp_crc.h"
#include "string.h"


static tAppEepromInit	sAppEepromInit;
extern __IO uint32_t    gCRCValue;

static void APP_Set_CRC(void);


/*
*   ������:APP_Set_CRC
*   ����:  У�鲢����CRC32
*   ����:
*   ���:
*/
static void APP_Set_CRC(void)
{
	sAppEepromInit.Crc32 = APP_Check_CRC();
}

/*
*   ������:APP_EEPROM_Init_Read
*   ����:  ��ȡ��ʼ���ṹ��   
*   ����:  
*   ���:  
*/
void APP_EEPROM_Init_Read(void)
{
	APP_Init_ReSetting();           //��ʼ��sAppEepromInit����
	AT24CXX_Read(0, (uint8_t *)&sAppEepromInit, sizeof(tAppEepromInit));
}

/*
*   ������:APP_EEPROM_Init_Write
*   ����:  д���ʼ���ṹ��(ȷ��д��д����ȷ)   
*   ����:  
*   ���:
*/
void APP_EEPROM_Init_Write(void)
{
    APP_Set_CRC();
	AT24CXX_Write(0, (uint8_t *)&sAppEepromInit, sizeof(tAppEepromInit));
}

/*
*   ������:APP_Set_TouchAdjust
*   ����:  ���ô�����У׼����
*   ����:  TouchAdjustУ׼�����ṹ��
*   ���:
*/
void APP_Set_TouchAdjust(tTouchAdjust TouchAdjust)
{
	sAppEepromInit.TouchAdjust = TouchAdjust;
    APP_EEPROM_Init_Write();
}

/*
*   ������:APP_Set_Passwd
*   ����:  ������֤���룬������CRC32
*   ����:  Passwd��֤����
*   ���:
*/
void APP_Set_Passwd(uint16_t Passwd)
{
	sAppEepromInit.Passwd = Passwd;
}

/*
*   ������:APP_Set_RfidID_Management
*   ����:  ����RFID�����к�
*   ����:  RfidNum��������RfidID�����к�
*   ���:
*/
void APP_Set_RfidID_Management(uint8_t RfidNum, uint32_t RfidID[])
{
	sAppEepromInit.RfidNum = RfidNum;

	for(uint8_t i=0; i<RfidNum; i++)
		sAppEepromInit.RfidID[i] = RfidID[i];
}

/*
*   ������:APP_Set_CAN_FilterInitStructure
*   ����:  ����CAN���߹�����
*   ����:  CanFilterNum������������CAN_FilterInitStructure
*   ���:
*/
void APP_Set_CAN_FilterInitStructure(uint8_t CanFilterNum, CAN_FilterInitTypeDef *CAN_FilterInitStructure)
{
    sAppEepromInit.CanFilterNum = CanFilterNum;
    
	for (uint8_t i=0; i<CanFilterNum; i++)
		sAppEepromInit.CAN_FilterInitStruct[i] = CAN_FilterInitStructure[i];
}

/*
*   ������:APP_Init_ReSetting
*   ����:  ����������ϢsAppEepromInit
*   ����:  
*   ���:
*/
void APP_Init_ReSetting(void)
{
	memset(&sAppEepromInit, 0, sizeof(tAppEepromInit));
}

/*
*   ������:APP_Check_CRC
*   ����:  ��������ϢsAppEepromInit���ų�����֮�󣬽���crc����
*   ����:  
*   ���: crcУ���ֵ
*/
uint32_t APP_Check_CRC(void)
{
	return BSP_CalcBlockCRC( (uint32_t *)&sAppEepromInit.Passwd, sizeof(tAppEepromInit)-4);
}

/*
*   ������:APP_Get_CRC
*   ����:  ��ȡ������ϢsAppEepromInit�е�Crc32
*   ����:  
*   ���: sAppEepromInit.Crc32
*/
uint32_t APP_Get_CRC(void)
{
	return sAppEepromInit.Crc32;
}

/*
*   ������:APP_Get_TouchAdjust
*   ����:  ��ȡ������ϢsAppEepromInit�еĴ�����У׼����
*   ����:  
*   ���: ��������У׼����
*/
tTouchAdjust APP_Get_TouchAdjust(void)
{
	return sAppEepromInit.TouchAdjust;
}

/*
*   ������:APP_Get_Passwd
*   ����:  ��ȡ������ϢsAppEepromInit�е������֤����
*   ����:  
*   ���: �����֤����
*/
uint16_t APP_Get_Passwd(  void)
{
	return sAppEepromInit.Passwd;
}

/*
*   ������:APP_Get_RfidID_Management
*   ����:  ��ȡ������ϢsAppEepromInit�еĿ����к�
*   ����:  RfidID�洢�����кŵĵ�ַ
*   ���: �����кŵ�����
*/
uint8_t APP_Get_RfidID_Management(uint32_t RfidID[])
{
	for (uint8_t i=0; i<sAppEepromInit.RfidNum; i++)
		RfidID[i] = sAppEepromInit.RfidID[i];

	return sAppEepromInit.RfidNum;
}

/*
*   ������:APP_Get_CAN_FilterInitStructure
*   ����:  ��ȡ������ϢsAppEepromInit�е�CAN���߹�����������
*   ����:  CAN_FilterInitStructure���������õĵ�ַ
*   ���: ���������õ�����
*/
uint8_t APP_Get_CAN_FilterInitStructure(CAN_FilterInitTypeDef *CAN_FilterInitStructure)
{
	for (uint8_t i=0; i<sAppEepromInit.CanFilterNum; i++)
        CAN_FilterInitStructure[i] = sAppEepromInit.CAN_FilterInitStruct[i];

	return sAppEepromInit.CanFilterNum;
}




