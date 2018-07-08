#include <string.h>
#include "includes.h"
#include "./LCD/bsp_lcd.h"
#include "./CAN/bsp_can.h"
#include "./BUZZER/bsp_buzzer.h"
#include "./TOUCH/bsp_touch.h"
#include "./EEPROM/bsp_eeprom.h"
#include "./RC522/bsp_rc522_config.h"
#include "./RC522/bsp_rc522_function.h"

#include "picture.h"
#include "app_core.h"
#include "app_can_tx.h"
#include "app_init.h"

//#include "app_chinese.h"

extern	tList       TerminalList[8][7];
extern	uint32_t	smasterPriorityID;
extern  tLcdDevice  gLcdDevice;

static const uint8_t	VersionID[] = {"MGP-u0.7 2018-6-3"};
static char stypeName[8][4]={"照明","风扇","窗帘","插座","环境","火灾","安防","返回"};

static uint8_t		sRfidCardMax;
static uint32_t 	sRfidCardID[10];
static uint32_t 	sRfidCardIDTmp;
static uint16_t 	sPasswd;
static uint16_t 	sPasswdTmp;
static uint8_t		sPasswdDigits;

static tModeBit     sModeBit;

//Core
OS_TCB   AppTaskCoreTCB;
CPU_STK  AppTaskCoreStk[APP_TASK_CORE_STK_SIZE];
void  AppTaskCore   (void *p_arg);

