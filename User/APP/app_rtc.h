#ifndef __APP_RTC_H__
#define	__APP_RTC_H__

#include <includes.h>
#include <time.h>

void RTC_Config (void);
void RTC_NVIC_Config(void);
void Time_SetCalendarTime ( struct tm t );
u32 Time_ConvCalendarToUnix( struct tm t );
u32 Time_GetUnixTime (void);
struct tm Time_ConvUnixToClendar (time_t t);
void Time_SetUnixTime ( time_t t );


#endif
