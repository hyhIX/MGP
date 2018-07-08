#include <includes.h>
#include "app_init.h"
#include "app_can_tx.h"
#include "app_esp8266.h"

#include "./LED/bsp_led.h"
#include "./BUZZER/bsp_buzzer.h"


static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  void  AppTaskStart  (void *p_arg);

extern  OS_TCB   AppTaskRtcTCB;
extern  CPU_STK  AppTaskRtcStk[APP_TASK_RTC_STK_SIZE];
extern  void  AppTaskRtc   (void *p_arg);

extern  OS_TCB   AppTaskCoreTCB;
extern  CPU_STK  AppTaskCoreStk[APP_TASK_CORE_STK_SIZE];
extern  void  AppTaskCore   (void *p_arg);

extern  OS_TCB   AppTaskCanMasterTCB;
extern  CPU_STK  AppTaskCanMasterStk[APP_TASK_CAN_MASTER_STK_SIZE];
extern  void  AppTaskCanMaster   (void *p_arg);

extern  OS_TCB   AppTaskEsp8266TCB;
extern  CPU_STK  AppTaskEsp8266Stk[APP_TASK_ESP8266_STK_SIZE];
extern  void  AppTaskEsp8266   (void *p_arg);


int  main (void)
{
    OS_ERR  err;
    
    BSP_Init();                                                 /* Initialize BSP functions                             */

    OSInit(&err);                                               /* Init uC/OS-III.                                      */
    
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;

   (void)p_arg;
    
    printf("AppTaskStart\n");
    
    CPU_Init();

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();

#if OS_CFG_STAT_TASK_EN > 0u
    //计算CPU使用率
    OSStatTaskCPUUsageInit(&err);
#endif

    //复位当前最大关中断时间
    CPU_IntDisMeasMaxCurReset();

    //开启时间轮片
    OSSchedRoundRobinCfg ( DEF_ENABLED, 0, &err);

    //创建实时时钟任务
    OSTaskCreate((OS_TCB     *)&AppTaskRtcTCB,				   
                 (CPU_CHAR   *)"App Task RTC",
                 (OS_TASK_PTR ) AppTaskRtc,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_RTC_PRIO,
                 (CPU_STK    *)&AppTaskRtcStk[0],
                 (CPU_STK_SIZE) APP_TASK_RTC_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_RTC_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
    printf("AppTaskRtc_ERR:%d\n",err);
    
    //创建核心处理任务，控制界面等
    OSTaskCreate((OS_TCB     *) &AppTaskCoreTCB,
                 (CPU_CHAR   *) "App Task Core",
                 (OS_TASK_PTR ) AppTaskCore,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_CORE_PRIO,
                 (CPU_STK    *) &AppTaskCoreStk[0],
                 (CPU_STK_SIZE) APP_TASK_CORE_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_CORE_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *) &err);
    printf("AppTaskCore_ERR:%d\n",err);

    //创建主机服务任务
    OSTaskCreate((OS_TCB     *) &AppTaskCanMasterTCB,			
                 (CPU_CHAR   *) "App Task CAN_Master",
                 (OS_TASK_PTR ) AppTaskCanMaster,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_CAN_MASTER_PRIO,
                 (CPU_STK    *) &AppTaskCanMasterStk[0],
                 (CPU_STK_SIZE) APP_TASK_CAN_MASTER_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_CAN_MASTER_STK_SIZE,
                 (OS_MSG_QTY  ) CAN_RX_MEM_MAX_NUMBER,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *) &err);
    printf("AppTaskCanMaster_ERR:%d\n",err); 

    //创建网关通信任务
    OSTaskCreate((OS_TCB     *) &AppTaskEsp8266TCB,			
                 (CPU_CHAR   *) "App Task ESP8266",
                 (OS_TASK_PTR ) AppTaskEsp8266,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_ESP8266_PRIO,
                 (CPU_STK    *) &AppTaskEsp8266Stk[0],
                 (CPU_STK_SIZE) APP_TASK_ESP8266_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_ESP8266_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *) &err);
    printf("AppTaskEsp8266_ERROR:%d\n",err);     
                 
                 
    printf("OSTaskCreate_All\n");
                 
    //蜂鸣器响一声提示
    BSP_Buzzer_Alarm( 1 );

    while (DEF_TRUE) {
        //每秒改变一次LED的工作状态
        //通过与状态栏RTC时钟的变化进行比较，改变的频率一致，说明系统正常
        LED1 = !LED1;     
        OSTimeDlyHMSM ( 0, 0, 1, 0, OS_OPT_TIME_DLY, &err );
    }
}

