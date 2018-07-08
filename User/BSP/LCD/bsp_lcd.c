#include "stm32f10x_fsmc.h"

#include "./USART/bsp_usart.h"	 
#include "./DELAY/bsp_delay.h"
#include "./FLASH/bsp_flash.h"
#include "./LCD/bsp_lcd.h"

#include "stdio.h"
#include "string.h"
#include "font.h"
				 
//LCD�Ļ�����ɫ�ͱ���ɫ	   
uint16_t POINT_COLOR    =   0x0000;     //������ɫ
uint16_t BACK_COLOR     =   0xFFFF;     //����ɫ 

/* ����LCD��Ҫ���� */
tLcdDevice gLcdDevice;  //Ĭ��Ϊ����

//����״̬
//static tLcdMode slcdMode = SYSTEM_NONE;
//static tLcdFontMode sLcdFontMode = EXTERN_FLASH;
//static uint8_t	sFontAddr = NULL;

//��̬����
static void LCD_WR_REG(uint16_t regval);
static void LCD_WR_DATA(uint16_t data);
static uint16_t LCD_RD_DATA(void);
static void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
static uint16_t LCD_ReadReg(uint8_t LCD_Reg);
static void LCD_WriteRAM_Prepare(void);
static void LCD_WriteRAM(uint16_t RGB_Code);
static void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
static void GetFont_from_EXFlash( uint8_t * pBuffer, uint16_t pCharCH );

extern const uint8_t gb2312[ ];

/*
*	��������LCD_WR_REG
*	������		д�Ĵ�������
*	���룺		regval:�Ĵ���ֵ
*	�����
*/
static void LCD_WR_REG(uint16_t regval)
{
	LCD->LCD_REG=regval;//д��Ҫд�ļĴ������
}

/*
*	��������LCD_WR_DATA
*	������		дLCD����
*	���룺		data:Ҫд���ֵ
*	�����
*/
static void LCD_WR_DATA(uint16_t data)
{
	LCD->LCD_RAM=data;
}

/*
*	��������LCD_RD_DATA
*	������		��LCD����
*	���룺		
*	�����		��ȡ����ֵ
*/
static uint16_t LCD_RD_DATA(void)
{
	return LCD->LCD_RAM;
}


/*
*	��������LCD_WriteReg
*	������		д�Ĵ���
*	���룺		LCD_Reg:�Ĵ�����ַ
*			LCD_RegValue:Ҫд�������
*	�����		
*/
static void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;		//д��Ҫд�ļĴ������	 
	LCD->LCD_RAM = LCD_RegValue;//д������	    		 
}

/*
*	��������LCD_ReadReg
*	������		���Ĵ���
*	���룺		LCD_Reg:�Ĵ�����ַ
*	�����		����������
*/
static uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
	LCD_WR_REG(LCD_Reg);		//д��Ҫ���ļĴ������
	delay_us(5);
	return LCD_RD_DATA();		//���ض�����ֵ
}


/*
*	��������LCD_WriteRAM_Prepare
*	������		��ʼдGRAM
*	���룺		
*	�����		
*/
static void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=gLcdDevice.wramcmd;	  
}


/*
*	��������LCD_WriteRAM
*	������		дGRAM��ɫ
*	���룺		RGB_Code:��ɫֵ
*	�����		
*/
static void LCD_WriteRAM(uint16_t RGB_Code)
{							    
	LCD->LCD_RAM = RGB_Code;//дʮ��λGRAM
}

/*
*	��������LCD_SetCursor
*	������		���ù��λ��
*	���룺		Xpos,Ypos����
*	�����
*/
static void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	 
	if(gLcdDevice.dir==1)
		Xpos=gLcdDevice.width-1-Xpos;			//������ʵ���ǵ�תx,y����
	LCD_WriteReg(gLcdDevice.setxcmd, Xpos);
	LCD_WriteReg(gLcdDevice.setycmd, Ypos); 
}

/*
*	��������BSP_LCD_DisplayOn
*	������		LCD������ʾ
*	���룺		
*	�����		
*/
void BSP_LCD_DisplayOn(void)
{					   
	LCD_WriteReg(R7,0x0173); 				//������ʾ
} 


