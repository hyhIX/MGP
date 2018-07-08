#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__	 
#include "./SYS/bsp_sys.h"	    

#define CANx                    CAN1
#define CAN_CLK                 RCC_APB1Periph_CAN1

#define CAN_TX_IRQ              USB_HP_CAN1_TX_IRQn
#define CAN_TX_IRQHandler       USB_HP_CAN1_TX_IRQHandler

#define CAN_RX0_IRQ             USB_LP_CAN1_RX0_IRQn
#define CAN_RX0_IRQHandler      USB_LP_CAN1_RX0_IRQHandler
#define CAN_RX1_IRQ             CAN1_RX1_IRQn
#define CAN_RX1_IRQHandler      CAN1_RX1_IRQHandler

#define CAN_TX_GPIO_PIN         GPIO_Pin_12
#define CAN_TX_GPIO_MODE        GPIO_Mode_AF_PP             //��������
#define CAN_RX_GPIO_PIN         GPIO_Pin_11
#define CAN_RX_GPIO_MODE        GPIO_Mode_IPU               //��������

#define CAN_GPIO_PORT           GPIOA
#define CAN_GPIO_SPEED          GPIO_Speed_50MHz
#define CAN_GPIO_CLK            RCC_APB2Periph_GPIOA

#define CAN_TIME_SJW            CAN_SJW_1tq                 //BTR-SJW ����ͬ����Ծ���
#define CAN_TIME_BS1            CAN_BS1_9tq                 //BTR-TS1 ʱ���1
#define CAN_TIME_BS2            CAN_BS2_8tq                 //BTR-TS1 ʱ���2
#define CAN_PRESCALER           4                           //BTR-BRP �����ʷ�Ƶ��

#define CAN_PRIORITY_ID         536870912

void CAN_Config_Init( u8 mode );//CAN��ʼ��
void CAN_Filter_Set ( void );

uint8_t Can_Send_Msg( uint8_t* msg );   //��������


#endif


