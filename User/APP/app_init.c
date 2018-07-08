#include "includes.h"
#include "app_init.h"
#include "./EEPROM/bsp_eeprom.h"
#include "./CRC/bsp_crc.h"
#include "string.h"


static tAppEepromInit	sAppEepromInit;
extern __IO uint32_t    gCRCValue;

static void APP_Set_CRC(void);


/*
*   函数名:APP_Set_CRC
*   描述:  校验并设置CRC32
*   输入:
*   输出:
*/
static void APP_Set_CRC(void)
{
	sAppEepromInit.Crc32 = APP_Check_CRC();
}

/*
*   函数名:APP_EEPROM_Init_Read
*   描述:  读取初始化结构体   
*   输入:  
*   输出:  
*/
void APP_EEPROM_Init_Read(void)
{
	APP_Init_ReSetting();           //初始化sAppEepromInit变量
	AT24CXX_Read(0, (uint8_t *)&sAppEepromInit, sizeof(tAppEepromInit));
}

/*
*   函数名:APP_EEPROM_Init_Write
*   描述:  写入初始化结构体(确保写入写入正确)   
*   输入:  
*   输出:
*/
void APP_EEPROM_Init_Write(void)
{
    APP_Set_CRC();
	AT24CXX_Write(0, (uint8_t *)&sAppEepromInit, sizeof(tAppEepromInit));
}

/*
*   函数名:APP_Set_TouchAdjust
*   描述:  设置触摸屏校准参数
*   输入:  TouchAdjust校准参数结构体
*   输出:
*/
void APP_Set_TouchAdjust(tTouchAdjust TouchAdjust)
{
	sAppEepromInit.TouchAdjust = TouchAdjust;
    APP_EEPROM_Init_Write();
}

/*
*   函数名:APP_Set_Passwd
*   描述:  设置验证密码，并设置CRC32
*   输入:  Passwd验证密码
*   输出:
*/
void APP_Set_Passwd(uint16_t Passwd)
{
	sAppEepromInit.Passwd = Passwd;
}

/*
*   函数名:APP_Set_RfidID_Management
*   描述:  设置RFID卡序列号
*   输入:  RfidNum卡数量，RfidID卡序列号
*   输出:
*/
void APP_Set_RfidID_Management(uint8_t RfidNum, uint32_t RfidID[])
{
	sAppEepromInit.RfidNum = RfidNum;

	for(uint8_t i=0; i<RfidNum; i++)
		sAppEepromInit.RfidID[i] = RfidID[i];
}

/*
*   函数名:APP_Set_CAN_FilterInitStructure
*   描述:  设置CAN总线过滤器
*   输入:  CanFilterNum过滤器数量，CAN_FilterInitStructure
*   输出:
*/
void APP_Set_CAN_FilterInitStructure(uint8_t CanFilterNum, CAN_FilterInitTypeDef *CAN_FilterInitStructure)
{
    sAppEepromInit.CanFilterNum = CanFilterNum;
    
	for (uint8_t i=0; i<CanFilterNum; i++)
		sAppEepromInit.CAN_FilterInitStruct[i] = CAN_FilterInitStructure[i];
}

/*
*   函数名:APP_Init_ReSetting
*   描述:  重置配置信息sAppEepromInit
*   输入:  
*   输出:
*/
void APP_Init_ReSetting(void)
{
	memset(&sAppEepromInit, 0, sizeof(tAppEepromInit));
}

/*
*   函数名:APP_Check_CRC
*   描述:  对配置信息sAppEepromInit，排除自身之后，进行crc计算
*   输入:  
*   输出: crc校验的值
*/
uint32_t APP_Check_CRC(void)
{
	return BSP_CalcBlockCRC( (uint32_t *)&sAppEepromInit.Passwd, sizeof(tAppEepromInit)-4);
}

/*
*   函数名:APP_Get_CRC
*   描述:  获取配置信息sAppEepromInit中的Crc32
*   输入:  
*   输出: sAppEepromInit.Crc32
*/
uint32_t APP_Get_CRC(void)
{
	return sAppEepromInit.Crc32;
}

/*
*   函数名:APP_Get_TouchAdjust
*   描述:  获取配置信息sAppEepromInit中的触摸屏校准参数
*   输入:  
*   输出: 触摸屏的校准参数
*/
tTouchAdjust APP_Get_TouchAdjust(void)
{
	return sAppEepromInit.TouchAdjust;
}

/*
*   函数名:APP_Get_Passwd
*   描述:  获取配置信息sAppEepromInit中的身份认证密码
*   输入:  
*   输出: 身份认证密码
*/
uint16_t APP_Get_Passwd(  void)
{
	return sAppEepromInit.Passwd;
}

/*
*   函数名:APP_Get_RfidID_Management
*   描述:  获取配置信息sAppEepromInit中的卡序列号
*   输入:  RfidID存储卡序列号的地址
*   输出: 卡序列号的数量
*/
uint8_t APP_Get_RfidID_Management(uint32_t RfidID[])
{
	for (uint8_t i=0; i<sAppEepromInit.RfidNum; i++)
		RfidID[i] = sAppEepromInit.RfidID[i];

	return sAppEepromInit.RfidNum;
}

/*
*   函数名:APP_Get_CAN_FilterInitStructure
*   描述:  获取配置信息sAppEepromInit中的CAN总线过滤器的配置
*   输入:  CAN_FilterInitStructure过滤器配置的地址
*   输出: 过滤器配置的数量
*/
uint8_t APP_Get_CAN_FilterInitStructure(CAN_FilterInitTypeDef *CAN_FilterInitStructure)
{
	for (uint8_t i=0; i<sAppEepromInit.CanFilterNum; i++)
        CAN_FilterInitStructure[i] = sAppEepromInit.CAN_FilterInitStruct[i];

	return sAppEepromInit.CanFilterNum;
}