/*
*	��������	BSP_LCD_DisplayOff
*	������		LCD�ر���ʾ
*	���룺		
*	�����		
*/
void BSP_LCD_DisplayOff(void)
{	   
	LCD_WriteReg(R7,0x0);					//�ر���ʾ 
}

/*
*	��������BSP_LCD_Scan_Dir
*	������		����LCD���Զ�ɨ�跽��
*	���룺		dir:0~7,����8������(���嶨���lcd.h)
*	�����
*/
void BSP_LCD_Scan_Dir(uint8_t dir)
{
	uint16_t	regval = 0;
	uint8_t		dirreg = 0;
	if(gLcdDevice.dir==1 )
	{			   
		switch(dir)//����ת��
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	}

	switch(dir)
	{
		case L2R_U2D://������,���ϵ���
			regval|=(1<<5)|(1<<4)|(0<<3); 
			break;
		case L2R_D2U://������,���µ���
			regval|=(0<<5)|(1<<4)|(0<<3); 
			break;
		case R2L_U2D://���ҵ���,���ϵ���
			regval|=(1<<5)|(0<<4)|(0<<3);
			break;
		case R2L_D2U://���ҵ���,���µ���
			regval|=(0<<5)|(0<<4)|(0<<3); 
			break;	 
		case U2D_L2R://���ϵ���,������
			regval|=(1<<5)|(1<<4)|(1<<3); 
			break;
		case U2D_R2L://���ϵ���,���ҵ���
			regval|=(1<<5)|(0<<4)|(1<<3); 
			break;
		case D2U_L2R://���µ���,������
			regval|=(0<<5)|(1<<4)|(1<<3); 
			break;
		case D2U_R2L://���µ���,���ҵ���
			regval|=(0<<5)|(0<<4)|(1<<3); 
			break;	 
	}

	dirreg=0x03;
	regval|=1<<12;
	
	LCD_WriteReg(dirreg,regval);
	
}

/*
*	��������BSP_LCD_Fast_DrawPoint
*	������		���ٻ���
*	���룺		x,y:����;color:�˵����ɫ
*	�����
*/
void BSP_LCD_Fast_DrawPoint(uint16_t x, uint16_t y)
{
    if(gLcdDevice.dir==1)
        x=gLcdDevice.width-1-x;//������ʵ���ǵ�תx,y����
    LCD_WriteReg(gLcdDevice.setxcmd,x);
    LCD_WriteReg(gLcdDevice.setycmd,y);
		 
	LCD->LCD_REG=gLcdDevice.wramcmd; 
	LCD->LCD_RAM=POINT_COLOR; 
}	 


//����
//x1,y1:�������
//x2,y2:�յ�����
void BSP_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		BSP_LCD_Fast_DrawPoint(uRow,uCol);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    


/*
*	��������	BSP_LCD_Display_Dir
*	������		����LCD��ʾ����
*	���룺		dir:0,������1,����
*	�����		
*/
void BSP_LCD_Display_Dir(uint8_t dir)
{
	if(dir==0)//����
	{
		gLcdDevice.dir      = 0;
		gLcdDevice.width    = 240;
		gLcdDevice.height   = 320;
		
        gLcdDevice.wramcmd  = R34;
        gLcdDevice.setxcmd  = R32;
        gLcdDevice.setycmd  = R33;  

	}
    else
	{	  
		gLcdDevice.dir      = 1;
		gLcdDevice.width    = 320;
		gLcdDevice.height   = 240;
		
        gLcdDevice.wramcmd  = R34;
        gLcdDevice.setxcmd  = R33;
        gLcdDevice.setycmd  = R32;  
	} 
	BSP_LCD_Scan_Dir(DFT_SCAN_DIR);	//Ĭ��ɨ�跽��
}

