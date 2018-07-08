#include "stm32f10x_spi.h"
#include "./FLASH/bsp_flash.h" 
#include "./SPI/bsp_spi.h"
#include "./DELAY/bsp_delay.h"   


static uint16_t sSpiFlashID = W25Q64;//默认就是25Q64

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25X16
//容量为2M字节,共有32个Block,512个Sector 

static uint8_t sSpiFlashBuffer[4096];

static void SPI_Flash_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
static void SPI_Flash_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
static uint8_t SPI_Flash_ReadSR(void);
//static void SPI_FLASH_Write_SR(uint8_t sr);
static void SPI_Flash_Wait_Busy(void);


/*
*	函数名:	BSP_SPI_Flash_Init
*	描述:		初始化外部Flash
*	输入:
*	输出:
*/
void BSP_SPI_Flash_Init(void)
{	
  	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB
						|	RCC_APB2Periph_GPIOD
						|	RCC_APB2Periph_GPIOG, ENABLE );//PORTB时钟使能 

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_12;			//PB12 推挽
 	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2;			//PD2 推挽
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOD,GPIO_Pin_2);

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_7;			//PG7 推挽
 	GPIO_Init(GPIOG, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOG,GPIO_Pin_7);

	SPI2_Init();											//初始化SPI
	SPI2_SetSpeed(SPI_BaudRatePrescaler_2);					//设置为18M时钟,高速模式
	sSpiFlashID = BSP_SPI_Flash_ReadID();					//读取FLASH ID.
}  


/*
*	函数名:	BSP_SPI_Flash_GetType
*	描述:		返回外部flash的类型
*	输入:
*	输出:		sSpiFlashID外部flash的类型
*/
uint16_t BSP_SPI_Flash_GetID(void)
{
	return sSpiFlashID;
}

/*
*	函数名:	BSP_SPI_Flash_ReadType
*	描述:		读取外部flash的类型，并返回
*	输入:
*	输出:		sSpiFlashID外部flash的类型
*/
uint16_t BSP_SPI_Flash_ReadID(void)
{  
	SPI_FLASH_CS = 0;				    
	SPI2_ReadWriteByte(W25X_ManufactDeviceID);//发送读取ID命令	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	 			   
	sSpiFlashID |= SPI2_ReadWriteByte(0xFF)<<8;  
	sSpiFlashID |= SPI2_ReadWriteByte(0xFF);	 
	SPI_FLASH_CS = 1;				    
	return sSpiFlashID;
}

/*
*	函数名:	BSP_SPI_Flash_Read
*	描述:		在指定地址开始读取指定长度的数据(RAM只有64K,读取再多也没必要了)
*	输入:		pBuffer:数据存储区；
*			ReadAddr开始读取的地址(24bit)；
*			NumByteToRead:要读取的字节数(最大65535个字节)
*	输出:
*/
void BSP_SPI_Flash_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{ 
 	uint16_t i;    												    
	SPI_FLASH_CS = 0;                          		//使能器件   
    SPI2_ReadWriteByte(W25X_ReadData);         		//发送读取命令   
    SPI2_ReadWriteByte((uint8_t)((ReadAddr)>>16));  //发送24bit地址    
    SPI2_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    SPI2_ReadWriteByte((uint8_t)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI2_ReadWriteByte(0XFF);   		//循环读数  
    }
	SPI_FLASH_CS = 1;                          		//取消片选     	      
}


/*
*	函数名:	BSP_SPI_Flash_Write
*	描述:		该函数带擦除操作;在指定地址开始写入指定长度的数据(RAM只有64K,写入再多也没必要了)
*	输入:		pBuffer:数据存储区;
*			WriteAddr:开始写入的地址(24bit);
*			NumByteToRead:要读取的字节数(最大65535个字节);
*	输出:
*/
void BSP_SPI_Flash_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    

	secpos = WriteAddr / 4096;	//扇区地址 0~511 for w25x16
	secoff = WriteAddr % 4096;	//在扇区内的偏移
	secremain = 4096 - secoff;	//扇区剩余空间大小   

	if(NumByteToWrite <= secremain)//不大于4096个字节
		secremain = NumByteToWrite;
	while(1) 
	{	
		BSP_SPI_Flash_Read(sSpiFlashBuffer, secpos*4096 ,4096);		//读出整个扇区的内容

		for(i=0; i<secremain; i++)								//校验数据,确保写入范围全部为0xFF
		{
			if(sSpiFlashBuffer[secoff+i] != 0xFF)				//需要擦除
				break;
		}
		if(i < secremain)//需要擦除
		{
			BSP_SPI_Flash_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				sSpiFlashBuffer[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(sSpiFlashBuffer,secpos*4096,4096);//写入整个扇区  
		}
		else
			SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   

		if(NumByteToWrite == secremain)
			break;//写入结束了
		else//写入未结束
		{
			secpos++;		//扇区地址增1
			secoff = 0;		//偏移位置为0 	 

		   	pBuffer			+= secremain;	//指针偏移
			WriteAddr		+= secremain;	//写地址偏移	   
		   	NumByteToWrite	-= secremain;	//字节数递减

			if(NumByteToWrite > 4096)
				secremain=4096;				//下一个扇区还是写不完
			else
				secremain=NumByteToWrite;	//下一个扇区可以写完了
		}	 
	} 	 
}

