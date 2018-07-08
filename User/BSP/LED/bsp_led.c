#include "includes.h"
#include "./LED/bsp_led.h"

void BSP_LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);	 //ʹ��PD�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin     = LED_1_PIN | LED_3_PIN;	 //LED1-->PB.13,LED3-->PB.14  �˿�����
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(LED_PORT, &GPIO_InitStructure);                //�����趨������ʼ��GPIOD.13,GPIO.14
}
  