/*
*	��������	BSP_LCD_Init
*	������		��ʼ��LCD
*	���룺		
*	�����		
*/
void BSP_LCD_Init(void)
{ 										  
	GPIO_InitTypeDef GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);	//ʹ��FSMCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO,ENABLE);//ʹ��PORTB,D,E,G�Լ�AFIO���ù���ʱ��

 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //PB0 ������� ����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);

 	//PORTD�����������  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;				 //	//PORTD�����������  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�����������   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	  
	//PORTE�����������  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //	//PORTD�����������  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�����������   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure); 
	  
   	//	//PORTG12����������� A0	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	 //	//PORTD�����������  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�����������   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOG, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	 //	//PORTD�����������  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�����������   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOF, &GPIO_InitStructure); 
 		  
	readWriteTiming.FSMC_AddressSetupTime = 0x01;	 //��ַ����ʱ�䣨ADDSET��Ϊ2��HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�	
    readWriteTiming.FSMC_DataSetupTime = 0x0f;		 // ���ݱ���ʱ��Ϊ16��HCLK,��ΪҺ������IC�Ķ����ݵ�ʱ���ٶȲ���̫�죬�����1289���IC��
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 
    
		 
	writeTiming.FSMC_AddressSetupTime = 0x00;	 //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK  
    writeTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨A		
    writeTiming.FSMC_DataSetupTime = 0x03;		 ////���ݱ���ʱ��Ϊ4��HCLK	
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 
		  
	   
 
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//  ��������ʹ��NE4 ��Ҳ�Ͷ�ӦBTCR[6],[7]��
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // ���������ݵ�ַ
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//�洢�����ݿ��Ϊ16bit   
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  �洢��дʹ��
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // ��дʹ�ò�ͬ��ʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //��дʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //дʱ��
	

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //��ʼ��FSMC����

   	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // ʹ��BANK1 
			 
 	delay_ms(50); // delay 50 ms 
 	LCD_WriteReg(0x0000,0x0001);
	delay_ms(50); // delay 50 ms 
  	gLcdDevice.id = LCD_ReadReg(0x0000);   
  		 
 	if(gLcdDevice.id==0x9325)//9325
	{
		LCD_WriteReg(0x00E5,0x78F0); 
		LCD_WriteReg(0x0001,0x0100); 
		LCD_WriteReg(0x0002,0x0700); 
		LCD_WriteReg(0x0003,0x1030); 
		LCD_WriteReg(0x0004,0x0000); 
		LCD_WriteReg(0x0008,0x0202);  
		LCD_WriteReg(0x0009,0x0000);
		LCD_WriteReg(0x000A,0x0000); 
		LCD_WriteReg(0x000C,0x0000); 
		LCD_WriteReg(0x000D,0x0000);
		LCD_WriteReg(0x000F,0x0000);
		//power on sequence VGHVGL
		LCD_WriteReg(0x0010,0x0000);   
		LCD_WriteReg(0x0011,0x0007);  
		LCD_WriteReg(0x0012,0x0000);  
		LCD_WriteReg(0x0013,0x0000); 
		LCD_WriteReg(0x0007,0x0000); 
		//vgh 
		LCD_WriteReg(0x0010,0x1690);   
		LCD_WriteReg(0x0011,0x0227);
		//delayms(100);
		//vregiout 
		LCD_WriteReg(0x0012,0x009D); //0x001b
		//delayms(100); 
		//vom amplitude
		LCD_WriteReg(0x0013,0x1900);
		//delayms(100); 
		//vom H
		LCD_WriteReg(0x0029,0x0025); 
		LCD_WriteReg(0x002B,0x000D); 
		//gamma
		LCD_WriteReg(0x0030,0x0007);
		LCD_WriteReg(0x0031,0x0303);
		LCD_WriteReg(0x0032,0x0003);// 0006
		LCD_WriteReg(0x0035,0x0206);
		LCD_WriteReg(0x0036,0x0008);
		LCD_WriteReg(0x0037,0x0406); 
		LCD_WriteReg(0x0038,0x0304);//0200
		LCD_WriteReg(0x0039,0x0007); 
		LCD_WriteReg(0x003C,0x0602);// 0504
		LCD_WriteReg(0x003D,0x0008); 
		//ram
		LCD_WriteReg(0x0050,0x0000); 
		LCD_WriteReg(0x0051,0x00EF);
		LCD_WriteReg(0x0052,0x0000); 
		LCD_WriteReg(0x0053,0x013F);  
		LCD_WriteReg(0x0060,0xA700); 
		LCD_WriteReg(0x0061,0x0001); 
		LCD_WriteReg(0x006A,0x0000); 
		//
		LCD_WriteReg(0x0080,0x0000); 
		LCD_WriteReg(0x0081,0x0000); 
		LCD_WriteReg(0x0082,0x0000); 
		LCD_WriteReg(0x0083,0x0000); 
		LCD_WriteReg(0x0084,0x0000); 
		LCD_WriteReg(0x0085,0x0000); 
		//
		LCD_WriteReg(0x0090,0x0010); 
		LCD_WriteReg(0x0092,0x0600); 
		
		LCD_WriteReg(0x0007,0x0133);
		LCD_WriteReg(0x00,0x0022);//
	}
	BSP_LCD_Display_Dir(0);		 	//Ĭ��Ϊ����
	LCD_LED=1;
						//��������
	BSP_LCD_Clear(WHITE);
}  