void  AppTaskCore   ( void *p_arg )
{    
	OS_ERR	err;
	tMode mode = AUTH_SHOW;

	while ( 1 )
	{            
		switch ( (uint8_t)mode )
		{            
			/****************************************认证****************************************/
			case	AUTH_SHOW:						LCD_Show_Authenticate(  );		//显示初始界面
			case	AUTH_RFID_PASSWD_SHOW:			LCD_Show_Passwd(  );			//显示认证界面 
			case	AUTH_RFID_PASSWD_WHILE:			mode = AUTH_RFID_PASSWD_WHILE;
                                                    sPasswdTmp = 0;					//临时存放密码
													sPasswdDigits = 0;				//已经输入密码的位数
			case	AUTH_RFID_PASSWD_CHECK:			mode = Authenticate( mode );	//扫描触摸或RFID卡,并认证
												break;
			case	AUTH_SUCCESS:					mode = TYPE_SHOW;				//成功跳转到下一级
												break;
			case	AUTH_FAILURE:					mode = AUTH_RFID_PASSWD_WHILE;	//返回初始界面
												break;
			/****************************************End****************************************/
			

			/****************************选择控制设备类型,跳转到下一级****************************/
			case	TYPE_SHOW:						LCD_Show_Type(  );				//显示控制设备类型选择界面
			case	TYPE_WHILE:						mode = TYPE_WHILE;
													mode = LCD_Touch( mode );		//循环获取触摸坐标
												break;
			case	TYPE_LIGHT:						mode = TERMINAL_LIGHT_SHOW;
												break;
			case	TYPE_FAN:						mode = TERMINAL_FAN_SHOW;
												break;
			case	TYPE_CURTAIN:					mode = TERMINAL_CURTAIN_SHOW;
												break;
			case	TYPE_SOCKET:					mode = TERMINAL_SOCKET_SHOW;
												break;
			case	TYPE_ENVIRONMENT:				mode = TERMINAL_ENVIRONMENT_SHOW;
												break;
			case	TYPE_FIRE:						mode = TERMINAL_FIRE_SHOW;
												break;
			case	TYPE_SECURITY:					mode = TERMINAL_SECURITY_SHOW;
												break;
			case	TYPE_EXIT:						mode = AUTH_SHOW;				//返回上一级
												break;
			/****************************************End****************************************/
			
			
			/******************************选择照明编号,跳转到下一级******************************/
			case	TERMINAL_LIGHT_SHOW:			LCD_Show_Terminal( mode++ );	//显示终端类型选择界面
			case	TERMINAL_LIGHT_WHILE:			mode = LCD_Touch( mode );		//循环获取触摸坐标
												break;
			case	TERMINAL_LIGHT_CTRL:			LCD_Show_Control( mode++ );
			case	TERMINAL_LIGHT_CTRL_WHILE:		mode = LCD_Touch( mode );		//循环获取触摸坐标
												break;
			case	TERMINAL_LIGHT_EXIT:			mode = TERMINAL_LIGHT_SHOW;		//返回上一级
												break;
			/****************************************End****************************************/
			
			
			/******************************选择风扇编号,跳转到下一级******************************/
			case	TERMINAL_FAN_SHOW:				LCD_Show_Terminal( mode++ );	//显示终端类型选择界面
			case	TERMINAL_FAN_WHILE:				mode = LCD_Touch( mode );		//循环获取触摸坐标
												break;		
			case	TERMINAL_FAN_CTRL:				LCD_Show_Control_UpDown( mode++ );
			case	TERMINAL_FAN_CTRL_WHILE:		mode = LCD_Touch( mode );		//循环获取触摸坐标
												break;
			case	TERMINAL_FAN_EXIT:				mode = TERMINAL_FAN_SHOW;		//返回上一级
												break;
			/****************************************End****************************************/
			
			
			/******************************选择窗帘编号,跳转到下一级******************************/
			case	TERMINAL_CURTAIN_SHOW:          LCD_Show_Terminal( mode++ );    //显示终端类型选择界面
			case	TERMINAL_CURTAIN_WHILE:         mode = LCD_Touch( mode );       //循环获取触摸坐标
												break;	
			case	TERMINAL_CURTAIN_CTRL:			LCD_Show_Control( mode++ );
			case	TERMINAL_CURTAIN_CTRL_WHILE:    mode = LCD_Touch( mode );       //循环获取触摸坐标
												break;
			case	TERMINAL_CURTAIN_EXIT:		    mode = TERMINAL_CURTAIN_SHOW;   //返回上一级
												break;
			/****************************************End****************************************/
			
			
			/******************************选择插座编号,跳转到下一级******************************/
			case	TERMINAL_SOCKET_SHOW:           LCD_Show_Terminal( mode++ );	//显示终端类型选择界面
			case	TERMINAL_SOCKET_WHILE:			mode = LCD_Touch( mode );	    //循环获取触摸坐标
												break;	
			case	TERMINAL_SOCKET_CTRL:			LCD_Show_Control( mode++ );
			case	TERMINAL_SOCKET_CTRL_WHILE:		mode = LCD_Touch( mode );       //循环获取触摸坐标
												break;
			case	TERMINAL_SOCKET_EXIT:			mode = TERMINAL_CURTAIN_SHOW;	//返回上一级
												break;
			/****************************************End****************************************/
			
			
			/******************************选择环境编号,跳转到下一级******************************/
			case	TERMINAL_ENVIRONMENT_SHOW:	    LCD_Show_Terminal( mode++ );    //显示终端类型选择界面
			case	TERMINAL_ENVIRONMENT_WHILE:	    mode = LCD_Touch( mode );       //循环获取触摸坐标
												break;	
			case	TERMINAL_ENVIRONMENT_CTRL:		LCD_Show_Control_Data( mode++ );
			case	TERMINAL_ENVIRONMENT_CTRL_WHILE:mode = LCD_Touch( mode );       //循环获取触摸坐标
												break;
			case	TERMINAL_ENVIRONMENT_EXIT:		mode = TERMINAL_CURTAIN_SHOW;   //返回上一级
												break;
			/****************************************End****************************************/
			
			
			/******************************选择火灾编号,跳转到下一级******************************/
			case	TERMINAL_FIRE_SHOW:             LCD_Show_Terminal( mode++ );    //显示终端类型选择界面
			case	TERMINAL_FIRE_WHILE:            mode = LCD_Touch( mode );       //循环获取触摸坐标
                                                break;	
			case	TERMINAL_FIRE_CTRL:             LCD_Show_Control_Data( mode++ );
			case	TERMINAL_FIRE_CTRL_WHILE:       mode = LCD_Touch( mode );       //循环获取触摸坐标
                                                break;
			case	TERMINAL_FIRE_EXIT:             mode = TERMINAL_CURTAIN_SHOW;   //返回上一级
                                                break;
			/****************************************End****************************************/
			
			
			/******************************选择安防编号,跳转到下一级******************************/
			case	TERMINAL_SECURITY_SHOW:         LCD_Show_Terminal( mode++ );    //显示终端类型选择界面
			case	TERMINAL_SECURITY_WHILE:        mode = LCD_Touch( mode );       //循环获取触摸坐标
                                                break;	
			case	TERMINAL_SECURITY_CTRL:         LCD_Show_Control_Data( mode++ );
			case	TERMINAL_SECURITY_CTRL_WHILE:   mode = LCD_Touch( mode );       //循环获取触摸坐标
                                                break;
			case	TERMINAL_SECURITY_EXIT:         mode = TERMINAL_CURTAIN_SHOW;   //返回上一级
                                                break;
			/****************************************End****************************************/
			
			case	CONTROL_SUCCESS:
			case	CONTROL_FAILURE:                mode = TYPE_SHOW;
                                                break;
            default:                                mode = AUTH_SHOW;
                                                break;
		}
		
		//循环显示数据
        uint8_t tmp = mode;
		if ( ((tmp/10) >= 2) && ((tmp/10) <= 8) )
        {            
            if ( ((tmp%10)==2) || ((tmp%10)==3) )
            {
                uint8_t type        = sModeBit.Type -1;
                uint8_t terminal    = sModeBit.Terminal -1;
                BSP_LCD_ShowxNum( 90, 120,  TerminalList[type][terminal].Update_Byte[0], 3, 16, 0 );
                BSP_LCD_ShowxNum( 200, 120, TerminalList[type][terminal].Update_Byte[1], 3, 16, 0 );
            }
        }
        
        OSTimeDlyHMSM ( 0, 0, 0, 100, OS_OPT_TIME_DLY, &err );
	}
}

