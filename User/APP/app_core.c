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
static char stypeName[8][4]={"����","����","����","����","����","����","����","����"};

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
			/****************************************��֤****************************************/
			case	AUTH_SHOW:						LCD_Show_Authenticate(  );		//��ʾ��ʼ����
			case	AUTH_RFID_PASSWD_SHOW:			LCD_Show_Passwd(  );			//��ʾ��֤���� 
			case	AUTH_RFID_PASSWD_WHILE:			mode = AUTH_RFID_PASSWD_WHILE;
                                                    sPasswdTmp = 0;					//��ʱ�������
													sPasswdDigits = 0;				//�Ѿ����������λ��
			case	AUTH_RFID_PASSWD_CHECK:			mode = Authenticate( mode );	//ɨ�败����RFID��,����֤
												break;
			case	AUTH_SUCCESS:					mode = TYPE_SHOW;				//�ɹ���ת����һ��
												break;
			case	AUTH_FAILURE:					mode = AUTH_RFID_PASSWD_WHILE;	//���س�ʼ����
												break;
			/****************************************End****************************************/
			

			/****************************ѡ������豸����,��ת����һ��****************************/
			case	TYPE_SHOW:						LCD_Show_Type(  );				//��ʾ�����豸����ѡ�����
			case	TYPE_WHILE:						mode = TYPE_WHILE;
													mode = LCD_Touch( mode );		//ѭ����ȡ��������
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
			case	TYPE_EXIT:						mode = AUTH_SHOW;				//������һ��
												break;
			/****************************************End****************************************/
			
			
			/******************************ѡ���������,��ת����һ��******************************/
			case	TERMINAL_LIGHT_SHOW:			LCD_Show_Terminal( mode++ );	//��ʾ�ն�����ѡ�����
			case	TERMINAL_LIGHT_WHILE:			mode = LCD_Touch( mode );		//ѭ����ȡ��������
												break;
			case	TERMINAL_LIGHT_CTRL:			LCD_Show_Control( mode++ );
			case	TERMINAL_LIGHT_CTRL_WHILE:		mode = LCD_Touch( mode );		//ѭ����ȡ��������
												break;
			case	TERMINAL_LIGHT_EXIT:			mode = TERMINAL_LIGHT_SHOW;		//������һ��
												break;
			/****************************************End****************************************/
			
			
			/******************************ѡ����ȱ��,��ת����һ��******************************/
			case	TERMINAL_FAN_SHOW:				LCD_Show_Terminal( mode++ );	//��ʾ�ն�����ѡ�����
			case	TERMINAL_FAN_WHILE:				mode = LCD_Touch( mode );		//ѭ����ȡ��������
												break;		
			case	TERMINAL_FAN_CTRL:				LCD_Show_Control_UpDown( mode++ );
			case	TERMINAL_FAN_CTRL_WHILE:		mode = LCD_Touch( mode );		//ѭ����ȡ��������
												break;
			case	TERMINAL_FAN_EXIT:				mode = TERMINAL_FAN_SHOW;		//������һ��
												break;
			/****************************************End****************************************/
			
			
			/******************************ѡ�������,��ת����һ��******************************/
			case	TERMINAL_CURTAIN_SHOW:          LCD_Show_Terminal( mode++ );    //��ʾ�ն�����ѡ�����
			case	TERMINAL_CURTAIN_WHILE:         mode = LCD_Touch( mode );       //ѭ����ȡ��������
												break;	
			case	TERMINAL_CURTAIN_CTRL:			LCD_Show_Control( mode++ );
			case	TERMINAL_CURTAIN_CTRL_WHILE:    mode = LCD_Touch( mode );       //ѭ����ȡ��������
												break;
			case	TERMINAL_CURTAIN_EXIT:		    mode = TERMINAL_CURTAIN_SHOW;   //������һ��
												break;
			/****************************************End****************************************/
			
			
			/******************************ѡ��������,��ת����һ��******************************/
			case	TERMINAL_SOCKET_SHOW:           LCD_Show_Terminal( mode++ );	//��ʾ�ն�����ѡ�����
			case	TERMINAL_SOCKET_WHILE:			mode = LCD_Touch( mode );	    //ѭ����ȡ��������
												break;	
			case	TERMINAL_SOCKET_CTRL:			LCD_Show_Control( mode++ );
			case	TERMINAL_SOCKET_CTRL_WHILE:		mode = LCD_Touch( mode );       //ѭ����ȡ��������
												break;
			case	TERMINAL_SOCKET_EXIT:			mode = TERMINAL_CURTAIN_SHOW;	//������һ��
												break;
			/****************************************End****************************************/
			
			
			/******************************ѡ�񻷾����,��ת����һ��******************************/
			case	TERMINAL_ENVIRONMENT_SHOW:	    LCD_Show_Terminal( mode++ );    //��ʾ�ն�����ѡ�����
			case	TERMINAL_ENVIRONMENT_WHILE:	    mode = LCD_Touch( mode );       //ѭ����ȡ��������
												break;	
			case	TERMINAL_ENVIRONMENT_CTRL:		LCD_Show_Control_Data( mode++ );
			case	TERMINAL_ENVIRONMENT_CTRL_WHILE:mode = LCD_Touch( mode );       //ѭ����ȡ��������
												break;
			case	TERMINAL_ENVIRONMENT_EXIT:		mode = TERMINAL_CURTAIN_SHOW;   //������һ��
												break;
			/****************************************End****************************************/
			
			
			/******************************ѡ����ֱ��,��ת����һ��******************************/
			case	TERMINAL_FIRE_SHOW:             LCD_Show_Terminal( mode++ );    //��ʾ�ն�����ѡ�����
			case	TERMINAL_FIRE_WHILE:            mode = LCD_Touch( mode );       //ѭ����ȡ��������
                                                break;	
			case	TERMINAL_FIRE_CTRL:             LCD_Show_Control_Data( mode++ );
			case	TERMINAL_FIRE_CTRL_WHILE:       mode = LCD_Touch( mode );       //ѭ����ȡ��������
                                                break;
			case	TERMINAL_FIRE_EXIT:             mode = TERMINAL_CURTAIN_SHOW;   //������һ��
                                                break;
			/****************************************End****************************************/
			
			
			/******************************ѡ�񰲷����,��ת����һ��******************************/
			case	TERMINAL_SECURITY_SHOW:         LCD_Show_Terminal( mode++ );    //��ʾ�ն�����ѡ�����
			case	TERMINAL_SECURITY_WHILE:        mode = LCD_Touch( mode );       //ѭ����ȡ��������
                                                break;	
			case	TERMINAL_SECURITY_CTRL:         LCD_Show_Control_Data( mode++ );
			case	TERMINAL_SECURITY_CTRL_WHILE:   mode = LCD_Touch( mode );       //ѭ����ȡ��������
                                                break;
			case	TERMINAL_SECURITY_EXIT:         mode = TERMINAL_CURTAIN_SHOW;   //������һ��
                                                break;
			/****************************************End****************************************/
			
			case	CONTROL_SUCCESS:
			case	CONTROL_FAILURE:                mode = TYPE_SHOW;
                                                break;
            default:                                mode = AUTH_SHOW;
                                                break;
		}
		
		//ѭ����ʾ����
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
*		��������	Authenticate_Config
*		������		��ʼ��RFID+Passwd
*		���룺		��
*		�����		��
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
*	��������	LCD_Show_Authenticate
*	������	��ʾ��֤����( RFID + PASSWD )
*	���룺	��
*	�����	��
*/
void LCD_Show_Authenticate ( void )
{
	OS_ERR      err;
	
	//LCD��ʾ
	BSP_LCD_Clear(WHITE);
	BSP_LCD_ShowString(40,60,200,16,16,(uint8_t *)"Please Authenticate");
	
	//��ʾͼƬ
	BSP_LCD_DrawPicture(0,100,240,133,(uint8_t *)Picture_Authentication);
	
	//��֤��ʽ��ʾ
	BSP_LCD_ShowString(20,240,200,16,16,(uint8_t *)"Please enter your password");
	BSP_LCD_ShowString(20,280,200,16,16,(uint8_t *)"Or swipe your RFID card");
	
	BSP_LCD_ShowString(80,300,200,16,16,(uint8_t *)VersionID);
	
	//��ȡEEPROM,��ʼ������
	BSP_LCD_ShowStringNotStack(20,260,200,16,16,(uint8_t *)"Initializing from EEPROM");

	Authenticate_Config(  );
	BSP_LCD_ShowStringNotStack(20,260,200,16,16,(uint8_t *)"       Initialized      ");
	
	OSTimeDlyHMSM ( 0, 0, 3, 0, OS_OPT_TIME_DLY, &err );
}

