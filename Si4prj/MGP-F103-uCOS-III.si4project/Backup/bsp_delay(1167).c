#include "includes.h"
#include "./DELAY/bsp_delay.h"


static uint8_t _delayTimeOut;

/*
*   使用硬件定时器TIM6，最低最低0.1ms
*
*
*/
void BSP_Delay_Init(uint16_t arr,uint16_t psc)	 
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(DELAY_TIMx_CLK, ENABLE); //时钟使能

    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(DELAY_TIMx, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig( TIM6,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = DELAY_TIMx_IRQ;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

    TIM_Cmd(DELAY_TIMx, ENABLE);  //使能TIMx
}


void delay_us(uint32_t nus)
{
    while(nus--);
}

void delay_ms(uint16_t nms)
{	
    _delayTimeOut = 0;

    TIM2->ARR = nms;

    TIM_Cmd(TIM6, ENABLE);

    while(!_delayTimeOut);

    TIM_Cmd(TIM6, DISABLE);
}

void TIM6_IRQHandler(void)
{
    OSIntEnter();
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) == SET)
    {
         _delayTimeOut = 1;
         TIM_ClearFlag(TIM6, TIM_FLAG_Update);
     }
    OSIntExit();
}
