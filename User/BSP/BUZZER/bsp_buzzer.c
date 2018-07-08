#include "includes.h"
#include "./BUZZER/bsp_buzzer.h"

void BSP_Buzzer_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(BUZZER_GPIO_CLK, ENABLE);        //使能PE端口时钟

	GPIO_InitStructure.GPIO_Pin     = BUZZER_PIN;           //Buzzer->PE0
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;     //推挽输出
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;     //IO口速度为50MHz
	GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);            //初始化
    
    BUZZER = 1;
}

void BSP_Buzzer_Alarm ( uint8_t time )
{
    OS_ERR      err;
    for ( uint8_t i=0; i<time; i++ )
    {
        BUZZER = !BUZZER;
        OSTimeDlyHMSM ( 0, 0, 0, 200, OS_OPT_TIME_DLY, &err );
    }
    BUZZER = 1;
}
