#include "includes.h"
#include "./RTC/bsp_rtc.h"
#include "time.h"

extern  OS_TCB  AppTaskRtcTCB;
extern  uint8_t gcanMasterRx;       //Canģʽ��ʱ
extern  uint8_t gcanMasterTx;       //Can��������
extern  uint8_t gBoardPing;         //�������

tRTCStatus gRTCStatus = RTC_NONE;

void BSP_RTC_Config (void)
{
    uint32_t    time = 0;
    
    //�������ϵ�PWR��BKPʱ��
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR | RCC_APB1Periph_BKP , ENABLE );
    
    //ʹ��RTC�ͺ󱸼Ĵ�������
    PWR_BackupAccessCmd(ENABLE);
   
    //��λ�����ݼĴ�������
    BKP_DeInit();
    
    //����LSEʱ�ӣ��ⲿ���٣����ȴ�LSE����
    RCC_LSEConfig(RCC_LSE_ON);
    while( RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET && time<RCC_LSE_TIMEOUT )
        time++;
    
    if (time>=RCC_LSE_TIMEOUT)
    {
        gRTCStatus = RTC_LSE_ERROR;
        return ;
    }
    
    //ѡ��LSE��Ϊʱ��Դ����ʹ��RTCʱ��
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);                     
    RCC_RTCCLKCmd(ENABLE);
    
    //�ȴ�RTC�Ĵ���ͬ�����
    RTC_WaitForSynchro();
    RTC_WaitForLastTask();
    
    //ʹ�����ж�
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    RTC_WaitForLastTask();
    
    //����RTC��Ƶ
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

//����RTC��ʱ��
void BSP_SetCalendarTime ( struct tm t )
{
    //������ҪUnixʱ��
    BSP_SetUnixTime ( BSP_ConvCalendarToUnix(t) );
}

//BCDתUnixʱ��
uint32_t BSP_ConvCalendarToUnix( struct tm t )
{
    t.tm_year -= 1900;
    return mktime(&t);   
}

//���RTC�е�Unixʱ��
uint32_t BSP_GetUnixTime (void)
{
    return RTC_GetCounter();
}

//���RTC�е�BCDʱ��
struct tm BSP_GetClendarTime (void)
{
    return BSP_ConvUnixToClendar( BSP_GetUnixTime() );
}


//UnixתBCDʱ��
struct tm BSP_ConvUnixToClendar (uint32_t t)
{
    struct tm *t_tm;
    t_tm = localtime(&t);
    t_tm->tm_year +=1900;
    return *t_tm;
}

//����RTC�е�Unixʱ��
void BSP_SetUnixTime ( uint32_t t )
{
    //�Ѿ���ȷ����ʱ��Դ
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
        
        //�����ź���
        OSTaskSemPost (	(OS_TCB	*) &AppTaskRtcTCB,
                        (OS_OPT	 ) OS_OPT_POST_NO_SCHED,
                        (OS_ERR	*) &err);
        
        gcanMasterRx++;     //��������������ʱ����ȫ�ֱ���
		gcanMasterTx++;     //��������������ʱ����ȫ�ֱ���
        gBoardPing++;       //�������
	}
	OSIntExit();
}