/*
*		函数名：	Authenticate_Config
*		描述：		初始化RFID+Passwd
*		输入：		无
*		输出：		无
*/
void Authenticate_Config ( void )
{
	//RFID
	sRfidCardMax = APP_Get_RfidID_Management( sRfidCardID );
    
    for ( uint8_t i=0;i<sRfidCardMax;i++ )
        printf( "%8x\n",sRfidCardID[i] );
    
	//Passwd
	sPasswd = APP_Get_Passwd(  );
}


/*
*	函数名：	LCD_Show_Authenticate
*	描述：	显示认证界面( RFID + PASSWD )
*	输入：	无
*	输出：	无
*/
void LCD_Show_Authenticate ( void )
{
	OS_ERR      err;
	
	//LCD显示
	BSP_LCD_Clear(WHITE);
	BSP_LCD_ShowString(40,60,200,16,16,(uint8_t *)"Please Authenticate");
	
	//显示图片
	BSP_LCD_DrawPicture(0,100,240,133,(uint8_t *)Picture_Authentication);
	
	//认证方式提示
	BSP_LCD_ShowString(20,240,200,16,16,(uint8_t *)"Please enter your password");
	BSP_LCD_ShowString(20,280,200,16,16,(uint8_t *)"Or swipe your RFID card");
	
	BSP_LCD_ShowString(80,300,200,16,16,(uint8_t *)VersionID);
	
	//读取EEPROM,初始化数据
	BSP_LCD_ShowStringNotStack(20,260,200,16,16,(uint8_t *)"Initializing from EEPROM");

	Authenticate_Config(  );
	BSP_LCD_ShowStringNotStack(20,260,200,16,16,(uint8_t *)"       Initialized      ");
	
	OSTimeDlyHMSM ( 0, 0, 3, 0, OS_OPT_TIME_DLY, &err );
}

/*
*	函数名：	LCD_Show_Passwd
*	描述：	显示输入密码界面,并提示可以刷卡
*	输入：	无
*	输出：	无
*/
void LCD_Show_Passwd( void )
{
	BSP_LCD_Clear(WHITE);
	
	//显示颜色块
	for ( uint8_t i=0; i<3; i++ )
	{
		BSP_LCD_Fill( 20,  110+60*i, 110, 160+60*i, LIGHTGREEN );
		BSP_LCD_Fill( 130, 110+60*i, 220, 160+60*i, LIGHTGREEN );
	}
	
	//显示数字
	uint8_t num = 1;
	for ( uint8_t i=1; i<3; i++ )
	{
		BSP_LCD_ShowxNum( 60,  70+60*i, num++, 1, 16, 1 );
        BSP_LCD_ShowxNum( 170, 70+60*i, num++, 1, 16, 1 );
	}
	
	//显示确认、删除
    BSP_LCD_DrawString_CH(50,   250, "确认", 2);
    BSP_LCD_DrawString_CH(160,  250, "删除", 2);
      
	BSP_LCD_ShowString( 30, 300, 200, 16, 16, (uint8_t *)"You can also swipe cards" );
}

/*
*	函数名：	Authenticate
*	描述：	认证( RFID + Passwd )
*	输入：	mode
*	输出：	ret_mode
*/
tMode Authenticate( tMode mode )
{
	//扫描RFID卡,并进行识别
	if ( RFID_Authenticate(  ) == AUTH_SUCCESS )
		return AUTH_SUCCESS;
    
	//扫描屏幕按键
	return LCD_Touch ( mode );
}

