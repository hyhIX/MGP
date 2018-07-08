#ifndef __APP_CAN_TX_H__
#define	__APP_CAN_TX_H__

#include "includes.h"

#pragma anon_unions
typedef struct
{
	uint8_t		FactoryID;			//厂商ID
	uint8_t		ModelID;			//型号ID
    uint8_t     Status;             //状态
	uint32_t    UpdateTime;         //更新时间
	uint32_t	PriorityID;			//优先级
	union
	{
		uint8_t		Update_Byte[4];
		uint16_t	Update_HalfWord[2];
		uint32_t	Update_FullWord;
	};	

} tList;

typedef enum
{
    DECVICE_NONE        = 0,
    DECVICE_NORMAL,
    DECVICE_ERROR,
    
    
} tDecviceStatus;

typedef union
{
    struct
    {
        uint8_t		Light           :1;
        uint8_t		Fan             :1;
        uint8_t		Curtain         :1;
        uint8_t		Socket          :1;
        uint8_t		Environment     :1;
        uint8_t		Fire            :1;
        uint8_t		Security        :1;
        uint8_t		Slave           :1;
    };
    uint8_t     Type;
} tListStatus;

typedef union
{
	struct
	{
		uint8_t		Type						:4;
		uint8_t		Terminal					:4;
	};
	uint8_t		Mode;
}tModeBit;


typedef struct
{
	struct
	{
		uint8_t		Mode;					//使用0-255，与系统Mode通用
		union
		{
			uint8_t		Control;			//按位，表示8种不同的控制内容，决定Control的使用
			uint8_t		Update;				//按位，表示8种不同的更新内容，决定Update的使用
		};
		union
		{
			uint8_t		Control_Byte[2];
			uint16_t	Control_HalfWord;
		};
	};
	union
	{
		uint8_t		Update_Byte[4];
		uint16_t	Update_HalfWord[2];
		uint32_t	Update_FullWord;
		uint32_t	PriorityID;
	};
} tMsg;

typedef union
{
    struct
    {
        uint8_t Clock_LSE           :1;
        uint8_t Clock_LSI           :1;
        uint8_t Usart_Mode          :1;
        uint8_t Usart_Control       :1;
    };
    uint8_t Alarm;
} tAlarm;

typedef union
{
    struct
    {
        uint8_t Timeout_ping        :1;
        uint8_t Timeout_Restart     :1;
        uint8_t Timeout_Wifi        :1;
        uint8_t Timeout_Mqtt        :1;
    
    };
    uint8_t Error;
} tError;


//主机发送——使用标准帧
/* 主机			列表模式 */
#define		CAN_PRIORITY_ID_MASTER_PING			0x1A	            //发送心跳
#define		CAN_PRIORITY_ID_MASTER_NTP			((uint32_t)0x1B)	//发送校时时间
#define     CAN_PRIORITY_ID_MASTER_LIST         ((uint32_t)0x1C)    //发送设备列表
#define		CAN_PRIORITY_ID_MASTER_TX_SLAVE		((uint32_t)0x1D)	//发送控制报文给从机
#define		CAN_PRIORITY_ID_MASTER_TX_TERMINAL	((uint32_t)0x1E)	//发送控制报文给终端

//非主机发送——使用扩展帧、后24bit唯一标识设备
/* 主机服务 		掩码模式		使用前5bit作为服务识别 */
#define		CAN_PRIORITY_ID_LOGIN_SLAVE			((uint32_t)0x0A<<24)
#define		CAN_PRIORITY_ID_LOGIN_TERMINAL		((uint32_t)0x0B<<24)
#define		CAN_PRIORITY_ID_SLAVE_CONTROL		((uint32_t)0x0C<<24)

/* 数据更新 		掩码模式		使用前5bit作为服务识别 */
#define		CAN_PRIORITY_ID_TYPE_LIGHT			((uint32_t)0x10<<24)
#define		CAN_PRIORITY_ID_TYPE_FAN			((uint32_t)0x1A<<24)
#define		CAN_PRIORITY_ID_TYPE_CURTAIN		((uint32_t)0x1B<<24)
#define		CAN_PRIORITY_ID_TYPE_SOCKET			((uint32_t)0x1C<<24)
#define		CAN_PRIORITY_ID_TYPE_ENVIRONMENT	((uint32_t)0x1D<<24)
#define		CAN_PRIORITY_ID_TYPE_FIRE			((uint32_t)0x1E<<24)
#define		CAN_PRIORITY_ID_TYPE_SECURITY		((uint32_t)0x1F<<24)


#define		LIST_TYPE_MAX						6


typedef enum
{
	NONE			=0,
	MASTER,
	SLAVE,
} tSystemMode;


typedef enum
{
	//主机管理
	FMI_MASTER_PING				= 0,
	FMI_MASTER_NTP,
	
	//从机、终端注册
	FMI_SLAVE_LOGIN,
	FMI_TERMINAL_LOGIN,
	
	//从机控制
	FMI_SLAVE_CONTROL,
	
	//终端更新
	FMI_UPDATE_LIGHT,
	FMI_UPDATE_FAN,
	FMI_UPDATE_CURTAIN,
	FMI_UPDATE_SOCKET,
	FMI_UPDATE_ENVIRONMENT,
	FMI_UPDATE_FIRE,
	FMI_UPDATE_SECURITY,
	
	FMI_RECEIVE_ALL       = 13,

	FMI_MAX_NUM,
	
} tFilterNum;