/*
*	��������	BSP_LCD_Clear
*	������		����
*	���룺		color:Ҫ���������ɫ
*	�����		
*/
void BSP_LCD_Clear(uint16_t color)
{
	u32 index=0;
	u32 totalpoint = gLcdDevice.width*gLcdDevice.height; 	//�õ��ܵ���
	
	LCD_SetCursor(0x00,0x0000);	//���ù��λ�� 
	LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
	for(index=0; index<totalpoint; index++)
	{
		LCD->LCD_RAM=color;		   			  
	}
}  	


/*
*	��������	BSP_LCD_Fill
*	������		��ָ����������䵥����ɫ
*	���룺		startx,starty��ʼ����
*			endx,endy�Խ�����
*			colorҪ���������ɫ
*	�����		
*/
void BSP_LCD_Fill(uint16_t startx, uint16_t starty, uint16_t endx, uint16_t endy, uint16_t color)
{          
	u16 i,j;
	u16 xlen = endx-startx+1;	   
	for(i=starty;i<=endy;i++)
	{
	 	LCD_SetCursor(startx,i);		//���ù��λ�� 
		LCD_WriteRAM_Prepare();			//��ʼд��GRAM	  
		for(j=0;j<xlen;j++)
			LCD_WR_DATA(color);			//���ù��λ�� 	    
	}
} 

/*
*	��������	BSP_LCD_DrawCircle
*	������		��ָ��λ�û�һ��ָ����С��Բ
*	���룺		x,yԭ������;r�뾶
*	�����		
*/
void BSP_LCD_DrawCircle(uint16_t x0,uint16_t y0,uint8_t r)
{
	int a=0, b=r;
	int di = 3-(r<<1);             //�ж��¸���λ�õı�־

	while( a <= b )
	{
		BSP_LCD_Fast_DrawPoint(x0+a,y0-b);             //5
 		BSP_LCD_Fast_DrawPoint(x0+b,y0-a);             //0           
		BSP_LCD_Fast_DrawPoint(x0+b,y0+a);             //4               
		BSP_LCD_Fast_DrawPoint(x0+a,y0+b);             //6 
		BSP_LCD_Fast_DrawPoint(x0-a,y0+b);             //1       
 		BSP_LCD_Fast_DrawPoint(x0-b,y0+a);             
		BSP_LCD_Fast_DrawPoint(x0-a,y0-b);             //2             
  		BSP_LCD_Fast_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)
			di += 4*a+6;	  
		else
		{
			di += 10+4*(a-b);   
			b--;
		}
	}
}

//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void BSP_LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u16 colortemp=POINT_COLOR;      			     
	//���ô���		   
	num=num-' ';//�õ�ƫ�ƺ��ֵ
	if(!mode) //�ǵ��ӷ�ʽ
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //����1206����
			else temp=asc2_1608[num][t];		 //����1608���� 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)POINT_COLOR=colortemp;
				else POINT_COLOR=BACK_COLOR;
				BSP_LCD_Fast_DrawPoint(x,y);	
				temp<<=1;
				y++;
				if(x>=gLcdDevice.width){POINT_COLOR=colortemp;return;}//��������
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=gLcdDevice.width){POINT_COLOR=colortemp;return;}//��������
					break;
				}
			}  	 
	    }    
	}else//���ӷ�ʽ
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //����1206����
			else temp=asc2_1608[num][t];		 //����1608���� 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)BSP_LCD_Fast_DrawPoint(x,y); 
				temp<<=1;
				y++;
				if(x>=gLcdDevice.height){POINT_COLOR=colortemp;return;}//��������
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=gLcdDevice.width){POINT_COLOR=colortemp;return;}//��������
					break;
				}
			}  	 
	    }     
	}
	POINT_COLOR=colortemp;	    	   	 	  
}   

