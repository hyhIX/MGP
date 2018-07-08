#include "includes.h"
#include "./CAN/bsp_can.h"
#include "./RTC/bsp_rtc.h"
#include "./USART/bsp_usart.h"
#include "string.h"
#include "app_can_tx.h"
#include "app_core.h"
#include "app_esp8266.h"

//CAN_Master
OS_TCB   AppTaskCanMasterTCB;
CPU_STK  AppTaskCanMasterStk[APP_TASK_CAN_MASTER_STK_SIZE];
void  AppTaskCanMaster   (void *p_arg);

//创建CAN_RX缓存
OS_MEM          CanRxMem;                                           //创建对象
static uint8_t  pRam[CAN_RX_MEM_MAX_NUMBER * sizeof(CanRxMsg)];     //缓存空间
static void*    pRamHeader = pRam;                                  //缓存头指针

//设备列表
tList	TerminalList[8][7];
static uint8_t sListTypeNum[8];//各类型有效终端数

//主机管理
static uint8_t	ssystemMode				= NONE;				//面板控制模式
uint32_t	gmasterPriorityID	        = 0;	            //目前主机ID

//自身设备优先级
static const uint32_t	spriorityID	    = 0x100;

//主机管理时间次数
uint8_t gcanMasterRx = 0;		//接收主机心跳计时
uint8_t gcanMasterTx;			//主机模式下发送心跳计时
extern  OS_TCB   AppTaskCanMasterTCB;
extern  CAN_FilterInitTypeDef CAN_FilterInitStruct[14];

//CAN报文指针
static CanRxMsg  * sCanRxMessage;                   //接收
static tMsg      * sCanRxMessageData;               //转换-数据段
static CanTxMsg    sCanTxMessage = {                //设置主机报文模版
    .StdId  = spriorityID,
    .IDE    = CAN_Id_Standard,
    .RTR    = CAN_RTR_Data,
    .DLC    = CAN_MESSAGE_LENGTH,    
};

//静态函数
static void appTaskInit ( void );
static void canMasterDecviceManagement ( void );


/*
*		函数名：	appTaskInit
*		描述：	任务初始化，创建缓存
*		输入：
*		输出：
*/
static void appTaskInit ( void )
{
    OS_ERR	      err;
    
    gcanMasterRx = 0;
    gcanMasterTx = 0;
    
    ssystemMode = MASTER;
    memset(sListTypeNum, 0 ,sizeof(sListTypeNum));//清空类型终端有效数量
    
    while ( 1 )
	{
		//创建CAN_RX缓存
		OSMemCreate (	(OS_MEM		*)	&CanRxMem,
                        (CPU_CHAR 	*)	"Can Receive Message Cache",
                        (void 		*)	pRamHeader,
                        (OS_MEM_QTY	 )	CAN_RX_MEM_MAX_NUMBER,
                        (OS_MEM_SIZE ) 	sizeof( CanRxMsg ),
                        (OS_ERR 	*)	&err);
		
        if ( err == OS_ERR_NONE )
        {
            //成功创建，退出循环
            printf("OSMemCreate Successful\n");
            break;
        }

        printf("OSMemCreate Failed Delay 5s\n");
        OSTimeDlyHMSM ( 0, 0, 5, 0, OS_OPT_TIME_DLY, &err );
	}
}

void  AppTaskCanMaster ( void *p_arg )
{
	OS_ERR	      err;
    
    //任务初始化
	appTaskInit(  );
	
	while ( 1 )
	{
        gmasterPriorityID = spriorityID;
        
		//接收任务消息队列
		sCanRxMessage = OSTaskQPend (	( OS_TICK        ) 0,
                                        ( OS_OPT         ) OS_OPT_PEND_NON_BLOCKING,
                                        ( OS_MSG_SIZE   *) sizeof( CanRxMsg ),
                                        ( CPU_TS        *) NULL,
                                        ( OS_ERR        *) &err	);
        
        //正确收到消息
		if ( err == OS_ERR_NONE )
		{
            //截取数据段
            sCanRxMessageData = (tMsg *)&sCanRxMessage->Data;
            
            //打印报文内容
            printf("sCanRxMessageData->Control:%x\n", sCanRxMessageData->Control);          
            printf("PriorityID ExtId： %x\n",sCanRxMessage->ExtId);
            printf("Data：");
            for( uint8_t i=0; i<8; i++ )
                printf("%x ",sCanRxMessage->Data[i]);
            printf("\n");
            
            //根据报文类型，进行处理
            switch ( sCanRxMessageData->Mode )
            {
                //更新设备列表
                case MODE_CAN_DEVICE_UPDATE:    canMasterDecviceManagement(  );
                                                //主机
                                                if ( ssystemMode == MASTER )
                                                {
                                                    //本机是主机，向服务器端发送转发更新报文
                                                    static uint8_t TxUpdateEsp8266[ESP8266_USART_TX];//创建缓冲区
                                                    
                                                    //串口报文头尾校验
                                                    TxUpdateEsp8266[ESP8266_CHECK_SIZE_FIRST]   = ESP8266_CHECK_FIRST;
                                                    TxUpdateEsp8266[ESP8266_CHECK_SIZE_LAST]    = ESP8266_CHECK_LAST;
                                                    
                                                    //复制内容
                                                    memcpy( &TxUpdateEsp8266[ESP8266_CHECK_SIZE_MODE], sCanRxMessage->Data, CAN_MESSAGE_LENGTH );
                                                    
                                                    Usart_SendArray( ESP8266_USARTx, (uint8_t*)&TxUpdateEsp8266, ESP8266_USART_TX );
                                                }
                                            break;
                
                //转发从机的控制型报文
                case MODE_CAN_CONTROL_SLAVE:    memcpy( &sCanTxMessage.Data[0], sCanRxMessageData, CAN_MESSAGE_LENGTH );
                                                printf("sCanTxMessageData->Control:%x\n", sCanTxMessage.Data[1] );
                                                
                                                //修改报文类型
                                                sCanTxMessage.Data[0] = MODE_CAN_CONTROL_TERMINAL;
                                                
                                                //打印修改后的报文内容
                                                printf("Tx Data: ");
                                                for ( uint8_t i=0; i<CAN_MESSAGE_LENGTH ;i++ )
                                                    printf("%x ", sCanTxMessage.Data[i]);
                                                printf("\n");

                                                CAN_Transmit( CAN1, &sCanTxMessage );
                                            break;
            }
		}

		//归还内存块
		OSMemPut (	(OS_MEM		*)	&CanRxMem,            	//指向内存管理对象
                    (void 		*)	sCanRxMessage,        	//内存块的首地址
                    (OS_ERR		*)	&err);		         	//返回错误类型

		OSTimeDlyHMSM ( 0, 0, 0, 500, OS_OPT_TIME_DLY, &err );
	}
}

