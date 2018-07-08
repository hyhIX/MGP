#ifndef __APP_CORE_H__
#define __APP_CORE_H__

#include <includes.h>

extern	uint8_t	SystemMode;

typedef enum
{
	//��֤
	AUTH_SHOW						    =	0,
	AUTH_RFID_PASSWD_SHOW,
	AUTH_RFID_PASSWD_WHILE,
	AUTH_RFID_PASSWD_CHECK,
	AUTH_SUCCESS,
	AUTH_FAILURE,
	
	//�豸����
	TYPE_SHOW						    =	10,
	TYPE_WHILE,
	TYPE_LIGHT,
	TYPE_FAN,
	TYPE_CURTAIN,
	TYPE_SOCKET,
	TYPE_ENVIRONMENT,
	TYPE_FIRE,
	TYPE_SECURITY,
	TYPE_EXIT,

	//�ն�Light							on/off
	TERMINAL_LIGHT_SHOW					=	20,
	TERMINAL_LIGHT_WHILE,
	TERMINAL_LIGHT_CTRL,
	TERMINAL_LIGHT_CTRL_WHILE,
	TERMINAL_LIGHT_EXIT,
	
	//����Fan						    speed/send
	TERMINAL_FAN_SHOW                   =	30,
	TERMINAL_FAN_WHILE,
	TERMINAL_FAN_CTRL,
	TERMINAL_FAN_CTRL_WHILE,
	TERMINAL_FAN_EXIT,
	
	//����Curtain						position/send
	TERMINAL_CURTAIN_SHOW				=	40,
	TERMINAL_CURTAIN_WHILE,
	TERMINAL_CURTAIN_CTRL,
	TERMINAL_CURTAIN_CTRL_WHILE,
	TERMINAL_CURTAIN_EXIT,

	//����Socket						on/off
	TERMINAL_SOCKET_SHOW				=	50,
	TERMINAL_SOCKET_WHILE,
	TERMINAL_SOCKET_CTRL,
	TERMINAL_SOCKET_CTRL_WHILE,
	TERMINAL_SOCKET_EXIT,

	//����Environment				    Temperature/Humidity
	TERMINAL_ENVIRONMENT_SHOW           =	60,
	TERMINAL_ENVIRONMENT_WHILE,
	TERMINAL_ENVIRONMENT_CTRL,
	TERMINAL_ENVIRONMENT_CTRL_WHILE,
	TERMINAL_ENVIRONMENT_EXIT,

	//����Fire							Smoke
	TERMINAL_FIRE_SHOW					=	70,
	TERMINAL_FIRE_WHILE,
	TERMINAL_FIRE_CTRL,
	TERMINAL_FIRE_CTRL_WHILE,
	TERMINAL_FIRE_EXIT,

	//����Securit y					    RFID/AccessControl/Alarm
	TERMINAL_SECURITY_SHOW              =	80,
	TERMINAL_SECURITY_WHILE,
	TERMINAL_SECURITY_CTRL,
	TERMINAL_SECURITY_CTRL_WHILE,
	TERMINAL_SECURITY_EXIT,	
	
	
	CONTROL_INIT		                = 200,
	CONTROL_ON,
	CONTROL_OFF,
	CONTROL_EXIT,
	
	CONTROL_SUCCESS                     = 210,
	CONTROL_FAILURE,
	
    MODE_CAN_MASTER_PING                = 220,  //��������
    MODE_CAN_MASTER_NTP,                        //����Уʱ
    MODE_CAN_MASTER_LIST,                       //�豸�б�
    MODE_BOARD_MASTER_PING,
    
    MODE_CAN_CONTROL_SLAVE              = 230,  //�ӻ�����
    MODE_CAN_CONTROL_TERMINAL,                  //�豸����
    MODE_CAN_CONTROL_SERVER,                    //����������     �ֶ�����
    
    MODE_CAN_DEVICE_LOGIN               = 240,  //�豸ע��
    MODE_CAN_DEVICE_LOGIN_RET,                  //�豸ע���Ӧ
    MODE_CAN_DEVICE_UPDATE,                     //�豸����
    
	MODE_ESP8266_INIT                   = 250,  //�����8266֮���
    MODE_ESP8266_RETINIT,
    MODE_ESP8266_PING,
    MODE_ESP8266_MSG,
    MODE_ESP8266_CHIPID,
	
}tMode;


void Authenticate_Config ( void );
void LCD_Show_Authenticate ( void );
void LCD_Show_Passwd( void );
tMode Authenticate( tMode mode );
tMode RFID_Authenticate( void );
tMode Passwd_Authenticate( void );
tMode RFID_ID_Match ( void );
tMode LCD_Touch ( tMode mode );
tMode Touch_Passwd ( tMode mode );
tMode Touch_Type ( void );
tMode Touch_Terminal ( tMode mode );
tMode Touch_Control ( tMode mode );
void LCD_Show_Type ( void );
void LCD_Show_Terminal ( tMode mode );
void LCD_Show_Control ( tMode mode );
void LCD_Show_Control_UpDown ( tMode mode );
void LCD_Show_Control_Data ( tMode mode );

#endif

