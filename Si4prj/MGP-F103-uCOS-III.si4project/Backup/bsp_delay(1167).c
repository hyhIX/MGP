#include "includes.h"
#include "./DELAY/bsp_delay.h"


static uint8_t _delayTimeOut;

/*
*   ʹ��Ӳ����ʱ��TIM6��������0.1ms
*
*
*/
void BSP_Delay_Init(uint16_t arr,uint16_t psc)	 
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(DELAY_TIMx_CLK, ENABLE); //ʱ��ʹ��

    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(DELAY_TIMx, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ITConfig( TIM6,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = DELAY_TIMx_IRQ;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

    TIM_Cmd(DELAY_TIMx, ENABLE);  //ʹ��TIMx
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
