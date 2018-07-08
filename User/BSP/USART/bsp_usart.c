#include "includes.h"
#include "./USART/bsp_usart.h"

///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
    /* ����һ���ֽ����ݵ����� */
    USART_SendData(DEBUG_USARTx, (uint8_t) ch);

    /* �ȴ�������� */
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		

    return (ch);
}

///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
    /* �ȴ������������� */
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(DEBUG_USARTx);
}

/*
*	������:	BSP_USART_DEBUG_Init
*	����:	���Դ��ڳ�ʼ��
*	����:	bound������
*	���:
*/
void BSP_USART_DEBUG_Init(uint32_t bound)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
	USART_InitTypeDef   USART_InitStructure;
    
    //ʹ��USART1��GPIOAʱ��
	RCC_APB2PeriphClockCmd( DEBUG_USART_CLK | DEBUG_USART_GPIO_CLK, ENABLE);
 	
    //��λ����1
    USART_DeInit(DEBUG_USARTx);
    
    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin     = DEBUG_USART_TX_GPIO_PIN; //PA.9
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure); //��ʼ��PA9

    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin     = DEBUG_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);  //��ʼ��PA10

    //USART ��ʼ������
    USART_InitStructure.USART_BaudRate              = bound;
    USART_InitStructure.USART_WordLength            = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits              = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity                = USART_Parity_No;//żУ��λ
    USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode                  = USART_Mode_Tx;
    USART_Init(DEBUG_USARTx, &USART_InitStructure);     //��ʼ������

    USART_Cmd(DEBUG_USARTx, ENABLE);                    //ʹ�ܴ���
}

void BSP_USART_ESP8266_Init(uint32_t bound)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
	USART_InitTypeDef   USART_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;
    
    //ʹ��USART3��GPIOAʱ��
    RCC_APB1PeriphClockCmd ( ESP8266_USART_CLK, ENABLE );
	RCC_APB2PeriphClockCmd(  ESP8266_USART_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE );
    GPIO_PinRemapConfig( GPIO_PartialRemap_USART3, ENABLE );
    
    //��λ����2
    USART_DeInit(ESP8266_USARTx);
    
    //USART3_TX   PC11
    GPIO_InitStructure.GPIO_Pin     = ESP8266_USART_TX_GPIO_PIN; 
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(ESP8266_USART_TX_GPIO_PORT, &GPIO_InitStructure); //��ʼ��

    //USART3_RX	  PC12
    GPIO_InitStructure.GPIO_Pin     = ESP8266_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(ESP8266_USART_RX_GPIO_PORT, &GPIO_InitStructure);  //��ʼ��PA

    NVIC_InitStructure.NVIC_IRQChannel                      = ESP8266_USART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 1;        //��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 1;        //�����ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;   //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ���

    //USART ��ʼ������
    USART_InitStructure.USART_BaudRate              = bound;
    USART_InitStructure.USART_WordLength            = USART_WordLength_9b;//�ֳ�Ϊ9λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits              = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity                = USART_Parity_Even;//żУ��λ
    USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode                  = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(ESP8266_USARTx, &USART_InitStructure);     //��ʼ������

    USART_ITConfig(ESP8266_USARTx, USART_IT_RXNE, ENABLE);      //���������ж�

    USART_Cmd(ESP8266_USARTx, ENABLE);                    //ʹ�ܴ���
}


/*****************  ����һ���ֽ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** ����8λ������ ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
    uint8_t i;
	
    for(i=0; i<num; i++)
    {
	    /* ����һ���ֽ����ݵ�USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
    }
	/* �ȴ�������� */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}