/*
*		函数名：	canMasterDecviceManagement
*		描述：	更新设备列表
*		输入：
*		输出：
*/
static void canMasterDecviceManagement ( void )
{
    //判断数据段
    if ( sCanRxMessageData->Mode != MODE_CAN_DEVICE_UPDATE )
        return ;
    
    //提取类型当前有效
    uint8_t type = sCanRxMessageData->Control - CONTROL_TYPE_MIN - 1;
    
    uint8_t num;
    for ( num=0; num<sListTypeNum[type]; num++ )
    {
        if ( TerminalList[type][num].FactoryID == sCanRxMessageData->PriorityID )
        {
            TerminalList[type][num].Status          = DECVICE_NORMAL;
            TerminalList[type][num].UpdateTime      = BSP_GetUnixTime(  );
            
            TerminalList[type][num].Update_Byte[0]  = sCanRxMessageData->Control_Byte[0];
            TerminalList[type][num].Update_Byte[1]  = sCanRxMessageData->Control_Byte[1];
        }
    }
    if ( num == sListTypeNum[type] && num < CAN_LIST_TYPE_MAX )
    {
        TerminalList[type][num].FactoryID   = LIST_DECVICE_FACTORY_ID;
        TerminalList[type][num].ModelID     = sCanRxMessageData->Control;
        TerminalList[type][num].Status      = DECVICE_NORMAL;
        TerminalList[type][num].UpdateTime  = BSP_GetUnixTime(  );
        TerminalList[type][num].PriorityID  = sCanRxMessageData->PriorityID;
        
        TerminalList[type][num].Update_Byte[0] = sCanRxMessageData->Control_Byte[0];
        TerminalList[type][num].Update_Byte[1] = sCanRxMessageData->Control_Byte[1];
    }
}

uint8_t getSystemMode ( void )
{
    return ssystemMode;
}


void USB_LP_CAN1_RX0_IRQHandler(void)
{
	OSIntEnter(  );

	OS_ERR		err;
	CanRxMsg	* RxMessage;

	//获取内存块
	RxMessage	= OSMemGet (	(OS_MEM		*)	&CanRxMem,   	//指向内存管理对象
                                (OS_ERR		*)	&err);       	//返回错误类型

	//接收CAN报文
	CAN_Receive( CAN1, 0, RxMessage );

	//发送任务消息队列
	OSTaskQPost (   ( OS_TCB        * ) &AppTaskCanMasterTCB,
                    ( void 	 	    * ) RxMessage,
                    ( OS_MSG_SIZE	  ) sizeof( CanRxMsg ),
                    ( OS_OPT 		  ) OS_OPT_POST_FIFO,
                    ( OS_ERR 		* ) &err );

	OSIntExit(  );
}

void CAN1_RX1_IRQHandler ( void )
{
    OSIntEnter(  );

	OS_ERR		err;
	CanRxMsg	* RxMessage;

	//获取内存块
	RxMessage	= OSMemGet (	(OS_MEM		*)	&CanRxMem,   	//指向内存管理对象
                                (OS_ERR		*)	&err);       	//返回错误类型

	//接收CAN报文
	CAN_Receive( CAN1, 1, RxMessage );

	//发送任务消息队列
	OSTaskQPost (   ( OS_TCB        * ) &AppTaskCanMasterTCB,
                    ( void 	 	    * ) RxMessage,
                    ( OS_MSG_SIZE	  ) sizeof( CanRxMsg ),
                    ( OS_OPT 		  ) OS_OPT_POST_FIFO,
                    ( OS_ERR 		* ) &err );
                    
	OSIntExit(  );
}
