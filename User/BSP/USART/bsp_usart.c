#include "includes.h"
#include "./USART/bsp_usart.h"

///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到串口 */
    USART_SendData(DEBUG_USARTx, (uint8_t) ch);

    /* 等待发送完毕 */
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		

    return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
    /* 等待串口输入数据 */
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(DEBUG_USARTx);
}

/*
*	函数名:	BSP_USART_DEBUG_Init
*	描述:	调试串口初始化
*	输入:	bound波特率
*	输出:
*/
void BSP_USART_DEBUG_Init(uint32_t bound)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
	USART_InitTypeDef   USART_InitStructure;
    
    //使能USART1，GPIOA时钟
	RCC_APB2PeriphClockCmd( DEBUG_USART_CLK | DEBUG_USART_GPIO_CLK, ENABLE);
 	
    //复位串口1
    USART_DeInit(DEBUG_USARTx);
    
    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin     = DEBUG_USART_TX_GPIO_PIN; //PA.9
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure); //初始化PA9

    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin     = DEBUG_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);  //初始化PA10

    //USART 初始化设置
    USART_InitStructure.USART_BaudRate              = bound;
    USART_InitStructure.USART_WordLength            = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits              = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity                = USART_Parity_No;//偶校验位
    USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode                  = USART_Mode_Tx;
    USART_Init(DEBUG_USARTx, &USART_InitStructure);     //初始化串口

    USART_Cmd(DEBUG_USARTx, ENABLE);                    //使能串口
}

void BSP_USART_ESP8266_Init(uint32_t bound)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
	USART_InitTypeDef   USART_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;
    
    //使能USART3，GPIOA时钟
    RCC_APB1PeriphClockCmd ( ESP8266_USART_CLK, ENABLE );
	RCC_APB2PeriphClockCmd(  ESP8266_USART_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE );
    GPIO_PinRemapConfig( GPIO_PartialRemap_USART3, ENABLE );
    
    //复位串口2
    USART_DeInit(ESP8266_USARTx);
    
    //USART3_TX   PC11
    GPIO_InitStructure.GPIO_Pin     = ESP8266_USART_TX_GPIO_PIN; 
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(ESP8266_USART_TX_GPIO_PORT, &GPIO_InitStructure); //初始化

    //USART3_RX	  PC12
    GPIO_InitStructure.GPIO_Pin     = ESP8266_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(ESP8266_USART_RX_GPIO_PORT, &GPIO_InitStructure);  //初始化PA

    NVIC_InitStructure.NVIC_IRQChannel                      = ESP8266_USART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 1;        //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 1;        //子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;   //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

    //USART 初始化设置
    USART_InitStructure.USART_BaudRate              = bound;
    USART_InitStructure.USART_WordLength            = USART_WordLength_9b;//字长为9位数据格式
    USART_InitStructure.USART_StopBits              = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity                = USART_Parity_Even;//偶校验位
    USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode                  = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(ESP8266_USARTx, &USART_InitStructure);     //初始化串口

    USART_ITConfig(ESP8266_USARTx, USART_IT_RXNE, ENABLE);      //开启空闲中断

    USART_Cmd(ESP8266_USARTx, ENABLE);                    //使能串口
}


/*****************  发送一个字节 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** 发送8位的数组 ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
    uint8_t i;
	
    for(i=0; i<num; i++)
    {
	    /* 发送一个字节数据到USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
    }
	/* 等待发送完成 */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}