/*
*	函数名：	RFID_Authenticate
*	描述：	扫描RFID卡，并进行识别认证
*	输入：	无
*	输出：	mode(识别成功、识别失败)
*/
tMode RFID_Authenticate( void )
{
	uint8_t ret;						//返回状态
	uint8_t	RFID_Tmp[4];
	
    memset( RFID_Tmp, 0 ,sizeof( RFID_Tmp ) );
    sRfidCardIDTmp = 0;
    
	//寻卡
	if ( ( ret = PcdRequest ( PICC_REQALL, RFID_Tmp ) ) != MI_OK )                                   
		ret = PcdRequest ( PICC_REQALL, RFID_Tmp );	//若失败再次寻卡	   
	if ( ret == MI_OK  )
	{
		//防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）
		if ( PcdAnticoll ( RFID_Tmp ) == MI_OK )  
		{
			sRfidCardIDTmp |= RFID_Tmp[0] << 24;
			sRfidCardIDTmp |= RFID_Tmp[1] << 16;
			sRfidCardIDTmp |= RFID_Tmp[2] << 8;
			sRfidCardIDTmp |= RFID_Tmp[3];

            printf("Check %02x%02x%02x%02x\n",RFID_Tmp[0],RFID_Tmp[1],RFID_Tmp[2],RFID_Tmp[3] );
            
			return RFID_ID_Match(  );
		}
	}
	
	return AUTH_FAILURE;
}

/*
*	函数名：	RFID_ID_Match
*	描述：	匹配RFID卡序列号
*	输入：	无(使用全局变量RFID_Tmp、RFID_Card)
*	输出：	mode(识别成功、识别失败)
*/
tMode	RFID_ID_Match ( void )
{
	for ( uint8_t i=0; i<sRfidCardMax; i++ )
	{
		if (  sRfidCardIDTmp == sRfidCardID[i] )
		{
            BSP_LCD_ShowString( 15, 70, 240, 16, 16, (uint8_t *)"Passwd_Authenticate Success" );
            printf("RFID Check Success:%x\n", sRfidCardIDTmp);
			return AUTH_SUCCESS;
		}
	}
    
    printf("RFID Check Failure:%x\n", sRfidCardIDTmp);
    BSP_Buzzer_Alarm( 3 );
    
	return AUTH_FAILURE;
}

/*
*	函数名：	Passwd_Authenticate
*	描述：	密码验证
*	输入：	无(使用全局变量Passwd、Passwd_Tmp)
*	输出：	mode
*/
tMode Passwd_Authenticate( void )
{
	sPasswdTmp /= 10;

	if ( sPasswdTmp == sPasswd )
	{
		BSP_LCD_ShowString( 15, 70, 240, 16, 16, (uint8_t *)"Passwd_Authenticate Success" );
		printf("RFID Check Success:%u\n", sPasswdTmp );
        return AUTH_SUCCESS;
	}
	BSP_LCD_Fill( 0, 30, 240, 90, WHITE );
    
    printf("Passwd Check Failure:%u\n",sPasswdTmp);
    BSP_Buzzer_Alarm( 3 );
    
	return AUTH_FAILURE;
}

/*
*	函数名：	LCD_Touch
*	描述：	获取触摸坐标，并根据模式，调用相应的处理函数
*	输入：	mode
*	输出：	mode
*/
tMode LCD_Touch ( tMode mode )
{
	OS_ERR 	err;
	tMode		ret_mode;
    
	//扫描触摸屏
	tp_dev.scan(0);
	
	//触摸屏被按下
	if( tp_dev.sta & TP_PRES_DOWN )
	{
		//触摸坐标是否在屏幕范围内
		if( tp_dev.x < gLcdDevice.width && tp_dev.y < gLcdDevice.height )
		{
			#ifdef DEBUG
				printf( "Touch X=%d Y=%d\n\r", tp_dev.x, tp_dev.y );
				printf( "Touch mode=%d\n", mode );
			#endif
			
			//调用相应的处理函数
			if			( mode == AUTH_RFID_PASSWD_WHILE || mode == AUTH_RFID_PASSWD_CHECK )				//认证
			{
                #ifdef DEBUG
                printf( "AUTH_RFID_PASSWD_WHILE\n" );
                #endif
				ret_mode = Touch_Passwd( mode );
			}
			else if ( mode == TYPE_WHILE )
			{
                #ifdef DEBUG
                printf( "TYPE_WHILE\n" );
                #endif
				ret_mode = Touch_Type(  );
			}
			else if ( mode == TERMINAL_LIGHT_WHILE			||	mode == TERMINAL_FAN_WHILE		\
                    ||	mode == TERMINAL_CURTAIN_WHILE		||	mode == TERMINAL_SOCKET_WHILE	\
                    ||	mode == TERMINAL_ENVIRONMENT_WHILE||	mode ==	TERMINAL_FIRE_WHILE		\
                    ||	mode == TERMINAL_SECURITY_WHILE )
			{
                #ifdef DEBUG
                printf( "TERMINAL_WHILE\n" );
                #endif
				ret_mode = Touch_Terminal( mode );
			}
			else if ( ( sModeBit.Type != 0 && sModeBit.Terminal !=0 )
					 	||  ( mode > 20 && mode < 160 ) )
			{
                #ifdef DEBUG
                printf( "Terminal\n" );
                #endif
				ret_mode = Touch_Control( mode );
			}
		}
		
		OSTimeDlyHMSM ( 0, 0, 0, 100, OS_OPT_TIME_DLY, &err );
		return ret_mode;
	}
	
	return mode;
}


