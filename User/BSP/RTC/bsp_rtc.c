#include "includes.h"
#include "./RTC/bsp_rtc.h"
#include "time.h"

extern  OS_TCB  AppTaskRtcTCB;
extern  uint8_t gcanMasterRx;       //Can模式计时
extern  uint8_t gcanMasterTx;       //Can主机心跳
extern  uint8_t gBoardPing;         //面板心跳

tRTCStatus gRTCStatus = RTC_NONE;

void BSP_RTC_Config (void)
{
    uint32_t    time = 0;
    
    //打开总线上的PWR、BKP时钟
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR | RCC_APB1Periph_BKP , ENABLE );
    
    //使能RTC和后备寄存器访问
    PWR_BackupAccessCmd(ENABLE);
   
    //复位：备份寄存器设置
    BKP_DeInit();
    
    //开启LSE时钟，外部低速，并等待LSE起振
    RCC_LSEConfig(RCC_LSE_ON);
    while( RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET && time<RCC_LSE_TIMEOUT )
        time++;
    
    if (time>=RCC_LSE_TIMEOUT)
    {
        gRTCStatus = RTC_LSE_ERROR;
        return ;
    }
    
    //选择LSE作为时钟源，并使能RTC时钟
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);                     
    RCC_RTCCLKCmd(ENABLE);
    
    //等待RTC寄存器同步完成
    RTC_WaitForSynchro();
    RTC_WaitForLastTask();
    
    //使能秒中断
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    RTC_WaitForLastTask();
    
    //设置RTC分频
    RTC_SetPrescaler(32767);
    RTC_WaitForLastTask();
    
    gRTCStatus = RTC_LSE_SUCCESS;
}

void BSP_RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel                      = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//设置RTC的时间
void BSP_SetCalendarTime ( struct tm t )
{
    //设置需要Unix时间
    BSP_SetUnixTime ( BSP_ConvCalendarToUnix(t) );
}

//BCD转Unix时间
uint32_t BSP_ConvCalendarToUnix( struct tm t )
{
    t.tm_year -= 1900;
    return mktime(&t);   
}

//获得RTC中的Unix时间
uint32_t BSP_GetUnixTime (void)
{
    return RTC_GetCounter();
}

//获得RTC中的BCD时间
struct tm BSP_GetClendarTime (void)
{
    return BSP_ConvUnixToClendar( BSP_GetUnixTime() );
}


//Unix转BCD时间
struct tm BSP_ConvUnixToClendar (uint32_t t)
{
    struct tm *t_tm;
    t_tm = localtime(&t);
    t_tm->tm_year +=1900;
    return *t_tm;
}

//设置RTC中的Unix时间
void BSP_SetUnixTime ( uint32_t t )
{
    //已经正确设置时钟源
    if ( gRTCStatus == RTC_LSE_SUCCESS || gRTCStatus == RTC_LSI_SUCCESS )
    {
        RTC_WaitForLastTask();
        RTC_SetCounter( t );
        RTC_WaitForLastTask();
    }
}

void RTC_IRQHandler (void)
{
	OS_ERR      err;
	OSIntEnter();
	if ( RTC_GetITStatus(RTC_IT_SEC) != RESET )
	{
        RTC_ClearITPendingBit(RTC_IT_SEC);
        
        //任务信号量
        OSTaskSemPost (	(OS_TCB	*) &AppTaskRtcTCB,
                        (OS_OPT	 ) OS_OPT_POST_NO_SCHED,
                        (OS_ERR	*) &err);
        
        gcanMasterRx++;     //接收主机心跳计时——全局变量
		gcanMasterTx++;     //发送主机心跳计时——全局变量
        gBoardPing++;       //面板心跳
	}
	OSIntExit();
}




