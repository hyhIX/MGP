#ifndef __USART_H__
#define __USART_H__

#include "stm32f10x.h"

// 串口1-USART1
#define  DEBUG_USARTx                   USART1
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1
#define  DEBUG_USART_BAUDRATE           115200

// USART GPIO 引脚宏定义
#define  DEBUG_USART_GPIO_CLK           RCC_APB2Periph_GPIOA

#define  DEBUG_USART_TX_GPIO_PORT       GPIOA   
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10

#define  DEBUG_USART_IRQ                USART1_IRQn
#define  DEBUG_USART_IRQHandler         USART1_IRQHandler

// 串口2-USART2
#define  ESP8266_USARTx                   USART3
#define  ESP8266_USART_CLK                RCC_APB1Periph_USART3
#define  ESP8266_USART_BAUDRATE           115200

// USART GPIO 引脚宏定义
#define  ESP8266_USART_GPIO_CLK           RCC_APB2Periph_GPIOC

#define  ESP8266_USART_TX_GPIO_PORT       GPIOC
#define  ESP8266_USART_TX_GPIO_PIN        GPIO_Pin_10
#define  ESP8266_USART_RX_GPIO_PORT       GPIOC
#define  ESP8266_USART_RX_GPIO_PIN        GPIO_Pin_11

#define  ESP8266_USART_IRQ                USART3_IRQn
#define  ESP8266_USART_IRQHandler         USART3_IRQHandler




/* 调试串口初始化 */
void BSP_USART_DEBUG_Init( uint32_t bound );
void BSP_USART_ESP8266_Init(uint32_t bound);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);

#endif


