#include "stm32f10x_can.h"
#include "includes.h"
#include "./CAN/bsp_can.h"
#include "app_can_tx.h"
#include "string.h"

extern CAN_FilterInitTypeDef CAN_FilterInitStruct[14];

/*
 * ��������CAN_Mode_Init
 * ����  ��CANģʽ����
 * ����  ��CANģʽ��
                ��ͨģʽ��     CAN_Mode_Normal
                �ػ�ģʽ��     CAN_Mode_LoopBack
                ��Ĭģʽ��     CAN_Mode_Silent
                ���ؾ�Ĭģʽ�� CAN_Mode_Silent_LoopBack
 * ���  ����
 * ����  ���ڲ�����
 */
void CAN_Config_Init( u8 mode )
{
    //�����ʼ���ṹ��
    GPIO_InitTypeDef        GPIO_InitStructure;
    CAN_InitTypeDef         CAN_InitStructure;
	CAN_FilterInitTypeDef   CAN_FilterInitStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;
    
    //ʹ��GPIO��CANʱ��
    RCC_APB2PeriphClockCmd( CAN_GPIO_CLK, ENABLE );         //ʹ��PORTAʱ��	                   											 
    RCC_APB1PeriphClockCmd( CAN_CLK, ENABLE );              //ʹ��CAN1ʱ��	

    //��ʼ��GPIO
    GPIO_InitStructure.GPIO_Pin         = CAN_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed       = CAN_GPIO_SPEED;
    GPIO_InitStructure.GPIO_Mode        = CAN_TX_GPIO_MODE;
    GPIO_Init( CAN_GPIO_PORT, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin         = CAN_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode        = CAN_RX_GPIO_MODE;
    GPIO_Init( CAN_GPIO_PORT, &GPIO_InitStructure);

    //CAN��Ԫ����
    CAN_InitStructure.CAN_TTCM = DISABLE;       //��ʱ�䴥��ͨ��ģʽ
    CAN_InitStructure.CAN_ABOM = DISABLE;       //����Զ����߹���
    CAN_InitStructure.CAN_AWUM = DISABLE;       //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
    CAN_InitStructure.CAN_NART = ENABLE;        //��ֹ�����Զ�����
    CAN_InitStructure.CAN_RFLM = DISABLE;       //���Ĳ�����,�µĸ��Ǿɵ�
    CAN_InitStructure.CAN_TXFP = ENABLE;       //���ȼ��ɱ��ı�ʶ������
    CAN_InitStructure.CAN_Mode = mode;          //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; //

    //���ò�����
    CAN_InitStructure.CAN_SJW = CAN_TIME_SJW;
    CAN_InitStructure.CAN_BS1 = CAN_TIME_BS1;
    CAN_InitStructure.CAN_BS2 = CAN_TIME_BS2;
    CAN_InitStructure.CAN_Prescaler = CAN_PRESCALER; 
    CAN_Init( CAN1, &CAN_InitStructure );


	CAN_FilterInitStructure.CAN_FilterNumber      =   FMI_RECEIVE_ALL;       //������0
	CAN_FilterInitStructure.CAN_FilterMode        =   CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale       =   CAN_FilterScale_32bit; //32λ 
	CAN_FilterInitStructure.CAN_FilterIdHigh      =   0x1234;   //32λID
	CAN_FilterInitStructure.CAN_FilterIdLow       =   0x1234;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh  =   0x0000;   //32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow   =   0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation  =	  ENABLE; //���������0

    CAN_FilterInit( &CAN_FilterInitStructure );//�˲�����ʼ��
    
    for ( uint8_t i=0; i<13; i++ )
    {
        CAN_FilterInitStructure.CAN_FilterNumber = i;
        CAN_FilterInitStructure.CAN_FilterActivation = DISABLE;
        CAN_FilterInit( &CAN_FilterInitStructure );//�˲�����ʼ��
    }

    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//����FIFO_0��Ϣ�Һ��ж�.	
    CAN_ITConfig(CAN1,CAN_IT_FMP1,ENABLE);//����FIFO_0��Ϣ�Һ��ж�.

    NVIC_InitStructure.NVIC_IRQChannel                      = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 2;    // �����ȼ�Ϊ2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 1;    // �����ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //����FIFO1�ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel                      = CAN1_RX1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

}


uint8_t Can_Send_Msg( uint8_t* msg )
{
	uint8_t ret;
    uint16_t i = 0;    
	CanTxMsg TxMessage;
	
	TxMessage.ExtId = CAN_PRIORITY_ID_MASTER_TX_TERMINAL;   // �����ͱ���->�����豸
    TxMessage.IDE   = CAN_ID_STD;                           // ��׼֡
	TxMessage.RTR   = CAN_RTR_DATA;                         // ����֡
	TxMessage.DLC   = CAN_MESSAGE_LENGTH;                   // ����
    
    memcpy ( &TxMessage.Data, msg, CAN_MESSAGE_LENGTH );
    
    printf("Tx Data: ");
    for ( uint8_t i=0; i<CAN_MESSAGE_LENGTH ;i++ )
        printf("%x ",TxMessage.Data[i]);
    printf("\n");
    
	ret = CAN_Transmit( CAN1, &TxMessage );

	while( ( CAN_TransmitStatus(CAN1, ret) == CAN_TxStatus_Failed ) && (i < 0xFFF) )
		i++;	//�ȴ����ͽ���
	
	if( i >= 0xFFF )
		return 1;
	
	return 0;
}
