#ifndef __APP_ESP8266_H__
#define __APP_ESP8266_H__

#include "stm32f10x.h"
#include "./SYS/bsp_sys.h"

#define     ESP8266_CH_PD_GPIO              RCC_APB2Periph_GPIOA
#define     ESP8266_CH_PD_PIN               GPIO_Pin_8
#define     ESP8266_PORT                    GPIOA

#define		ESP8266_RX_MEM_MAX_NUMBER		20
#define     ESP8266_RX_PAGE_NUM             25

#define     ESP8266_CHECK_FIRST             0xEF
#define     ESP8266_CHECK_LAST              0xFE
#define		ESP8266_CHECK_SIZE_FIRST		0
#define		ESP8266_CHECK_SIZE_MODE			1
#define		ESP8266_CHECK_SIZE_TYPE			2
#define		ESP8266_CHECK_SIZE_ID			5

#define		ESP8266_CHECK_SIZE_LAST			9

#define     ESP8266_USART_RX                (2+8)
#define     ESP8266_USART_TX                (2+8)
#define     ESP8266_TX_LOG                  (1+128)
#define     ESP8266_TX_MAX                  (1+128)

#define     ESP8266_PING_TIMEOUT            10
#define     ESP8266_RESTART_TIMEOUT         5
#define     BOARD_PING_TIME                 5

#define     ESP8266_PIN_CH_PD               PAout(8)

void Esp8266_Config ( void );

#endif

