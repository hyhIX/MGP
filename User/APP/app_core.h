#ifndef __APP_CORE_H__
#define __APP_CORE_H__

#include <includes.h>

extern	uint8_t	SystemMode;

typedef enum
{
	//认证
	AUTH_SHOW						    =	0,
	AUTH_RFID_PASSWD_SHOW,
	AUTH_RFID_PASSWD_WHILE,
	AUTH_RFID_PASSWD_CHECK,
	AUTH_SUCCESS,
	AUTH_FAILURE,
	
	//设备类型
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

	//终端Light							on/off
	TERMINAL_LIGHT_SHOW					=	20,
	TERMINAL_LIGHT_WHILE,
	TERMINAL_LIGHT_CTRL,
	TERMINAL_LIGHT_CTRL_WHILE,
	TERMINAL_LIGHT_EXIT,
	
	//风扇Fan						    speed/send
	TERMINAL_FAN_SHOW                   =	30,
	TERMINAL_FAN_WHILE,
	TERMINAL_FAN_CTRL,
	TERMINAL_FAN_CTRL_WHILE,
	TERMINAL_FAN_EXIT,
	
	//窗帘Curtain						position/send
	TERMINAL_CURTAIN_SHOW				=	40,
	TERMINAL_CURTAIN_WHILE,
	TERMINAL_CURTAIN_CTRL,
	TERMINAL_CURTAIN_CTRL_WHILE,
	TERMINAL_CURTAIN_EXIT,

	//插座Socket						on/off
	TERMINAL_SOCKET_SHOW				=	50,
	TERMINAL_SOCKET_WHILE,
	TERMINAL_SOCKET_CTRL,
	TERMINAL_SOCKET_CTRL_WHILE,
	TERMINAL_SOCKET_EXIT,

	//环境Environment				    Temperature/Humidity
	TERMINAL_ENVIRONMENT_SHOW           =	60,
	TERMINAL_ENVIRONMENT_WHILE,
	TERMINAL_ENVIRONMENT_CTRL,
	TERMINAL_ENVIRONMENT_CTRL_WHILE,
	TERMINAL_ENVIRONMENT_EXIT,

	//火灾Fire							Smoke
	TERMINAL_FIRE_SHOW					=	70,
	TERMINAL_FIRE_WHILE,
	TERMINAL_FIRE_CTRL,
	TERMINAL_FIRE_CTRL_WHILE,
	TERMINAL_FIRE_EXIT,

	//安防Securit y					    RFID/AccessControl/Alarm
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
	
    MODE_CAN_MASTER_PING                = 220,  //主机心跳
    MODE_CAN_MASTER_NTP,                        //主机校时
    MODE_CAN_MASTER_LIST,                       //设备列表
    MODE_BOARD_MASTER_PING,
    
    MODE_CAN_CONTROL_SLAVE              = 230,  //从机控制
    MODE_CAN_CONTROL_TERMINAL,                  //设备控制
    MODE_CAN_CONTROL_SERVER,                    //服务器控制     手动更新
    
    MODE_CAN_DEVICE_LOGIN               = 240,  //设备注册
    MODE_CAN_DEVICE_LOGIN_RET,                  //设备注册回应
    MODE_CAN_DEVICE_UPDATE,                     //设备更新
    
	MODE_ESP8266_INIT                   = 250,  //面板与8266之间的
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