//m^n����
//����ֵ:m^n�η�.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
void BSP_LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				BSP_LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	BSP_LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~999999999);	 
//len:����(��Ҫ��ʾ��λ��)
//size:�����С
//mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
void BSP_LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)BSP_LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else BSP_LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	BSP_LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//��ʾ�ַ���
//x,y:�������
//width,height:�����С  
//size:�����С
//*p:�ַ�����ʼ��ַ		  
void BSP_LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;

    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        BSP_LCD_ShowChar(x,y,*p,size,1);
        x+=size/2;
        p++;
    }  
}

void BSP_LCD_ShowStringNotStack(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;

    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        BSP_LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}

//startx  ����ʼ����
//starty   ����ʼ����
//Endx	  �н�������
//Endy	   �н�������
//pic	  ͼƬ��ͷָ��
void BSP_LCD_DrawPicture(u16 startx,u16 starty,u16 Endx,u16 Endy,u8 pic[])
{
	union _Data
	{
		unsigned short int u16;
		unsigned char u8[2];
	}Data;
	
	int i,LONG;
  	LCD_SetCursor(startx,starty);
	LCD_WriteRAM_Prepare();
	LONG=(Endx*Endy);	 //�õ��ܴ�С
	for(i=0;i<LONG;i++)
	{
 		Data.u8[1]=pic[i*2+1];
		Data.u8[0]=pic[i*2];
		LCD_WriteRAM(Data.u16);	
	}
}


static void GetFont_from_EXFlash( uint8_t * pBuffer, uint16_t pCharCH )
{ 
    uint8_t High8bit,Low8bit;
    uint32_t pos;
    
	if ( pBuffer == NULL )
		return ;
	
    High8bit = pCharCH >> 8;     /* ȡ��8λ���� */
    Low8bit  = pCharCH & 0x00FF;  /* ȡ��8λ���� */		

    pos = ((High8bit-0xb0)*94+Low8bit-0xa1)*32; 		/* GB2312 Ѱַ��ʽ */
    
    memcpy(pBuffer, gb2312+pos, 32);
}


void BSP_LCD_DrawChar_CH ( uint16_t startx, uint16_t starty, uint16_t usChar )
{
    uint8_t pos_x;
    uint8_t pos_y;
    uint8_t arrayPoint[32];

	if ( startx>=gLcdDevice.width || starty>=gLcdDevice.height )
		return ;

    GetFont_from_EXFlash( arrayPoint, usChar );
    
    for ( pos_y=0; pos_y<16; pos_y++ )
    {
        LCD_SetCursor( startx, starty+pos_y );
		LCD_WriteRAM_Prepare();
		
        for ( pos_x=0; pos_x<8;pos_x++ )
        {
            if ( arrayPoint[pos_y*2] & (1<<pos_x) )
                BSP_LCD_Fast_DrawPoint( startx+pos_x, starty+pos_y);
        }
        for ( pos_x=0; pos_x<8;pos_x++ )
        {
            if ( arrayPoint[pos_y*2+1] & (1<<pos_x) )
                BSP_LCD_Fast_DrawPoint( startx+pos_x+8, starty+pos_y);
        }
    }
}

/*
*	��������	BSP_LCD_DrawString_CH
*	������		���һ�����Ĵ�
*	���룺		startx,starty��ʼ����;pStr���Ĵ�;len����
*	�����		
*/
void BSP_LCD_DrawString_CH ( uint16_t startx, uint16_t starty, char *pStr, uint8_t len )
{
    uint16_t usCh;
    uint8_t size=0;

	if ( pStr == NULL || len == 0 )
		return ;

	if ( startx>=gLcdDevice.width || starty>=gLcdDevice.height )
		return ;
	
    while ( len-- )
    {
        usCh = * ( uint16_t * ) pStr;
        usCh = ( usCh << 8 ) + ( usCh >> 8 );
//        printf("usCh %4x\n",usCh);
        BSP_LCD_DrawChar_CH( startx+size*16, starty, usCh );
        pStr += 2;
        size++;
    }
}