/*
*	函数名：	Touch_Passwd
*	描述：	密码输入模式：触摸坐标处理
*	输入：	mode(使用全局变量sPasswdTmp)
*	输出：	mode
*/
tMode Touch_Passwd ( tMode mode )
{
	if 		( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>110 && tp_dev.y<160 )
		sPasswdTmp += 1;
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>110 && tp_dev.y<160 )
		sPasswdTmp += 2;
	else if ( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>170 && tp_dev.y<220 )
		sPasswdTmp += 3;
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>170 && tp_dev.y<220 )
		sPasswdTmp += 4;
	else if ( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>230 && tp_dev.y<280 )
		return Passwd_Authenticate(  );
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>230 && tp_dev.y<280 )
	{
		BSP_LCD_Fill( 0, 30, 240, 90, WHITE );
		return AUTH_RFID_PASSWD_WHILE;
	}
	
	if ( sPasswdDigits < 4 )
		BSP_LCD_DrawCircle( 50+sPasswdDigits*45, 50, 10 );
		
	sPasswdTmp *= 10;
	sPasswdDigits++;
	
	return AUTH_RFID_PASSWD_CHECK;
}

/*
*	函数名：	Touch_Type
*	描述：	解析Type触摸坐标
*	输入：	无
*	输出：	对应的mode
*/
tMode Touch_Type ( void )
{
	tMode 	mode = TYPE_WHILE;
	sModeBit.Type = 0;
	
	if      ( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>50 && tp_dev.y<100 )
	{
		mode = TYPE_LIGHT;
		sModeBit.Type = 1;
	}
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>50 && tp_dev.y<100 )
	{
		mode = TYPE_FAN;
		sModeBit.Type = 2;
	}
	else if ( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>110 && tp_dev.y<160 )
	{
		mode = TYPE_CURTAIN;
		sModeBit.Type = 3;
	}
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>110 && tp_dev.y<160 )
	{
		mode = TYPE_SOCKET;
		sModeBit.Type = 4;
	}
	else if ( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>170 && tp_dev.y<220 )
	{
		mode = TYPE_ENVIRONMENT;
		sModeBit.Type = 5;
	}
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>170 && tp_dev.y<220 )
	{
		mode = TYPE_FIRE;
		sModeBit.Type = 6;
	}
	else if ( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>230 && tp_dev.y<280 )
	{
		mode = TYPE_SECURITY;
		sModeBit.Type = 7;
	}
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>230 && tp_dev.y<280 )
	{
		mode = TYPE_EXIT;
	}
	
	return mode;
}

/*
*	函数名：	Touch_Terminal
*	描述：	解析Terminal触摸坐标
*	输入：	tMode(系统调度)
*	输出：	tMode(系统调度)
*/
tMode Touch_Terminal ( tMode mode )
{
	sModeBit.Terminal = 0;
	
	if      ( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>50 && tp_dev.y<100 )
	{
		sModeBit.Terminal = 1;
	}
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>50 && tp_dev.y<100 )
	{
		sModeBit.Terminal = 2;
	}
	else if ( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>110 && tp_dev.y<160 )
	{
		sModeBit.Terminal = 3;
	}
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>110 && tp_dev.y<160 )
	{
		sModeBit.Terminal = 4;
	}
	else if ( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>170 && tp_dev.y<220 )
	{
		sModeBit.Terminal = 5;
	}
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>170 && tp_dev.y<220 )
	{
		sModeBit.Terminal = 6;
	}
	else if ( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>230 && tp_dev.y<280 )
	{
		sModeBit.Terminal = 7;
	}
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>230 && tp_dev.y<280 )
		return TYPE_SHOW;
	
	if ( sModeBit.Terminal != 0 )
		return ++mode;
	
	return mode;
}