/*
*	函数名:	BSP_SPI_Flash_Erase_Chip
*	描述:		擦除整个芯片
*	输入:
*	输出:
*/
void BSP_SPI_Flash_Erase_Chip(void)
{                                             
    BSP_SPI_FLASH_Write_Enable();                  //SET WEL
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS=0;                            //使能器件
    SPI2_ReadWriteByte(W25X_ChipErase);        //发送片擦除命令
	SPI_FLASH_CS=1;                            //取消片选
	SPI_Flash_Wait_Busy();   				   //等待芯片擦除结束
}

/*
*	函数名:	BSP_SPI_Flash_Erase_Sector
*	描述:		擦除一个扇区
*	输入:		Dst_Addr:扇区地址
*	输出:
*/
void BSP_SPI_Flash_Erase_Sector(u32 Dst_Addr)
{   
	Dst_Addr*=4096;
    BSP_SPI_FLASH_Write_Enable();                  //SET WEL 	 
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS=0;                            //使能器件   
    SPI2_ReadWriteByte(W25X_SectorErase);      //发送扇区擦除指令 
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //发送24bit地址    
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI2_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_CS=1;                            //取消片选     	      
    SPI_Flash_Wait_Busy();   				   //等待擦除完成
}  

//进入掉电模式
void BSP_SPI_Flash_PowerDown(void)
{ 
  	SPI_FLASH_CS = 0;							//使能器件   
    SPI2_ReadWriteByte(W25X_PowerDown);			//发送掉电命令  
	SPI_FLASH_CS = 1;							//取消片选     	      
    delay_us(3);								//等待TPD  
}

//唤醒
void BSP_SPI_Flash_Wakeup(void)
{  
  	SPI_FLASH_CS = 0;							//使能器件   
    SPI2_ReadWriteByte(W25X_ReleasePowerDown);	//send W25X_PowerDown command 0xAB    
	SPI_FLASH_CS = 1;							//取消片选     	      
    delay_us(3);								//等待TRES1
}

//SPI_FLASH写使能	
//将WEL置位   
void BSP_SPI_FLASH_Write_Enable(void)
{
	SPI_FLASH_CS = 0;							//使能器件   
    SPI2_ReadWriteByte(W25X_WriteEnable);		//发送写使能  
	SPI_FLASH_CS = 1;							//取消片选     	      
}

//SPI_FLASH写禁止	
//将WEL清零  
void BSP_SPI_FLASH_Write_Disable(void)
{
	SPI_FLASH_CS = 0;							//使能器件   
    SPI2_ReadWriteByte(W25X_WriteDisable);		//发送写禁止指令    
	SPI_FLASH_CS = 1;							//取消片选     	      
}

/*
*	函数名:	SPI_Flash_Write_Page
*	描述:		在指定地址开始写入最大256字节的数据
*	输入:		pBuffer:数据存储区;
*			WriteAddr:开始写入的地址(24bit);
*			NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
*	输出:
*/
static void SPI_Flash_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
 	uint16_t i;  
    BSP_SPI_FLASH_Write_Enable();							//SET WEL 
	SPI_FLASH_CS = 0;									//使能器件   
    SPI2_ReadWriteByte(W25X_PageProgram);				//发送写页命令   
    SPI2_ReadWriteByte((uint8_t)((WriteAddr)>>16));		//发送24bit地址    
    SPI2_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    SPI2_ReadWriteByte((uint8_t)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)
		SPI2_ReadWriteByte(pBuffer[i]);					//循环写数  
	SPI_FLASH_CS = 1;									//取消片选 
	SPI_Flash_Wait_Busy();								//等待写入结束
}

/*
*	函数名:	SPI_Flash_Write_NoCheck
*	描述:		无检验写SPI FLASH;具有自动换页功能;
*			必须确保所写的地址范围内全部为0xFF,否则在非0xFF处将写入失败!
*	输入:		pBuffer:数据存储区;
*			WriteAddr:开始写入的地址(24bit)
*			NumByteToWrite:要写入的字节数(最大65535)
*	输出:
*/
static void SPI_Flash_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{ 			 		 
	uint16_t pageremain;	   
	pageremain = 256 - WriteAddr % 256; //单页剩余的字节数		 	    
	if(NumByteToWrite <= pageremain)
		pageremain = NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)
			break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			//减去已经写入了的字节数
			if(NumByteToWrite > 256)
				pageremain = 256;				//一次可以写入256个字节
			else
				pageremain = NumByteToWrite;	//不够256个字节了
		}
	};	    
}


//读取SPI_FLASH的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
static uint8_t SPI_Flash_ReadSR(void)
{  
	uint8_t byte = 0;   
	SPI_FLASH_CS = 0;							//使能器件   
	SPI2_ReadWriteByte(W25X_ReadStatusReg);		//发送读取状态寄存器命令    
	byte = SPI2_ReadWriteByte(0Xff);			//读取一个字节  
	SPI_FLASH_CS = 1;							//取消片选     
	return byte;   
}

////写SPI_FLASH状态寄存器
////只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
//static void SPI_FLASH_Write_SR(uint8_t sr)
//{   
//	SPI_FLASH_CS = 0;							//使能器件   
//	SPI2_ReadWriteByte(W25X_WriteStatusReg);	//发送写取状态寄存器命令    
//	SPI2_ReadWriteByte(sr);						//写入一个字节  
//	SPI_FLASH_CS = 1;							//取消片选     	      
//}

//等待空闲
static void SPI_Flash_Wait_Busy(void)
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);	//等待BUSY位清空
}  