/*
*	��������	LCD_Show_Passwd
*	������	��ʾ�����������,����ʾ����ˢ��
*	���룺	��
*	�����	��
*/
void LCD_Show_Passwd( void )
{
	BSP_LCD_Clear(WHITE);
	
	//��ʾ��ɫ��
	for ( uint8_t i=0; i<3; i++ )
	{
		BSP_LCD_Fill( 20,  110+60*i, 110, 160+60*i, LIGHTGREEN );
		BSP_LCD_Fill( 130, 110+60*i, 220, 160+60*i, LIGHTGREEN );
	}
	
	//��ʾ����
	uint8_t num = 1;
	for ( uint8_t i=1; i<3; i++ )
	{
		BSP_LCD_ShowxNum( 60,  70+60*i, num++, 1, 16, 1 );
        BSP_LCD_ShowxNum( 170, 70+60*i, num++, 1, 16, 1 );
	}
	
	//��ʾȷ�ϡ�ɾ��
    BSP_LCD_DrawString_CH(50,   250, "ȷ��", 2);
    BSP_LCD_DrawString_CH(160,  250, "ɾ��", 2);
      
	BSP_LCD_ShowString( 30, 300, 200, 16, 16, (uint8_t *)"You can also swipe cards" );
}

/*
*	��������	Authenticate
*	������	��֤( RFID + Passwd )
*	���룺	mode
*	�����	ret_mode
*/
tMode Authenticate( tMode mode )
{
	//ɨ��RFID��,������ʶ��
	if ( RFID_Authenticate(  ) == AUTH_SUCCESS )
		return AUTH_SUCCESS;
    
	//ɨ����Ļ����
	return LCD_Touch ( mode );
}