/*
*		函数名：	Touch_Control
*		描述：		控制部分处理触摸
*		输入：		tMode(系统调度)
*		输出：		tMode(系统调度)
*/
tMode Touch_Control ( tMode mode )
{
	tMsg	    CanCtrlMsg;
    uint8_t		ret;  
	OS_ERR		err;
    uint8_t type        = sModeBit.Type - 1;
    uint8_t terminal    = sModeBit.Terminal - 1;
    
    //增加/开启
	if ( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>170 && tp_dev.y<220 )
	{
        switch ( (uint8_t)mode )
        {
            case TERMINAL_FAN_CTRL:
            case TERMINAL_FAN_CTRL_WHILE:
                        if ( TerminalList[type][terminal].Update_Byte[0] < 100 )
                            TerminalList[type][terminal].Update_Byte[0]++;
                break;
            case TERMINAL_LIGHT_CTRL:
            case TERMINAL_LIGHT_CTRL_WHILE:
            case TERMINAL_CURTAIN_CTRL:
            case TERMINAL_CURTAIN_CTRL_WHILE:
            case TERMINAL_SOCKET_CTRL:
            case TERMINAL_SOCKET_CTRL_WHILE:
                        if ( TerminalList[type][terminal].Update_Byte[0] < 1 )
                            TerminalList[type][terminal].Update_Byte[0] = 1;
                break;
            case TERMINAL_ENVIRONMENT_CTRL:
            case TERMINAL_ENVIRONMENT_CTRL_WHILE:
            case TERMINAL_FIRE_CTRL:
            case TERMINAL_FIRE_CTRL_WHILE:
            case TERMINAL_SECURITY_CTRL:
            case TERMINAL_SECURITY_CTRL_WHILE:
                break;
        }

        BSP_LCD_ShowxNum( 90, 120, TerminalList[type][terminal].Update_Byte[0], 3, 16, 0 );
	}
    
    //减小/关闭
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>170 && tp_dev.y<220 )
	{
        switch ( (uint8_t)mode )
        {
            case TERMINAL_FAN_CTRL:
            case TERMINAL_FAN_CTRL_WHILE:
                        if ( TerminalList[type][terminal].Update_Byte[0] > 0 )
                            TerminalList[type][terminal].Update_Byte[0]--;
                break;
            case TERMINAL_LIGHT_CTRL:
            case TERMINAL_LIGHT_CTRL_WHILE:
            case TERMINAL_CURTAIN_CTRL:
            case TERMINAL_CURTAIN_CTRL_WHILE:
            case TERMINAL_SOCKET_CTRL:
            case TERMINAL_SOCKET_CTRL_WHILE:
                        if ( TerminalList[type][terminal].Update_Byte[0] > 0 )
                            TerminalList[type][terminal].Update_Byte[0] = 0;
                break;
            case TERMINAL_ENVIRONMENT_CTRL:
            case TERMINAL_ENVIRONMENT_CTRL_WHILE:
            case TERMINAL_FIRE_CTRL:
            case TERMINAL_FIRE_CTRL_WHILE:
            case TERMINAL_SECURITY_CTRL:
            case TERMINAL_SECURITY_CTRL_WHILE:
                break;
        }
         
        BSP_LCD_ShowxNum( 90, 120, TerminalList[type][terminal].Update_Byte[0], 3, 16, 0 );
	}
    
    //发送
	else if ( tp_dev.x>20 && tp_dev.x<110 && tp_dev.y>230 && tp_dev.y<280 )
	{
		CanCtrlMsg.Mode             = MODE_CAN_CONTROL_TERMINAL;
		CanCtrlMsg.Control          = CONTROL_TYPE_LIGHT + type;
		CanCtrlMsg.Control_Byte[0]  = TerminalList[type][terminal].Update_Byte[0];
		CanCtrlMsg.Control_Byte[1]  = TerminalList[type][terminal].Update_Byte[1];
		CanCtrlMsg.PriorityID       = TerminalList[type][terminal].PriorityID;

		ret = Can_Send_Msg( (uint8_t *)&CanCtrlMsg );

		if ( !ret )
		{
			BSP_LCD_ShowString( 20, 150, 240, 16, 16, (uint8_t *)"Can Message Send Completed" );
            OSTimeDlyHMSM ( 0, 0, 1, 0, OS_OPT_TIME_DLY, &err );
            BSP_LCD_Fill( 0, 150, 240, 170, WHITE);
		}
		else
		{
			BSP_LCD_ShowString( 20, 150, 240, 16, 16, (uint8_t *)"Can Message Send Failure" );
            OSTimeDlyHMSM ( 0, 0, 1, 0, OS_OPT_TIME_DLY, &err );
            BSP_LCD_Fill( 0, 150, 240, 170, WHITE);
		}
	}
    //返回
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>230 && tp_dev.y<280 )
	{
		return (tMode)(mode/10*10);
	}
    
	OSTimeDlyHMSM ( 0, 0, 0, 100, OS_OPT_TIME_DLY, &err );
	
    return mode;
}

