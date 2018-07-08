#ifndef __APP_CAN_TX_H__
#define	__APP_CAN_TX_H__

#include "includes.h"

#pragma anon_unions
typedef struct
{
	uint8_t		FactoryID;			//����ID
	uint8_t		ModelID;			//�ͺ�ID
    uint8_t     Status;             //״̬
	uint32_t    UpdateTime;         //����ʱ��
	uint32_t	PriorityID;			//���ȼ�
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
		uint8_t		Mode;					//ʹ��0-255����ϵͳModeͨ��
		union
		{
			uint8_t		Control;			//��λ����ʾ8�ֲ�ͬ�Ŀ������ݣ�����Control��ʹ��
			uint8_t		Update;				//��λ����ʾ8�ֲ�ͬ�ĸ������ݣ�����Update��ʹ��
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


//�������͡���ʹ�ñ�׼֡
/* ����			�б�ģʽ */
#define		CAN_PRIORITY_ID_MASTER_PING			0x1A	            //��������
#define		CAN_PRIORITY_ID_MASTER_NTP			((uint32_t)0x1B)	//����Уʱʱ��
#define     CAN_PRIORITY_ID_MASTER_LIST         ((uint32_t)0x1C)    //�����豸�б�
#define		CAN_PRIORITY_ID_MASTER_TX_SLAVE		((uint32_t)0x1D)	//���Ϳ��Ʊ��ĸ��ӻ�
#define		CAN_PRIORITY_ID_MASTER_TX_TERMINAL	((uint32_t)0x1E)	//���Ϳ��Ʊ��ĸ��ն�

//���������͡���ʹ����չ֡����24bitΨһ��ʶ�豸
/* �������� 		����ģʽ		ʹ��ǰ5bit��Ϊ����ʶ�� */
#define		CAN_PRIORITY_ID_LOGIN_SLAVE			((uint32_t)0x0A<<24)
#define		CAN_PRIORITY_ID_LOGIN_TERMINAL		((uint32_t)0x0B<<24)
#define		CAN_PRIORITY_ID_SLAVE_CONTROL		((uint32_t)0x0C<<24)

/* ���ݸ��� 		����ģʽ		ʹ��ǰ5bit��Ϊ����ʶ�� */
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
	//��������
	FMI_MASTER_PING				= 0,
	FMI_MASTER_NTP,
	
	//�ӻ����ն�ע��
	FMI_SLAVE_LOGIN,
	FMI_TERMINAL_LOGIN,
	
	//�ӻ�����
	FMI_SLAVE_CONTROL,
	
	//�ն˸���
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
    RET_CAN_PING_MASTER_MODE_ERROR,//���ݶΣ�ģʽ����
    RET_CAN_PING_MASTER_REAL_ERROR,//����ģʽ����ǰ�뱾������
    RET_CAN_PING_MASTER_RECEIVE_ERROR,//����ģʽ���յ���������������
    RET_CAN_PING_MASTER_RECEIVE_SUCCESS,//�ɹ�
    
    RET_CAN_PING_NONE_RECEIVE_ERROR,//��ʼ��ģʽ���յ���������������
    RET_CAN_PING_NONE_SYNC_UNFINISHED,//��ʼ��ģʽ��δ���ͬ��
    RET_CAN_PING_NONE_RECEIVE_SUCCESS,//�ɹ�
    
    RET_CAN_PING_SLAVE_RECEIVE_ERROR,//�ӻ�ģʽ���յ���������������
    RET_CAN_PING_SLAVE_RECEIVE_SUCCESS,//�ɹ�
    
    RET_CAN_PING_MODE_ERROR,//����ģʽ����
    /*-------------------PING-------------------*/
    
    /*-------------------LOGIN-------------------*/
    RET_CAN_LOGIN_ID_ERROR,//�豸���ȼ���һ��
    RET_CAN_LOGIN_MODE_ERROR,//�豸ע�ᣬģʽ����   
    RET_CAN_LOGIN_TYPE_ERROR,//�豸���ʹ���
    RET_CAN_LOGIN_DEVICE_FOUND,//���б��в��ҵ��豸
    RET_CAN_LOGIN_LIST_FILLED_ERROR,//�б��������޷����
    RET_CAN_LOGIN_DEVICE_ADDED,//�����
    
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
    
    /*----�豸�б�ѡ��----*/
    CONTROL_LIST_FACTORY_ID     = 10,
    CONTROL_LIST_MODEL_ID,
    CONTROL_LIST_FLAG_ID,
    CONTROL_LIST_STATUS_ID,
    CONTROL_LIST_PRIORITY_ID,
    /*----�豸�б�ѡ��----*/
    
    /*----�豸����ѡ��----*/
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
    /*----�豸����ѡ��----*/
    
    /*------ͬ���б�------*/
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
    /*------ͬ���б�------*/
    
    CONTROL_MANUALLY_UPDATE     = 100,
    
    CONTROL_ESP8266_RET             = 220,
    
    /*------ESP8266��Ϣ����------*/
    CONTROL_ESP8266_WIFI_CONNECTING = 240,
    CONTROL_ESP8266_WIFI_CONNECTED,
    CONTROL_ESP8266_MQTT_CONNECTING,
    CONTROL_ESP8266_MQTT_CONNECTED,
    /*------ESP8266��Ϣ����------*/
    
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


