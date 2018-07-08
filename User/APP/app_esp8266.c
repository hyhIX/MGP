#include <includes.h>
#include <string.h>
#include "./USART/bsp_usart.h"
#include "app_esp8266.h"
#include "app_can_tx.h"
#include "app_core.h"

//ESP8266
OS_TCB   AppTaskEsp8266TCB;
CPU_STK  AppTaskEsp8266Stk[APP_TASK_ESP8266_STK_SIZE];
void  AppTaskEsp8266   (void *p_arg);

//函数
static void ESP8266_Restart ( void );
static void ESP8266_Forwarding ( void );

//全局变量
uint8_t gBoardPing;
uint8_t gEsp8266Restart;
extern tAlarm gAlarm;     //警告
extern tError gError;     //错误
extern uint32_t	gmasterPriorityID;

//静态变量
static uint8_t  sLength;
static uint8_t  sPage;
static uint8_t  sUsartReceive[ESP8266_RX_PAGE_NUM][ESP8266_USART_RX];

static const tMsg sTxBoardPing = {
    .Mode       = MODE_BOARD_MASTER_PING,
    .Control    = CONTROL_BOARD_MASTER_PING,
};

static CanTxMsg sCanTxControlMsg = {
    .StdId              = CAN_PRIORITY_ID_MASTER_TX_TERMINAL,   //优先级
    .IDE                = CAN_ID_STD,                           //标准帧
    .RTR                = CAN_RTR_DATA,                         //数据帧
    .DLC                = CAN_MESSAGE_LENGTH,  
};
static tMsg*    sRxMsgData;                                                     //串口接收数据段
static tMsg*    sCanTxControlMsgData = (tMsg*)&sCanTxControlMsg.Data;            //CAN发送数据段

/*
*		函数名：	Esp8266_Config
*		描述：	CH_PD引脚控制
*		输入：
*		输出：
*/
void Esp8266_Config ( void )
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(ESP8266_CH_PD_GPIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin     = ESP8266_CH_PD_PIN;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(ESP8266_PORT, &GPIO_InitStructure);
    
    ESP8266_PIN_CH_PD = 1;
}


void  AppTaskEsp8266   (void *p_arg)
{
    OS_ERR	      err;
    
    sPage = 0;
    sLength = 0;
    
    ESP8266_Restart(  );
    OSTimeDlyHMSM ( 0, 0, 3, 0, OS_OPT_TIME_DLY, &err );
    
    while ( 1 )
    {
        if ( sPage > 0 )
        {
            printf("sPage=%d\n",sPage);
            sRxMsgData = (tMsg*)&sUsartReceive[sPage-1][ESP8266_CHECK_SIZE_MODE];

            //再次校验数据包
            if ( sUsartReceive[sPage-1][ESP8266_CHECK_SIZE_FIRST] == ESP8266_CHECK_FIRST
              && sUsartReceive[sPage-1][ESP8266_CHECK_SIZE_LAST]  == ESP8266_CHECK_LAST )
            {
                printf("sRxMsgData->Mode:%x\n",sRxMsgData->Mode);
                //校验正确
                switch ( sRxMsgData->Mode )
                {
                    //服务器控制型报文——转发
                    case MODE_CAN_CONTROL_SERVER:   ESP8266_Forwarding(  );
                                                    break;
                    //收到其他类型报文
                    default:                        printf( "Esp8266 Rx: " );
                                                    for ( uint8_t i=0; i<CAN_MESSAGE_LENGTH; i++ )
                                                        printf( "%02x ", sUsartReceive[sPage-1][ESP8266_CHECK_SIZE_MODE+i] );
                                                    printf( "\n" );
                        break;
                }
                sPage--;
            }
        }
        //面板心跳
        if ( gBoardPing > BOARD_PING_TIME )
        {
            gBoardPing = 0;
            
            printf("gBoardPing\n");
            static uint8_t TxBoardPing[ESP8266_USART_TX];
            TxBoardPing[ESP8266_CHECK_SIZE_FIRST]   = ESP8266_CHECK_FIRST;
            TxBoardPing[ESP8266_CHECK_SIZE_LAST]    = ESP8266_CHECK_LAST;
            memcpy( &TxBoardPing[ESP8266_CHECK_SIZE_MODE], (uint8_t*)&sTxBoardPing, CAN_MESSAGE_LENGTH );
            Usart_SendArray( ESP8266_USARTx, (uint8_t*)&TxBoardPing, ESP8266_USART_TX );
        }   
        
        OSTimeDlyHMSM ( 0, 0, 0, 100, OS_OPT_TIME_DLY, &err ); 
    }
}

/*
*		函数名：	ESP8266_Restart
*		描述：	esp8266重启
*		输入：
*		输出：
*/
static void ESP8266_Restart ( void )
{
    OS_ERR	      err;

    ESP8266_PIN_CH_PD = 0;
    OSTimeDlyHMSM ( 0, 0, 1, 0, OS_OPT_TIME_DLY, &err );
    ESP8266_PIN_CH_PD = 1;
}

/*
*		函数名：	ESP8266_Forwarding
*		描述：	服务器端控制转发
*		输入：
*		输出：
*/
static void ESP8266_Forwarding ( void )
{
    printf( "Server Control: " );
    for ( uint8_t i=0; i<CAN_MESSAGE_LENGTH; i++ )
        printf( "%02x ", sUsartReceive[sPage-1][ESP8266_CHECK_SIZE_MODE+i] );
    printf( "\n" );
    
    sCanTxControlMsgData->Mode              = MODE_CAN_CONTROL_TERMINAL;
    sCanTxControlMsgData->Control           = sUsartReceive[sPage-1][ESP8266_CHECK_SIZE_TYPE];
    sCanTxControlMsgData->Control_Byte[0]   = sUsartReceive[sPage-1][ESP8266_CHECK_SIZE_TYPE+1];
    sCanTxControlMsgData->Control_Byte[1]   = sUsartReceive[sPage-1][ESP8266_CHECK_SIZE_TYPE+2];
    
    sCanTxControlMsgData->Update_Byte[0]    = sUsartReceive[sPage-1][ESP8266_CHECK_SIZE_ID+3];
    sCanTxControlMsgData->Update_Byte[1]    = sUsartReceive[sPage-1][ESP8266_CHECK_SIZE_ID+2];
    sCanTxControlMsgData->Update_Byte[2]    = sUsartReceive[sPage-1][ESP8266_CHECK_SIZE_ID+1];
    sCanTxControlMsgData->Update_Byte[3]    = sUsartReceive[sPage-1][ESP8266_CHECK_SIZE_ID];
       
    CAN_Transmit( CAN1, &sCanTxControlMsg ); 
}

void ESP8266_USART_IRQHandler ( void )
{
    OSIntEnter(  );
    if( USART_GetITStatus(ESP8266_USARTx, USART_IT_RXNE) != RESET )
    {
        sUsartReceive[sPage][sLength++] = USART_ReceiveData(ESP8266_USARTx);
        
        //满一个数据包
        if ( sLength >= ESP8266_USART_RX )
        {
            //校验数据包的正确性
            if ( sUsartReceive[sPage][ESP8266_CHECK_SIZE_FIRST] == ESP8266_CHECK_FIRST
              && sUsartReceive[sPage][ESP8266_CHECK_SIZE_LAST]  == ESP8266_CHECK_LAST )
                sPage++;
            
            sLength = 0;
        }
        //缓冲区满
        if ( sPage >= ESP8266_RX_PAGE_NUM )
        {
            sPage = 0;
            sLength = 0;
        }
    }
    OSIntExit(  );
}