/*
*		函数名：	LCD_Show_Type
*		描述：		显示不同类型
*		输入：		无
*		输出：		无
*/
void LCD_Show_Type ( void )
{
    BSP_LCD_Clear(WHITE);

    for ( uint8_t i=0; i<4; i++ )
    {
        BSP_LCD_Fill( 20,  50+60*i, 110, 100+60*i, LIGHTGREEN );
        BSP_LCD_Fill( 130, 50+60*i, 220, 100+60*i, LIGHTGREEN );
        
        BSP_LCD_DrawString_CH( 50,  70+60*i, stypeName[2*i], 2 );
        BSP_LCD_DrawString_CH( 160, 70+60*i, stypeName[2*i+1], 2 );
    }
}

/*
*		函数名：	LCD_Show_Terminal
*		描述：		显示不同类型终端的编号选择
*		输入：		tMode(系统调度)
*		输出：		无
*/
void LCD_Show_Terminal ( tMode mode )
{
    BSP_LCD_Clear(WHITE);

    BSP_LCD_DrawString_CH( 105, 30, stypeName[ sModeBit.Type -1 ], 2 );

    for ( uint8_t i=0; i<4; i++ )
    {
            BSP_LCD_Fill( 20,  50+60*i, 110, 100+60*i, LIGHTGREEN );
            BSP_LCD_Fill( 130, 50+60*i, 220, 100+60*i, LIGHTGREEN );
    }

    uint8_t num = 1;
    for ( uint8_t i=0; i<3; i++ )
    {
            BSP_LCD_ShowxNum( 60,  70+60*i, num++, 1, 16, 1 );
            BSP_LCD_ShowxNum( 170, 70+60*i, num++, 1, 16, 1 );
    }

    BSP_LCD_ShowxNum( 60, 250, 7, 1, 16, 1 );
    BSP_LCD_DrawString_CH( 160, 250, stypeName[ 7 ], 2 );
}


/*
*		函数名：	LCD_Show_Control
*		描述：		显示不同终端的控制
*		输入：		tMode(系统调度)
*		输出：		无
*/
void LCD_Show_Control ( tMode mode )
{
	BSP_LCD_Clear(WHITE);
    
    uint8_t type        = sModeBit.Type -1;
    uint8_t terminal    = sModeBit.Terminal -1;

    BSP_LCD_DrawString_CH( 70, 40, stypeName[ sModeBit.Type -1 ], 2 );
	BSP_LCD_ShowxNum( 160, 40, sModeBit.Terminal, 1, 16, 1 );
    
    switch ( TerminalList[type][terminal].Status )
    {
        case DECVICE_NONE:      BSP_LCD_DrawString_CH( 80, 60, "没有该设备", 5 );
                            break;
        case DECVICE_NORMAL:    BSP_LCD_DrawString_CH( 100, 60, "正常", 2 );
                            break;
        default:                BSP_LCD_DrawString_CH( 90, 60, "失败次数", 4 );
                                BSP_LCD_ShowxNum( 90, 80, TerminalList[type][terminal].Status-1, 3, 16, 1 );
                            break;
    }

	BSP_LCD_DrawString_CH( 20, 80, "厂商", 2);
	BSP_LCD_ShowxNum( 90, 80, TerminalList[type][terminal].FactoryID, 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 130, 80, "型号", 2 );
	BSP_LCD_ShowxNum( 200, 80, TerminalList[type][terminal].ModelID, 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 20, 100, "优先级", 3 );
	BSP_LCD_ShowxNum( 130, 100, TerminalList[type][terminal].PriorityID, 10, 16, 1 );
    
    //数据
    BSP_LCD_DrawString_CH( 20, 120, "数据一", 3 );
	BSP_LCD_ShowxNum( 90, 120, TerminalList[type][terminal].Update_Byte[0], 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 130, 120, "数据二", 3 );
	BSP_LCD_ShowxNum( 200, 120, TerminalList[type][terminal].Update_Byte[1], 3, 16, 1 );

	//按键
	BSP_LCD_Fill( 20, 170, 110, 220, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 50, 190, "开启", 2 );				//On

	BSP_LCD_Fill( 130, 170, 220, 220, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 160, 190, "关闭", 2 );			//Off

	BSP_LCD_Fill( 20, 230, 110, 280, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 50, 250, "发送", 2 );		//Confirm

	BSP_LCD_Fill( 130, 230, 220, 280, LIGHTGREEN );
    BSP_LCD_DrawString_CH( 160, 250, stypeName[ 7 ], 2 );
}

