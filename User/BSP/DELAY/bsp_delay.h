#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f10x.h"

#define     DELAY_TIMx          TIM6
#define     DELAY_TIMx_CLK      RCC_APB1Periph_TIM6
#define     DELAY_TIMx_IRQ      TIM6_IRQn

void BSP_Delay_Init(uint16_t arr, uint16_t psc);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);

#endif





























