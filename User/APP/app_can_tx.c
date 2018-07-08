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

//����CAN_RX����
OS_MEM          CanRxMem;                                           //��������
static uint8_t  pRam[CAN_RX_MEM_MAX_NUMBER * sizeof(CanRxMsg)];     //����ռ�
static void*    pRamHeader = pRam;                                  //����ͷָ��

//�豸�б�
tList	TerminalList[8][7];
static uint8_t sListTypeNum[8];//��������Ч�ն���

//��������
static uint8_t	ssystemMode				= NONE;				//������ģʽ
uint32_t	gmasterPriorityID	        = 0;	            //Ŀǰ����ID

//�����豸���ȼ�
static const uint32_t	spriorityID	    = 0x100;

//��������ʱ�����
uint8_t gcanMasterRx = 0;		//��������������ʱ
uint8_t gcanMasterTx;			//����ģʽ�·���������ʱ
extern  OS_TCB   AppTaskCanMasterTCB;
extern  CAN_FilterInitTypeDef CAN_FilterInitStruct[14];

//CAN����ָ��
static CanRxMsg  * sCanRxMessage;                   //����
static tMsg      * sCanRxMessageData;               //ת��-���ݶ�
static CanTxMsg    sCanTxMessage = {                //������������ģ��
    .StdId  = spriorityID,
    .IDE    = CAN_Id_Standard,
    .RTR    = CAN_RTR_Data,
    .DLC    = CAN_MESSAGE_LENGTH,    
};

//��̬����
static void appTaskInit ( void );
static void canMasterDecviceManagement ( void );


/*
*		��������	appTaskInit
*		������	�����ʼ������������
*		���룺
*		�����
*/
static void appTaskInit ( void )
{
    OS_ERR	      err;
    
    gcanMasterRx = 0;
    gcanMasterTx = 0;
    
    ssystemMode = MASTER;
    memset(sListTypeNum, 0 ,sizeof(sListTypeNum));//��������ն���Ч����
    
    while ( 1 )
	{
		//����CAN_RX����
		OSMemCreate (	(OS_MEM		*)	&CanRxMem,
                        (CPU_CHAR 	*)	"Can Receive Message Cache",
                        (void 		*)	pRamHeader,
                        (OS_MEM_QTY	 )	CAN_RX_MEM_MAX_NUMBER,
                        (OS_MEM_SIZE ) 	sizeof( CanRxMsg ),
                        (OS_ERR 	*)	&err);
		
        if ( err == OS_ERR_NONE )
        {
            //�ɹ��������˳�ѭ��
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
    
    //�����ʼ��
	appTaskInit(  );
	
	while ( 1 )
	{
        gmasterPriorityID = spriorityID;
        
		//����������Ϣ����
		sCanRxMessage = OSTaskQPend (	( OS_TICK        ) 0,
                                        ( OS_OPT         ) OS_OPT_PEND_NON_BLOCKING,
                                        ( OS_MSG_SIZE   *) sizeof( CanRxMsg ),
                                        ( CPU_TS        *) NULL,
                                        ( OS_ERR        *) &err	);
        
        //��ȷ�յ���Ϣ
		if ( err == OS_ERR_NONE )
		{
            //��ȡ���ݶ�
            sCanRxMessageData = (tMsg *)&sCanRxMessage->Data;
            
            //��ӡ��������
            printf("sCanRxMessageData->Control:%x\n", sCanRxMessageData->Control);          
            printf("PriorityID ExtId�� %x\n",sCanRxMessage->ExtId);
            printf("Data��");
            for( uint8_t i=0; i<8; i++ )
                printf("%x ",sCanRxMessage->Data[i]);
            printf("\n");
            
            //���ݱ������ͣ����д���
            switch ( sCanRxMessageData->Mode )
            {
                //�����豸�б�
                case MODE_CAN_DEVICE_UPDATE:    canMasterDecviceManagement(  );
                                                //����
                                                if ( ssystemMode == MASTER )
                                                {
                                                    //��������������������˷���ת�����±���
                                                    static uint8_t TxUpdateEsp8266[ESP8266_USART_TX];//����������
                                                    
                                                    //���ڱ���ͷβУ��
                                                    TxUpdateEsp8266[ESP8266_CHECK_SIZE_FIRST]   = ESP8266_CHECK_FIRST;
                                                    TxUpdateEsp8266[ESP8266_CHECK_SIZE_LAST]    = ESP8266_CHECK_LAST;
                                                    
                                                    //��������
                                                    memcpy( &TxUpdateEsp8266[ESP8266_CHECK_SIZE_MODE], sCanRxMessage->Data, CAN_MESSAGE_LENGTH );
                                                    
                                                    Usart_SendArray( ESP8266_USARTx, (uint8_t*)&TxUpdateEsp8266, ESP8266_USART_TX );
                                                }
                                            break;
                
                //ת���ӻ��Ŀ����ͱ���
                case MODE_CAN_CONTROL_SLAVE:    memcpy( &sCanTxMessage.Data[0], sCanRxMessageData, CAN_MESSAGE_LENGTH );
                                                printf("sCanTxMessageData->Control:%x\n", sCanTxMessage.Data[1] );
                                                
                                                //�޸ı�������
                                                sCanTxMessage.Data[0] = MODE_CAN_CONTROL_TERMINAL;
                                                
                                                //��ӡ�޸ĺ�ı�������
                                                printf("Tx Data: ");
                                                for ( uint8_t i=0; i<CAN_MESSAGE_LENGTH ;i++ )
                                                    printf("%x ", sCanTxMessage.Data[i]);
                                                printf("\n");

                                                CAN_Transmit( CAN1, &sCanTxMessage );
                                            break;
            }
		}

		//�黹�ڴ��
		OSMemPut (	(OS_MEM		*)	&CanRxMem,            	//ָ���ڴ�������
                    (void 		*)	sCanRxMessage,        	//�ڴ����׵�ַ
                    (OS_ERR		*)	&err);		         	//���ش�������

		OSTimeDlyHMSM ( 0, 0, 0, 500, OS_OPT_TIME_DLY, &err );
	}
}

/*
*		��������	canMasterDecviceManagement
*		������	�����豸�б�
*		���룺
*		�����
*/
static void canMasterDecviceManagement ( void )
{
    //�ж����ݶ�
    if ( sCanRxMessageData->Mode != MODE_CAN_DEVICE_UPDATE )
        return ;
    
    //��ȡ���͵�ǰ��Ч
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

	//��ȡ�ڴ��
	RxMessage	= OSMemGet (	(OS_MEM		*)	&CanRxMem,   	//ָ���ڴ�������
                                (OS_ERR		*)	&err);       	//���ش�������

	//����CAN����
	CAN_Receive( CAN1, 0, RxMessage );

	//����������Ϣ����
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

	//��ȡ�ڴ��
	RxMessage	= OSMemGet (	(OS_MEM		*)	&CanRxMem,   	//ָ���ڴ�������
                                (OS_ERR		*)	&err);       	//���ش�������

	//����CAN����
	CAN_Receive( CAN1, 1, RxMessage );

	//����������Ϣ����
	OSTaskQPost (   ( OS_TCB        * ) &AppTaskCanMasterTCB,
                    ( void 	 	    * ) RxMessage,
                    ( OS_MSG_SIZE	  ) sizeof( CanRxMsg ),
                    ( OS_OPT 		  ) OS_OPT_POST_FIFO,
                    ( OS_ERR 		* ) &err );
                    
	OSIntExit(  );
}
