#include "stm32f10x_fsmc.h"

#include "./USART/bsp_usart.h"	 
#include "./DELAY/bsp_delay.h"
#include "./FLASH/bsp_flash.h"
#include "./LCD/bsp_lcd.h"

#include "stdio.h"
#include "string.h"
#include "font.h"
				 
//LCD的画笔颜色和背景色	   
uint16_t POINT_COLOR    =   0x0000;     //画笔颜色
uint16_t BACK_COLOR     =   0xFFFF;     //背景色 

/* 管理LCD重要参数 */
tLcdDevice gLcdDevice;  //默认为竖屏

//运行状态
//static tLcdMode slcdMode = SYSTEM_NONE;
//static tLcdFontMode sLcdFontMode = EXTERN_FLASH;
//static uint8_t	sFontAddr = NULL;

//静态函数
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
*	函数名：LCD_WR_REG
*	描述：		写寄存器函数
*	输入：		regval:寄存器值
*	输出：
*/
static void LCD_WR_REG(uint16_t regval)
{
	LCD->LCD_REG=regval;//写入要写的寄存器序号
}

/*
*	函数名：LCD_WR_DATA
*	描述：		写LCD数据
*	输入：		data:要写入的值
*	输出：
*/
static void LCD_WR_DATA(uint16_t data)
{
	LCD->LCD_RAM=data;
}

/*
*	函数名：LCD_RD_DATA
*	描述：		读LCD数据
*	输入：		
*	输出：		读取到的值
*/
static uint16_t LCD_RD_DATA(void)
{
	return LCD->LCD_RAM;
}


/*
*	函数名：LCD_WriteReg
*	描述：		写寄存器
*	输入：		LCD_Reg:寄存器地址
*			LCD_RegValue:要写入的数据
*	输出：		
*/
static void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;		//写入要写的寄存器序号	 
	LCD->LCD_RAM = LCD_RegValue;//写入数据	    		 
}

/*
*	函数名：LCD_ReadReg
*	描述：		读寄存器
*	输入：		LCD_Reg:寄存器地址
*	输出：		读到的数据
*/
static uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
	LCD_WR_REG(LCD_Reg);		//写入要读的寄存器序号
	delay_us(5);
	return LCD_RD_DATA();		//返回读到的值
}


/*
*	函数名：LCD_WriteRAM_Prepare
*	描述：		开始写GRAM
*	输入：		
*	输出：		
*/
static void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=gLcdDevice.wramcmd;	  
}


/*
*	函数名：LCD_WriteRAM
*	描述：		写GRAM颜色
*	输入：		RGB_Code:颜色值
*	输出：		
*/
static void LCD_WriteRAM(uint16_t RGB_Code)
{							    
	LCD->LCD_RAM = RGB_Code;//写十六位GRAM
}

/*
*	函数名：LCD_SetCursor
*	描述：		设置光标位置
*	输入：		Xpos,Ypos坐标
*	输出：
*/
static void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	 
	if(gLcdDevice.dir==1)
		Xpos=gLcdDevice.width-1-Xpos;			//横屏其实就是调转x,y坐标
	LCD_WriteReg(gLcdDevice.setxcmd, Xpos);
	LCD_WriteReg(gLcdDevice.setycmd, Ypos); 
}

/*
*	函数名：BSP_LCD_DisplayOn
*	描述：		LCD开启显示
*	输入：		
*	输出：		
*/
void BSP_LCD_DisplayOn(void)
{					   
	LCD_WriteReg(R7,0x0173); 				//开启显示
} 


/*
*	函数名：	BSP_LCD_DisplayOff
*	描述：		LCD关闭显示
*	输入：		
*	输出：		
*/
void BSP_LCD_DisplayOff(void)
{	   
	LCD_WriteReg(R7,0x0);					//关闭显示 
}