/*
*	��������	RFID_Authenticate
*	������	ɨ��RFID����������ʶ����֤
*	���룺	��
*	�����	mode(ʶ��ɹ���ʶ��ʧ��)
*/
tMode RFID_Authenticate( void )
{
	uint8_t ret;						//����״̬
	uint8_t	RFID_Tmp[4];
	
    memset( RFID_Tmp, 0 ,sizeof( RFID_Tmp ) );
    sRfidCardIDTmp = 0;
    
	//Ѱ��
	if ( ( ret = PcdRequest ( PICC_REQALL, RFID_Tmp ) ) != MI_OK )                                   
		ret = PcdRequest ( PICC_REQALL, RFID_Tmp );	//��ʧ���ٴ�Ѱ��	   
	if ( ret == MI_OK  )
	{
		//����ײ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����
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
*	��������	RFID_ID_Match
*	������	ƥ��RFID�����к�
*	���룺	��(ʹ��ȫ�ֱ���RFID_Tmp��RFID_Card)
*	�����	mode(ʶ��ɹ���ʶ��ʧ��)
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
*	��������	Passwd_Authenticate
*	������	������֤
*	���룺	��(ʹ��ȫ�ֱ���Passwd��Passwd_Tmp)
*	�����	mode
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
*	��������	LCD_Touch
*	������	��ȡ�������꣬������ģʽ��������Ӧ�Ĵ�����
*	���룺	mode
*	�����	mode
*/
tMode LCD_Touch ( tMode mode )
{
	OS_ERR 	err;
	tMode		ret_mode;
    
	//ɨ�败����
	tp_dev.scan(0);
	
	//������������
	if( tp_dev.sta & TP_PRES_DOWN )
	{
		//���������Ƿ�����Ļ��Χ��
		if( tp_dev.x < gLcdDevice.width && tp_dev.y < gLcdDevice.height )
		{
			#ifdef DEBUG
				printf( "Touch X=%d Y=%d\n\r", tp_dev.x, tp_dev.y );
				printf( "Touch mode=%d\n", mode );
			#endif
			
			//������Ӧ�Ĵ�����
			if			( mode == AUTH_RFID_PASSWD_WHILE || mode == AUTH_RFID_PASSWD_CHECK )				//��֤
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
*	��������	Touch_Passwd
*	������	��������ģʽ���������괦��
*	���룺	mode(ʹ��ȫ�ֱ���sPasswdTmp)
*	�����	mode
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
*	��������	Touch_Type
*	������	����Type��������
*	���룺	��
*	�����	��Ӧ��mode
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
*	��������	Touch_Terminal
*	������	����Terminal��������
*	���룺	tMode(ϵͳ����)
*	�����	tMode(ϵͳ����)
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
*		��������	Touch_Control
*		������		���Ʋ��ִ�����
*		���룺		tMode(ϵͳ����)
*		�����		tMode(ϵͳ����)
*/
tMode Touch_Control ( tMode mode )
{
	tMsg	    CanCtrlMsg;
    uint8_t		ret;  
	OS_ERR		err;
    uint8_t type        = sModeBit.Type - 1;
    uint8_t terminal    = sModeBit.Terminal - 1;
    
    //����/����
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
    
    //��С/�ر�
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
    
    //����
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
    //����
	else if ( tp_dev.x>130 && tp_dev.x<220 && tp_dev.y>230 && tp_dev.y<280 )
	{
		return (tMode)(mode/10*10);
	}
    
	OSTimeDlyHMSM ( 0, 0, 0, 100, OS_OPT_TIME_DLY, &err );
	
    return mode;
}

/*
*		��������	LCD_Show_Type
*		������		��ʾ��ͬ����
*		���룺		��
*		�����		��
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
*		��������	LCD_Show_Terminal
*		������		��ʾ��ͬ�����ն˵ı��ѡ��
*		���룺		tMode(ϵͳ����)
*		�����		��
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
*		��������	LCD_Show_Control
*		������		��ʾ��ͬ�ն˵Ŀ���
*		���룺		tMode(ϵͳ����)
*		�����		��
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
        case DECVICE_NONE:      BSP_LCD_DrawString_CH( 80, 60, "û�и��豸", 5 );
                            break;
        case DECVICE_NORMAL:    BSP_LCD_DrawString_CH( 100, 60, "����", 2 );
                            break;
        default:                BSP_LCD_DrawString_CH( 90, 60, "ʧ�ܴ���", 4 );
                                BSP_LCD_ShowxNum( 90, 80, TerminalList[type][terminal].Status-1, 3, 16, 1 );
                            break;
    }

	BSP_LCD_DrawString_CH( 20, 80, "����", 2);
	BSP_LCD_ShowxNum( 90, 80, TerminalList[type][terminal].FactoryID, 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 130, 80, "�ͺ�", 2 );
	BSP_LCD_ShowxNum( 200, 80, TerminalList[type][terminal].ModelID, 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 20, 100, "���ȼ�", 3 );
	BSP_LCD_ShowxNum( 130, 100, TerminalList[type][terminal].PriorityID, 10, 16, 1 );
    
    //����
    BSP_LCD_DrawString_CH( 20, 120, "����һ", 3 );
	BSP_LCD_ShowxNum( 90, 120, TerminalList[type][terminal].Update_Byte[0], 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 130, 120, "���ݶ�", 3 );
	BSP_LCD_ShowxNum( 200, 120, TerminalList[type][terminal].Update_Byte[1], 3, 16, 1 );

	//����
	BSP_LCD_Fill( 20, 170, 110, 220, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 50, 190, "����", 2 );				//On

	BSP_LCD_Fill( 130, 170, 220, 220, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 160, 190, "�ر�", 2 );			//Off

	BSP_LCD_Fill( 20, 230, 110, 280, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 50, 250, "����", 2 );		//Confirm

	BSP_LCD_Fill( 130, 230, 220, 280, LIGHTGREEN );
    BSP_LCD_DrawString_CH( 160, 250, stypeName[ 7 ], 2 );
}

/*
*		��������	LCD_Show_Control_UpDown
*		������		��ʾ��ͬ�ն˵Ŀ���(���ٷֱȿ���)
*		���룺		tMode(ϵͳ����)
*		�����		��
*/
void LCD_Show_Control_UpDown ( tMode mode )
{
	BSP_LCD_Clear(WHITE);
    uint8_t type        = sModeBit.Type -1;
    uint8_t terminal    = sModeBit.Terminal -1;
    
    switch ( TerminalList[type][terminal].Status )
    {
        case DECVICE_NONE:      BSP_LCD_DrawString_CH( 80, 60, "û�и��豸", 5 );
                            break;
        case DECVICE_NORMAL:    BSP_LCD_DrawString_CH( 100, 60, "����", 2 );
                            break;
        default:                BSP_LCD_DrawString_CH( 90, 60, "ʧ�ܴ���", 4 );
                                BSP_LCD_ShowxNum( 90, 80, TerminalList[type][terminal].Status-1, 3, 16, 1 );
                            break;
    }

    BSP_LCD_DrawString_CH( 70, 40, stypeName[ sModeBit.Type -1 ], 2 );
	BSP_LCD_ShowxNum( 160, 40, sModeBit.Terminal, 1, 16, 1 );

	BSP_LCD_DrawString_CH( 20, 80, "����", 2);
	BSP_LCD_ShowxNum( 90, 80, TerminalList[type][terminal].FactoryID, 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 130, 80, "�ͺ�", 2 );
	BSP_LCD_ShowxNum( 200, 80, TerminalList[type][terminal].ModelID, 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 20, 100, "���ȼ�", 3 );
	BSP_LCD_ShowxNum( 130, 100, TerminalList[type][terminal].PriorityID, 10, 16, 1 );
	  
    //����
    BSP_LCD_DrawString_CH( 20, 120, "����һ", 3 );
	BSP_LCD_ShowxNum( 90, 120, TerminalList[type][terminal].Update_Byte[0], 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 130, 120, "���ݶ�", 3 );
	BSP_LCD_ShowxNum( 200, 120, TerminalList[type][terminal].Update_Byte[1], 3, 16, 1 );
    

	//����
	BSP_LCD_Fill( 20, 170, 110, 220, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 50, 190, "����", 2 );				//Up

	BSP_LCD_Fill( 130, 170, 220, 220, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 160, 190, "����", 2 );			//Down

	BSP_LCD_Fill( 20, 230, 110, 280, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 50, 250, "����", 2 );		//Confirm

	BSP_LCD_Fill( 130, 230, 220, 280, LIGHTGREEN );
    BSP_LCD_DrawString_CH( 160, 250, stypeName[ 7 ], 2 );
}

/*
*		��������	LCD_Show_Control_Data
*		������		��ʾ��ͬ�ն˵Ŀ���(ֻ��ʾ����)
*		���룺		tMode(ϵͳ����)
*		�����		��
*/
void LCD_Show_Control_Data ( tMode mode )
{
	BSP_LCD_Clear(WHITE);
    uint8_t type        = sModeBit.Type -1;
    uint8_t terminal    = sModeBit.Terminal -1;
    
    switch ( TerminalList[type][terminal].Status )
    {
        case DECVICE_NONE:      BSP_LCD_DrawString_CH( 80, 60, "û�и��豸", 5 );
                            break;
        case DECVICE_NORMAL:    BSP_LCD_DrawString_CH( 100, 60, "����", 2 );
                            break;
        default:                BSP_LCD_DrawString_CH( 90, 60, "ʧ�ܴ���", 4 );
                                BSP_LCD_ShowxNum( 90, 80, TerminalList[type][terminal].Status-1, 3, 16, 1 );
                            break;
    }

    BSP_LCD_DrawString_CH( 70, 40, stypeName[ sModeBit.Type -1 ], 2 );
	BSP_LCD_ShowxNum( 160, 40, sModeBit.Terminal, 1, 16, 1 );

	BSP_LCD_DrawString_CH( 20, 80, "����", 2);
	BSP_LCD_ShowxNum( 90, 80, TerminalList[type][terminal].FactoryID, 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 130, 80, "�ͺ�", 2 );
	BSP_LCD_ShowxNum( 200, 80, TerminalList[type][terminal].ModelID, 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 20, 100, "���ȼ�", 3 );
	BSP_LCD_ShowxNum( 130, 100, TerminalList[type][terminal].PriorityID, 10, 16, 1 );
	
    
    //����
    BSP_LCD_DrawString_CH( 20, 120, "����һ", 3 );
	BSP_LCD_ShowxNum( 90, 120, TerminalList[type][terminal].Update_Byte[0], 3, 16, 1 );
	
	BSP_LCD_DrawString_CH( 130, 120, "���ݶ�", 3 );
	BSP_LCD_ShowxNum( 200, 120, TerminalList[type][terminal].Update_Byte[1], 3, 16, 1 );
    

	BSP_LCD_Fill( 20, 230, 110, 280, LIGHTGREEN );
	BSP_LCD_DrawString_CH( 50, 250, "����", 2 );

	BSP_LCD_Fill( 130, 230, 220, 280, LIGHTGREEN );
    BSP_LCD_DrawString_CH( 160, 250, stypeName[ 7 ], 2 );
}

