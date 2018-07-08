#include "includes.h"
#include "./LED/bsp_led.h"

void BSP_LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);	 //使能PD端口时钟

	GPIO_InitStructure.GPIO_Pin     = LED_1_PIN | LED_3_PIN;	 //LED1-->PB.13,LED3-->PB.14  端口配置
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(LED_PORT, &GPIO_InitStructure);                //根据设定参数初始化GPIOD.13,GPIO.14
}
  
