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

void BSP_RTC_Config (void);                            //初始化
void BSP_RTC_NVIC_Config(void);                        //RTC中断

void BSP_SetCalendarTime ( struct tm t );              //使用BCD时间设置时间
void BSP_SetUnixTime ( time_t t );                     //设置Unix时间

uint32_t BSP_ConvCalendarToUnix( struct tm t );        //BCD时间转换Unix时间
struct tm BSP_ConvUnixToClendar (uint32_t t);          //Unix时间转换BCD时间

uint32_t BSP_GetUnixTime (void);                       //获得Unix时间
struct tm BSP_GetClendarTime (void);


#endif