/*
*		函数名：	LCD_Show_Control_UpDown
*		描述：		显示不同终端的控制(带百分比控制)
*		输入：		tMode(系统调度)
*		输出：		无
*/
void LCD_Show_Control_UpDown ( tMode mode )
{
	BSP_LCD_Clear(WHITE);
    uint8_t type        = sModeBit.Type -1;
    uint8_t terminal    = sModeBit.Terminal -1;
    
    switch ( TerminalList[type][terminal].Status )
    {
        case DECVICE_NONE:      BSP_LCD_DrawString_CH( 80, 60, "没有该设备", 5 );
                            break;
        case DECVICE_NORMAL:    BSP_LCD_DrawString_CH( 100, 60, "正常", 2 );
                            break;
        default:                BSP_LCD_DrawString_CH( 90, 60, "失败次数", 4 );
                                BSP_LCD_ShowxNum( 90, 80, TerminalList[type][terminal].Status-1, 3, 16, 1 );
                            break;
    }

    BSP_LCD_DrawString_CH( 70, 40, stypeName[ sModeBit.Type -1 ], 2 );
	BSP_LCD_ShowxNum( 160, 40, sModeBit.Terminal, 1, 16, 1 );

	BSP_LCD_DrawString_CH( 20, 80, "厂商", 2);
	BSP_LCD_ShowxNum( 90, 80, TerminalList[type][terminal].FactoryID, 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 130, 80, "型号", 2 );
	BSP_LCD_ShowxNum( 200, 80, TerminalList[type][terminal].ModelID, 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 20, 100, "优先级", 3 );
	BSP_LCD_ShowxNum( 130, 100, TerminalList[type][terminal].PriorityID, 10, 16, 1 );
	  
    //数据
    BSP_LCD_DrawString_CH( 20, 120, "数据一", 3 );
	BSP_LCD_ShowxNum( 90, 120, TerminalList[type][terminal].Update_Byte[0], 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 130, 120, "数据二", 3 );
	BSP_LCD_ShowxNum( 200, 120, TerminalList[type][terminal].Update_Byte[1], 3, 16, 1 );
    

	//按键
	BSP_LCD_Fill( 20, 170, 110, 220, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 50, 190, "增加", 2 );				//Up

	BSP_LCD_Fill( 130, 170, 220, 220, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 160, 190, "减少", 2 );			//Down

	BSP_LCD_Fill( 20, 230, 110, 280, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 50, 250, "发送", 2 );		//Confirm

	BSP_LCD_Fill( 130, 230, 220, 280, LIGHTGREEN );
    BSP_LCD_DrawString_CH( 160, 250, stypeName[ 7 ], 2 );
}

/*
*		函数名：	LCD_Show_Control_Data
*		描述：		显示不同终端的控制(只显示数据)
*		输入：		tMode(系统调度)
*		输出：		无
*/
void LCD_Show_Control_Data ( tMode mode )
{
	BSP_LCD_Clear(WHITE);
    uint8_t type        = sModeBit.Type -1;
    uint8_t terminal    = sModeBit.Terminal -1;
    
    switch ( TerminalList[type][terminal].Status )
    {
        case DECVICE_NONE:      BSP_LCD_DrawString_CH( 80, 60, "没有该设备", 5 );
                            break;
        case DECVICE_NORMAL:    BSP_LCD_DrawString_CH( 100, 60, "正常", 2 );
                            break;
        default:                BSP_LCD_DrawString_CH( 90, 60, "失败次数", 4 );
                                BSP_LCD_ShowxNum( 90, 80, TerminalList[type][terminal].Status-1, 3, 16, 1 );
                            break;
    }

    BSP_LCD_DrawString_CH( 70, 40, stypeName[ sModeBit.Type -1 ], 2 );
	BSP_LCD_ShowxNum( 160, 40, sModeBit.Terminal, 1, 16, 1 );

	BSP_LCD_DrawString_CH( 20, 80, "厂商", 2);
	BSP_LCD_ShowxNum( 90, 80, TerminalList[type][terminal].FactoryID, 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 130, 80, "型号", 2 );
	BSP_LCD_ShowxNum( 200, 80, TerminalList[type][terminal].ModelID, 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 20, 100, "优先级", 3 );
	BSP_LCD_ShowxNum( 130, 100, TerminalList[type][terminal].PriorityID, 10, 16, 1 );
	
    
    //数据
    BSP_LCD_DrawString_CH( 20, 120, "数据一", 3 );
	BSP_LCD_ShowxNum( 90, 120, TerminalList[type][terminal].Update_Byte[0], 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 130, 120, "数据二", 3 );
	BSP_LCD_ShowxNum( 200, 120, TerminalList[type][terminal].Update_Byte[1], 3, 16, 1 );
    

	BSP_LCD_Fill( 20, 230, 110, 280, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 50, 250, "更新", 2 );

	BSP_LCD_Fill( 130, 230, 220, 280, LIGHTGREEN );
    BSP_LCD_DrawString_CH( 160, 250, stypeName[ 7 ], 2 );
}