typedef enum
{
	RET_INIT														= 0,
	
    /*-------------------PING-------------------*/
    RET_CAN_PING_MASTER_MODE_ERROR,//数据段，模式错误
    RET_CAN_PING_MASTER_REAL_ERROR,//主机模式，当前与本机不符
    RET_CAN_PING_MASTER_RECEIVE_ERROR,//主机模式，收到其他主机的心跳
    RET_CAN_PING_MASTER_RECEIVE_SUCCESS,//成功
    
    RET_CAN_PING_NONE_RECEIVE_ERROR,//初始化模式，收到其他主机的心跳
    RET_CAN_PING_NONE_SYNC_UNFINISHED,//初始化模式，未完成同步
    RET_CAN_PING_NONE_RECEIVE_SUCCESS,//成功
    
    RET_CAN_PING_SLAVE_RECEIVE_ERROR,//从机模式，收到其他主机的心跳
    RET_CAN_PING_SLAVE_RECEIVE_SUCCESS,//成功
    
    RET_CAN_PING_MODE_ERROR,//本机模式错误
    /*-------------------PING-------------------*/
    
    /*-------------------LOGIN-------------------*/
    RET_CAN_LOGIN_ID_ERROR,//设备优先级不一致
    RET_CAN_LOGIN_MODE_ERROR,//设备注册，模式错误   
    RET_CAN_LOGIN_TYPE_ERROR,//设备类型错误
    RET_CAN_LOGIN_DEVICE_FOUND,//在列表中查找到设备
    RET_CAN_LOGIN_LIST_FILLED_ERROR,//列表已满，无法添加
    RET_CAN_LOGIN_DEVICE_ADDED,//已添加
    
    /*-------------------LOGIN-------------------*/
    
	RET_DEVICE_MGMT_FAILURE_MODE_ERROR,
	RET_DEVICE_MGMT_FAILURE_LIST_FULL,
	RET_DEVICE_MGMT_TRUE_ADDED,
	
	RET_SYSTEM_MODE_ERROR,
	RET_SYSTEM_MODE_TRUE,
	RET_SYSTEM_MODE_FAILURE,
	RET_SYSTEM_MODE_CHANGE,
	
} tMgmtRet;


typedef enum
{
	LIST_TYPE_LIGHT,
	LIST_TYPE_FAN,
	LIST_TYPE_CURTAIN,
	LIST_TYPE_SOCKET,
	LIST_TYPE_ENVIRONMENT,
	LIST_TYPE_FIRE,
	LIST_TYPE_SECURITY,
	LIST_TYPE_SLAVE,

	SYSTEM_MODE_MASTER,
	
} tUpdate;

typedef enum
{
	CONTROL_CAN_MASTER_PING     = 0,
    CONTROL_CAN_MASTER_NTP,
    CONTROL_BOARD_MASTER_PING,
    
    /*----设备列表选择----*/
    CONTROL_LIST_FACTORY_ID     = 10,
    CONTROL_LIST_MODEL_ID,
    CONTROL_LIST_FLAG_ID,
    CONTROL_LIST_STATUS_ID,
    CONTROL_LIST_PRIORITY_ID,
    /*----设备列表选择----*/
    
    /*----设备类型选择----*/
    CONTROL_TYPE_MIN            = 20,
    CONTROL_TYPE_LIGHT,
	CONTROL_TYPE_FAN,
	CONTROL_TYPE_CURTAIN,
	CONTROL_TYPE_SOCKET,
	CONTROL_TYPE_ENVIRONMENT,
	CONTROL_TYPE_FIRE,
	CONTROL_TYPE_SECURITY,
	CONTROL_TYPE_SLAVE,
    CONTROL_TYPE_MAX,
    /*----设备类型选择----*/
    
    /*------同步列表------*/
    CONTROL_LOGIN_MIN            = 50,
    CONTROL_LOGIN_LIGHT,
    CONTROL_LOGIN_FAN,
    CONTROL_LOGIN_CURTAIN,
    CONTROL_LOGIN_SOCKET,
    CONTROL_LOGIN_ENVIRONMENT,
    CONTROL_LOGIN_FIRE,
    CONTROL_LOGIN_SECURIT,
    CONTROL_LOGIN_SLAVE,
    CONTROL_LOGIN_MAX,
    /*------同步列表------*/
    
    CONTROL_MANUALLY_UPDATE     = 100,
    
    CONTROL_ESP8266_RET             = 220,
    
    /*------ESP8266信息报文------*/
    CONTROL_ESP8266_WIFI_CONNECTING = 240,
    CONTROL_ESP8266_WIFI_CONNECTED,
    CONTROL_ESP8266_MQTT_CONNECTING,
    CONTROL_ESP8266_MQTT_CONNECTED,
    /*------ESP8266信息报文------*/
    
    CONTROL_ESP8266_INIT        = 250,
    
} tControl;


#define     CAN_LIST_TYPE_MAX           7
#define     LIST_DECVICE_FACTORY_ID     0x99

#define		CAN_RX_MEM_MAX_NUMBER		100

#define		CAN_MASTER_PING_TX_TIMEOUT  3
#define     CAN_MASTER_PING_RX_TIMEOUT  10
#define     CAN_MASTER_NTP_TX_TIMEOUT   10

#define     CAN_MESSAGE_LENGTH          8

uint8_t getSystemMode ( void );

#endif


