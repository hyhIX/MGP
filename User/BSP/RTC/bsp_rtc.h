#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__

#include "stm32f10x.h"
#include "time.h"

#define     RCC_LSE_TIMEOUT         1000000

typedef enum
{
    RTC_NONE,
    RTC_LSE_SUCCESS,
    RTC_LSE_ERROR,
    RTC_LSI_SUCCESS,
    RTC_LSI_ERROR,
} tRTCStatus;

void BSP_RTC_Config (void);                            //��ʼ��
void BSP_RTC_NVIC_Config(void);                        //RTC�ж�

void BSP_SetCalendarTime ( struct tm t );              //ʹ��BCDʱ������ʱ��
void BSP_SetUnixTime ( time_t t );                     //����Unixʱ��

uint32_t BSP_ConvCalendarToUnix( struct tm t );        //BCDʱ��ת��Unixʱ��
struct tm BSP_ConvUnixToClendar (uint32_t t);          //Unixʱ��ת��BCDʱ��

uint32_t BSP_GetUnixTime (void);                       //���Unixʱ��
struct tm BSP_GetClendarTime (void);


#endif