/*
*	函数名：BSP_LCD_Scan_Dir
*	描述：		设置LCD的自动扫描方向
*	输入：		dir:0~7,代表8个方向(具体定义见lcd.h)
*	输出：
*/
void BSP_LCD_Scan_Dir(uint8_t dir)
{
	uint16_t	regval = 0;
	uint8_t		dirreg = 0;
	if(gLcdDevice.dir==1 )
	{			   
		switch(dir)//方向转换
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
		case L2R_U2D://从左到右,从上到下
			regval|=(1<<5)|(1<<4)|(0<<3); 
			break;
		case L2R_D2U://从左到右,从下到上
			regval|=(0<<5)|(1<<4)|(0<<3); 
			break;
		case R2L_U2D://从右到左,从上到下
			regval|=(1<<5)|(0<<4)|(0<<3);
			break;
		case R2L_D2U://从右到左,从下到上
			regval|=(0<<5)|(0<<4)|(0<<3); 
			break;	 
		case U2D_L2R://从上到下,从左到右
			regval|=(1<<5)|(1<<4)|(1<<3); 
			break;
		case U2D_R2L://从上到下,从右到左
			regval|=(1<<5)|(0<<4)|(1<<3); 
			break;
		case D2U_L2R://从下到上,从左到右
			regval|=(0<<5)|(1<<4)|(1<<3); 
			break;
		case D2U_R2L://从下到上,从右到左
			regval|=(0<<5)|(0<<4)|(1<<3); 
			break;	 
	}

	dirreg=0x03;
	regval|=1<<12;
	
	LCD_WriteReg(dirreg,regval);
	
}

/*
*	函数名：BSP_LCD_Fast_DrawPoint
*	描述：		快速画点
*	输入：		x,y:坐标;color:此点的颜色
*	输出：
*/
void BSP_LCD_Fast_DrawPoint(uint16_t x, uint16_t y)
{
    if(gLcdDevice.dir==1)
        x=gLcdDevice.width-1-x;//横屏其实就是调转x,y坐标
    LCD_WriteReg(gLcdDevice.setxcmd,x);
    LCD_WriteReg(gLcdDevice.setycmd,y);
		 
	LCD->LCD_REG=gLcdDevice.wramcmd; 
	LCD->LCD_RAM=POINT_COLOR; 
}	 


//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void BSP_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		BSP_LCD_Fast_DrawPoint(uRow,uCol);//画点 
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
*	函数名：	BSP_LCD_Display_Dir
*	描述：		设置LCD显示方向
*	输入：		dir:0,竖屏；1,横屏
*	输出：		
*/
void BSP_LCD_Display_Dir(uint8_t dir)
{
	if(dir==0)//竖屏
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
	BSP_LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}

/*
*	函数名：	BSP_LCD_Init
*	描述：		初始化LCD
*	输入：		
*	输出：		
*/
void BSP_LCD_Init(void)
{ 										  
	GPIO_InitTypeDef GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);	//使能FSMC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO,ENABLE);//使能PORTB,D,E,G以及AFIO复用功能时钟

 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //PB0 推挽输出 背光
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);

 	//PORTD复用推挽输出  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;				 //	//PORTD复用推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	  
	//PORTE复用推挽输出  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //	//PORTD复用推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure); 
	  
   	//	//PORTG12复用推挽输出 A0	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	 //	//PORTD复用推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOG, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	 //	//PORTD复用推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOF, &GPIO_InitStructure); 
 		  
	readWriteTiming.FSMC_AddressSetupTime = 0x01;	 //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
    readWriteTiming.FSMC_DataSetupTime = 0x0f;		 // 数据保存时间为16个HCLK,因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
    
		 
	writeTiming.FSMC_AddressSetupTime = 0x00;	 //地址建立时间（ADDSET）为1个HCLK  
    writeTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（A		
    writeTiming.FSMC_DataSetupTime = 0x03;		 ////数据保存时间为4个HCLK	
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
		  
	   
 
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//  这里我们使用NE4 ，也就对应BTCR[6],[7]。
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // 不复用数据地址
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit   
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  存储器写使能
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //读写时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //写时序
	

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

   	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // 使能BANK1 
			 
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
	BSP_LCD_Display_Dir(0);		 	//默认为竖屏
	LCD_LED=1;
						//点亮背光
	BSP_LCD_Clear(WHITE);
}  

