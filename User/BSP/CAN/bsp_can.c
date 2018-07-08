#include "stm32f10x_can.h"
#include "includes.h"
#include "./CAN/bsp_can.h"
#include "app_can_tx.h"
#include "string.h"

extern CAN_FilterInitTypeDef CAN_FilterInitStruct[14];

/*
 * 函数名：CAN_Mode_Init
 * 描述  ：CAN模式配置
 * 输入  ：CAN模式：
                普通模式：     CAN_Mode_Normal
                回环模式：     CAN_Mode_LoopBack
                静默模式：     CAN_Mode_Silent
                环回静默模式： CAN_Mode_Silent_LoopBack
 * 输出  ：无
 * 调用  ：内部调用
 */
void CAN_Config_Init( u8 mode )
{
    //定义初始化结构体
    GPIO_InitTypeDef        GPIO_InitStructure;
    CAN_InitTypeDef         CAN_InitStructure;
	CAN_FilterInitTypeDef   CAN_FilterInitStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;
    
    //使能GPIO、CAN时钟
    RCC_APB2PeriphClockCmd( CAN_GPIO_CLK, ENABLE );         //使能PORTA时钟	                   											 
    RCC_APB1PeriphClockCmd( CAN_CLK, ENABLE );              //使能CAN1时钟	

    //初始化GPIO
    GPIO_InitStructure.GPIO_Pin         = CAN_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed       = CAN_GPIO_SPEED;
    GPIO_InitStructure.GPIO_Mode        = CAN_TX_GPIO_MODE;
    GPIO_Init( CAN_GPIO_PORT, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin         = CAN_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode        = CAN_RX_GPIO_MODE;
    GPIO_Init( CAN_GPIO_PORT, &GPIO_InitStructure);

    //CAN单元设置
    CAN_InitStructure.CAN_TTCM = DISABLE;       //非时间触发通信模式
    CAN_InitStructure.CAN_ABOM = DISABLE;       //软件自动离线管理
    CAN_InitStructure.CAN_AWUM = DISABLE;       //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN_InitStructure.CAN_NART = ENABLE;        //禁止报文自动传送
    CAN_InitStructure.CAN_RFLM = DISABLE;       //报文不锁定,新的覆盖旧的
    CAN_InitStructure.CAN_TXFP = ENABLE;       //优先级由报文标识符决定
    CAN_InitStructure.CAN_Mode = mode;          //模式设置： mode:0,普通模式;1,回环模式; //

    //设置波特率
    CAN_InitStructure.CAN_SJW = CAN_TIME_SJW;
    CAN_InitStructure.CAN_BS1 = CAN_TIME_BS1;
    CAN_InitStructure.CAN_BS2 = CAN_TIME_BS2;
    CAN_InitStructure.CAN_Prescaler = CAN_PRESCALER; 
    CAN_Init( CAN1, &CAN_InitStructure );


	CAN_FilterInitStructure.CAN_FilterNumber      =   FMI_RECEIVE_ALL;       //过滤器0
	CAN_FilterInitStructure.CAN_FilterMode        =   CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale       =   CAN_FilterScale_32bit; //32位 
	CAN_FilterInitStructure.CAN_FilterIdHigh      =   0x1234;   //32位ID
	CAN_FilterInitStructure.CAN_FilterIdLow       =   0x1234;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh  =   0x0000;   //32位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow   =   0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation  =	  ENABLE; //激活过滤器0

    CAN_FilterInit( &CAN_FilterInitStructure );//滤波器初始化
    
    for ( uint8_t i=0; i<13; i++ )
    {
        CAN_FilterInitStructure.CAN_FilterNumber = i;
        CAN_FilterInitStructure.CAN_FilterActivation = DISABLE;
        CAN_FilterInit( &CAN_FilterInitStructure );//滤波器初始化
    }

    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//允许FIFO_0消息挂号中断.	
    CAN_ITConfig(CAN1,CAN_IT_FMP1,ENABLE);//允许FIFO_0消息挂号中断.

    NVIC_InitStructure.NVIC_IRQChannel                      = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 2;    // 主优先级为2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 1;    // 次优先级为1
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //配置FIFO1中断优先级
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
	
	TxMessage.ExtId = CAN_PRIORITY_ID_MASTER_TX_TERMINAL;   // 控制型报文->控制设备
    TxMessage.IDE   = CAN_ID_STD;                           // 标准帧
	TxMessage.RTR   = CAN_RTR_DATA;                         // 数据帧
	TxMessage.DLC   = CAN_MESSAGE_LENGTH;                   // 长度
    
    memcpy ( &TxMessage.Data, msg, CAN_MESSAGE_LENGTH );
    
    printf("Tx Data: ");
    for ( uint8_t i=0; i<CAN_MESSAGE_LENGTH ;i++ )
        printf("%x ",TxMessage.Data[i]);
    printf("\n");
    
	ret = CAN_Transmit( CAN1, &TxMessage );

	while( ( CAN_TransmitStatus(CAN1, ret) == CAN_TxStatus_Failed ) && (i < 0xFFF) )
		i++;	//等待发送结束
	
	if( i >= 0xFFF )
		return 1;
	
	return 0;
}
