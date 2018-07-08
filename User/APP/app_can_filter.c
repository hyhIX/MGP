#include "stm32f10x_can.h"
#include "./CAN/bsp_can.h"
#include "app_can_tx.h"

CAN_FilterInitTypeDef CAN_FilterInitStruct[14] = 
{
	{//0
		.CAN_FilterNumber 			=	FMI_MASTER_PING,
		.CAN_FilterMode				=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_MASTER_PING<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_MASTER_PING<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO0,
		.CAN_FilterActivation		=	ENABLE
	},
	{//1
		.CAN_FilterNumber			=	FMI_MASTER_NTP,
		.CAN_FilterMode 			=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_MASTER_NTP<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_MASTER_NTP<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO0,
		.CAN_FilterActivation		=	ENABLE
	},
	{//2
		.CAN_FilterNumber			=	FMI_SLAVE_LOGIN,
		.CAN_FilterMode 			=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO0,
		.CAN_FilterActivation		=	ENABLE
	},
    {//3
		.CAN_FilterNumber			=	FMI_TERMINAL_LOGIN,
		.CAN_FilterMode 			=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO0,
		.CAN_FilterActivation		=	ENABLE
	},
    {//4
		.CAN_FilterNumber			=	FMI_SLAVE_CONTROL,
		.CAN_FilterMode 			=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO0,
		.CAN_FilterActivation		=	ENABLE
	},
    {//5
		.CAN_FilterNumber			=	FMI_UPDATE_LIGHT,
		.CAN_FilterMode 			=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO1,
		.CAN_FilterActivation		=	DISABLE
	},
    {//6
		.CAN_FilterNumber			=	FMI_UPDATE_FAN,
		.CAN_FilterMode 			=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO1,
		.CAN_FilterActivation		=	DISABLE
	},
    {//7
		.CAN_FilterNumber			=	FMI_UPDATE_CURTAIN,
		.CAN_FilterMode 			=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO1,
		.CAN_FilterActivation		=	DISABLE
	},
    {//8
		.CAN_FilterNumber			=	FMI_UPDATE_SOCKET,
		.CAN_FilterMode 			=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO1,
		.CAN_FilterActivation		=	DISABLE
	},
    {//9
		.CAN_FilterNumber			=	FMI_UPDATE_ENVIRONMENT,
		.CAN_FilterMode 			=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO1,
		.CAN_FilterActivation		=	DISABLE
	},
    {//10
		.CAN_FilterNumber			=	FMI_UPDATE_FIRE,
		.CAN_FilterMode 			=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO1,
		.CAN_FilterActivation		=	DISABLE
	},
    {//11
		.CAN_FilterNumber			=	FMI_UPDATE_SECURITY,
		.CAN_FilterMode 			=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO1,
		.CAN_FilterActivation		=	DISABLE
	},
    {//12
		.CAN_FilterNumber			=	FMI_SLAVE_LOGIN,
		.CAN_FilterMode 			=	CAN_FilterMode_IdList,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	(((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16,
		.CAN_FilterIdLow			=	((CAN_PRIORITY_ID_LOGIN_SLAVE<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF,
		.CAN_FilterMaskIdHigh		=	0xFFFF,
		.CAN_FilterMaskIdLow		=	0XFFFF,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO1,
		.CAN_FilterActivation		=	DISABLE
	},
    {//13
		.CAN_FilterNumber			=	FMI_RECEIVE_ALL,
		.CAN_FilterMode 			=	CAN_FilterMode_IdMask,
		.CAN_FilterScale			=	CAN_FilterScale_32bit,
		.CAN_FilterIdHigh			=	0x0000,
		.CAN_FilterIdLow			=	0x0000,
		.CAN_FilterMaskIdHigh		=	0x0000,
		.CAN_FilterMaskIdLow		=	0x0000,
		.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO1,
		.CAN_FilterActivation		=	ENABLE
	}

};
