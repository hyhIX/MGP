#ifndef __BSP_BUZZER_H__
#define __BSP_BUZZER_H__

#include "stm32f10x.h"
#include "./SYS/bsp_sys.h"

#define     BUZZER_GPIO_CLK     RCC_APB2Periph_GPIOD
#define     BUZZER_PIN          GPIO_Pin_12
#define     BUZZER_PORT         GPIOD

#define     BUZZER              PDout(12)


void BSP_Buzzer_Init(void);
void BSP_Buzzer_Alarm ( uint8_t time );

#endif


