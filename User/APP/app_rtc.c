#include "includes.h"
#include "./RTC/bsp_rtc.h"
#include "./LCD/bsp_lcd.h"
#include "app_can_tx.h"

//RTC
OS_TCB   AppTaskRtcTCB;
CPU_STK  AppTaskRtcStk[APP_TASK_RTC_STK_SIZE];
void  AppTaskRtc   (void *p_arg);

//默认初始时间
struct tm time_now = { 0,0,0,14,3,2018 };

void  AppTaskRtc (void *p_arg)
{
	OS_ERR 		err;
	char		arrayStatus[80];
    char        arrayCan[8];
    uint8_t     systemmode = NONE;
	
	if ( BSP_GetUnixTime(  ) <= BSP_ConvCalendarToUnix( time_now ) )
        BSP_SetCalendarTime( time_now );
	
	while ( 1 )
	{
		OSTaskSemPend ( ( OS_TICK	)	0,
                        ( OS_OPT	)	OS_OPT_PEND_BLOCKING,
                        ( CPU_TS   *)	NULL,
                        ( OS_ERR   *)	&err);
        
        //获取RTC时钟
		time_now = BSP_GetClendarTime( );
        systemmode = getSystemMode(  );         
        
        
        switch ( systemmode )
        {
            case NONE:      sprintf( arrayCan, "NONE");
                break;
            case MASTER:    sprintf( arrayCan, "MASTER");
                break;
            case SLAVE:     sprintf( arrayCan, "SLAVE");
                break;
        }
		sprintf( arrayStatus, "%04d-%02d-%02d   %02d:%02d:%02d  %s",
                                                    time_now.tm_year,
                                                    time_now.tm_mon,
                                                    time_now.tm_mday,
                                                    time_now.tm_hour, 
                                                    time_now.tm_min,
                                                    time_now.tm_sec,
                                                    arrayCan            );
        
        //显示输出
		BSP_LCD_ShowStringNotStack( 0, 0, 240, 16, 16, (uint8_t *)arrayStatus );
	}
}