/*
*	函数名：	BSP_LCD_Clear
*	描述：		清屏
*	输入：		color:要清屏的填充色
*	输出：		
*/
void BSP_LCD_Clear(uint16_t color)
{
	u32 index=0;
	u32 totalpoint = gLcdDevice.width*gLcdDevice.height; 	//得到总点数
	
	LCD_SetCursor(0x00,0x0000);	//设置光标位置 
	LCD_WriteRAM_Prepare();     //开始写入GRAM	
	for(index=0; index<totalpoint; index++)
	{
		LCD->LCD_RAM=color;		   			  
	}
}  	


/*
*	函数名：	BSP_LCD_Fill
*	描述：		在指定区域内填充单个颜色
*	输入：		startx,starty起始坐标
*			endx,endy对角坐标
*			color要清屏的填充色
*	输出：		
*/
void BSP_LCD_Fill(uint16_t startx, uint16_t starty, uint16_t endx, uint16_t endy, uint16_t color)
{          
	u16 i,j;
	u16 xlen = endx-startx+1;	   
	for(i=starty;i<=endy;i++)
	{
	 	LCD_SetCursor(startx,i);		//设置光标位置 
		LCD_WriteRAM_Prepare();			//开始写入GRAM	  
		for(j=0;j<xlen;j++)
			LCD_WR_DATA(color);			//设置光标位置 	    
	}
} 

/*
*	函数名：	BSP_LCD_DrawCircle
*	描述：		在指定位置画一个指定大小的圆
*	输入：		x,y原点坐标;r半径
*	输出：		
*/
void BSP_LCD_DrawCircle(uint16_t x0,uint16_t y0,uint8_t r)
{
	int a=0, b=r;
	int di = 3-(r<<1);             //判断下个点位置的标志

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
		//使用Bresenham算法画圆     
		if(di<0)
			di += 4*a+6;	  
		else
		{
			di += 10+4*(a-b);   
			b--;
		}
	}
}

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
void BSP_LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u16 colortemp=POINT_COLOR;      			     
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	if(!mode) //非叠加方式
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //调用1206字体
			else temp=asc2_1608[num][t];		 //调用1608字体 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)POINT_COLOR=colortemp;
				else POINT_COLOR=BACK_COLOR;
				BSP_LCD_Fast_DrawPoint(x,y);	
				temp<<=1;
				y++;
				if(x>=gLcdDevice.width){POINT_COLOR=colortemp;return;}//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=gLcdDevice.width){POINT_COLOR=colortemp;return;}//超区域了
					break;
				}
			}  	 
	    }    
	}else//叠加方式
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //调用1206字体
			else temp=asc2_1608[num][t];		 //调用1608字体 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)BSP_LCD_Fast_DrawPoint(x,y); 
				temp<<=1;
				y++;
				if(x>=gLcdDevice.height){POINT_COLOR=colortemp;return;}//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=gLcdDevice.width){POINT_COLOR=colortemp;return;}//超区域了
					break;
				}
			}  	 
	    }     
	}
	POINT_COLOR=colortemp;	    	   	 	  
}   

//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
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
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
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
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void BSP_LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;

    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
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

    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        BSP_LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}

//startx  行起始坐标
//starty   列起始坐标
//Endx	  行结束坐标
//Endy	   列结束坐标
//pic	  图片的头指针
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
	LONG=(Endx*Endy);	 //得到总大小
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
	
    High8bit = pCharCH >> 8;     /* 取高8位数据 */
    Low8bit  = pCharCH & 0x00FF;  /* 取低8位数据 */		

    pos = ((High8bit-0xb0)*94+Low8bit-0xa1)*32; 		/* GB2312 寻址公式 */
    
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
*	函数名：	BSP_LCD_DrawString_CH
*	描述：		输出一个中文串
*	输入：		startx,starty起始坐标;pStr中文串;len长度
*	输出：		
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
