#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "stm32f10x.h"
#include "./SYS/bsp_sys.h"

#define     LED_GPIO_CLK    RCC_APB2Periph_GPIOD
#define     LED_1_PIN       GPIO_Pin_13
#define     LED_3_PIN       GPIO_Pin_14
#define     LED_PORT        GPIOD

#define LED1 PDout(13)// PD13
#define LED3 PDout(14)// PD14	

void BSP_LED_Init(void);//≥ı ºªØ	
	 				    
#endif
