#include "stm32f10x_spi.h"
#include "./FLASH/bsp_flash.h" 
#include "./SPI/bsp_spi.h"
#include "./DELAY/bsp_delay.h"   


static uint16_t sSpiFlashID = W25Q64;//Ĭ�Ͼ���25Q64

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25X16
//����Ϊ2M�ֽ�,����32��Block,512��Sector 

static uint8_t sSpiFlashBuffer[4096];

static void SPI_Flash_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
static void SPI_Flash_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
static uint8_t SPI_Flash_ReadSR(void);
//static void SPI_FLASH_Write_SR(uint8_t sr);
static void SPI_Flash_Wait_Busy(void);


/*
*	������:	BSP_SPI_Flash_Init
*	����:		��ʼ���ⲿFlash
*	����:
*	���:
*/
void BSP_SPI_Flash_Init(void)
{	
  	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB
						|	RCC_APB2Periph_GPIOD
						|	RCC_APB2Periph_GPIOG, ENABLE );//PORTBʱ��ʹ�� 

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_12;			//PB12 ����
 	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;		//�������
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2;			//PD2 ����
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOD,GPIO_Pin_2);

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_7;			//PG7 ����
 	GPIO_Init(GPIOG, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOG,GPIO_Pin_7);

	SPI2_Init();											//��ʼ��SPI
	SPI2_SetSpeed(SPI_BaudRatePrescaler_2);					//����Ϊ18Mʱ��,����ģʽ
	sSpiFlashID = BSP_SPI_Flash_ReadID();					//��ȡFLASH ID.
}  


/*
*	������:	BSP_SPI_Flash_GetType
*	����:		�����ⲿflash������
*	����:
*	���:		sSpiFlashID�ⲿflash������
*/
uint16_t BSP_SPI_Flash_GetID(void)
{
	return sSpiFlashID;
}

/*
*	������:	BSP_SPI_Flash_ReadType
*	����:		��ȡ�ⲿflash�����ͣ�������
*	����:
*	���:		sSpiFlashID�ⲿflash������
*/
uint16_t BSP_SPI_Flash_ReadID(void)
{  
	SPI_FLASH_CS = 0;				    
	SPI2_ReadWriteByte(W25X_ManufactDeviceID);//���Ͷ�ȡID����	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	 			   
	sSpiFlashID |= SPI2_ReadWriteByte(0xFF)<<8;  
	sSpiFlashID |= SPI2_ReadWriteByte(0xFF);	 
	SPI_FLASH_CS = 1;				    
	return sSpiFlashID;
}

/*
*	������:	BSP_SPI_Flash_Read
*	����:		��ָ����ַ��ʼ��ȡָ�����ȵ�����(RAMֻ��64K,��ȡ�ٶ�Ҳû��Ҫ��)
*	����:		pBuffer:���ݴ洢����
*			ReadAddr��ʼ��ȡ�ĵ�ַ(24bit)��
*			NumByteToRead:Ҫ��ȡ���ֽ���(���65535���ֽ�)
*	���:
*/
void BSP_SPI_Flash_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{ 
 	uint16_t i;    												    
	SPI_FLASH_CS = 0;                          		//ʹ������   
    SPI2_ReadWriteByte(W25X_ReadData);         		//���Ͷ�ȡ����   
    SPI2_ReadWriteByte((uint8_t)((ReadAddr)>>16));  //����24bit��ַ    
    SPI2_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    SPI2_ReadWriteByte((uint8_t)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI2_ReadWriteByte(0XFF);   		//ѭ������  
    }
	SPI_FLASH_CS = 1;                          		//ȡ��Ƭѡ     	      
}


/*
*	������:	BSP_SPI_Flash_Write
*	����:		�ú�������������;��ָ����ַ��ʼд��ָ�����ȵ�����(RAMֻ��64K,д���ٶ�Ҳû��Ҫ��)
*	����:		pBuffer:���ݴ洢��;
*			WriteAddr:��ʼд��ĵ�ַ(24bit);
*			NumByteToRead:Ҫ��ȡ���ֽ���(���65535���ֽ�);
*	���:
*/
void BSP_SPI_Flash_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    

	secpos = WriteAddr / 4096;	//������ַ 0~511 for w25x16
	secoff = WriteAddr % 4096;	//�������ڵ�ƫ��
	secremain = 4096 - secoff;	//����ʣ��ռ��С   

	if(NumByteToWrite <= secremain)//������4096���ֽ�
		secremain = NumByteToWrite;
	while(1) 
	{	
		BSP_SPI_Flash_Read(sSpiFlashBuffer, secpos*4096 ,4096);		//������������������

		for(i=0; i<secremain; i++)								//У������,ȷ��д�뷶Χȫ��Ϊ0xFF
		{
			if(sSpiFlashBuffer[secoff+i] != 0xFF)				//��Ҫ����
				break;
		}
		if(i < secremain)//��Ҫ����
		{
			BSP_SPI_Flash_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				sSpiFlashBuffer[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(sSpiFlashBuffer,secpos*4096,4096);//д����������  
		}
		else
			SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   

		if(NumByteToWrite == secremain)
			break;//д�������
		else//д��δ����
		{
			secpos++;		//������ַ��1
			secoff = 0;		//ƫ��λ��Ϊ0 	 

		   	pBuffer			+= secremain;	//ָ��ƫ��
			WriteAddr		+= secremain;	//д��ַƫ��	   
		   	NumByteToWrite	-= secremain;	//�ֽ����ݼ�

			if(NumByteToWrite > 4096)
				secremain=4096;				//��һ����������д����
			else
				secremain=NumByteToWrite;	//��һ����������д����
		}	 
	} 	 
}

/*
*	������:	BSP_SPI_Flash_Erase_Chip
*	����:		��������оƬ
*	����:
*	���:
*/
void BSP_SPI_Flash_Erase_Chip(void)
{                                             
    BSP_SPI_FLASH_Write_Enable();                  //SET WEL
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS=0;                            //ʹ������
    SPI2_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ
	SPI_Flash_Wait_Busy();   				   //�ȴ�оƬ��������
}

/*
*	������:	BSP_SPI_Flash_Erase_Sector
*	����:		����һ������
*	����:		Dst_Addr:������ַ
*	���:
*/
void BSP_SPI_Flash_Erase_Sector(u32 Dst_Addr)
{   
	Dst_Addr*=4096;
    BSP_SPI_FLASH_Write_Enable();                  //SET WEL 	 
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS=0;                            //ʹ������   
    SPI2_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //����24bit��ַ    
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI2_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
    SPI_Flash_Wait_Busy();   				   //�ȴ��������
}  

//�������ģʽ
void BSP_SPI_Flash_PowerDown(void)
{ 
  	SPI_FLASH_CS = 0;							//ʹ������   
    SPI2_ReadWriteByte(W25X_PowerDown);			//���͵�������  
	SPI_FLASH_CS = 1;							//ȡ��Ƭѡ     	      
    delay_us(3);								//�ȴ�TPD  
}

//����
void BSP_SPI_Flash_Wakeup(void)
{  
  	SPI_FLASH_CS = 0;							//ʹ������   
    SPI2_ReadWriteByte(W25X_ReleasePowerDown);	//send W25X_PowerDown command 0xAB    
	SPI_FLASH_CS = 1;							//ȡ��Ƭѡ     	      
    delay_us(3);								//�ȴ�TRES1
}

//SPI_FLASHдʹ��	
//��WEL��λ   
void BSP_SPI_FLASH_Write_Enable(void)
{
	SPI_FLASH_CS = 0;							//ʹ������   
    SPI2_ReadWriteByte(W25X_WriteEnable);		//����дʹ��  
	SPI_FLASH_CS = 1;							//ȡ��Ƭѡ     	      
}

//SPI_FLASHд��ֹ	
//��WEL����  
void BSP_SPI_FLASH_Write_Disable(void)
{
	SPI_FLASH_CS = 0;							//ʹ������   
    SPI2_ReadWriteByte(W25X_WriteDisable);		//����д��ָֹ��    
	SPI_FLASH_CS = 1;							//ȡ��Ƭѡ     	      
}

/*
*	������:	SPI_Flash_Write_Page
*	����:		��ָ����ַ��ʼд�����256�ֽڵ�����
*	����:		pBuffer:���ݴ洢��;
*			WriteAddr:��ʼд��ĵ�ַ(24bit);
*			NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
*	���:
*/
static void SPI_Flash_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
 	uint16_t i;  
    BSP_SPI_FLASH_Write_Enable();							//SET WEL 
	SPI_FLASH_CS = 0;									//ʹ������   
    SPI2_ReadWriteByte(W25X_PageProgram);				//����дҳ����   
    SPI2_ReadWriteByte((uint8_t)((WriteAddr)>>16));		//����24bit��ַ    
    SPI2_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    SPI2_ReadWriteByte((uint8_t)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)
		SPI2_ReadWriteByte(pBuffer[i]);					//ѭ��д��  
	SPI_FLASH_CS = 1;									//ȡ��Ƭѡ 
	SPI_Flash_Wait_Busy();								//�ȴ�д�����
}

/*
*	������:	SPI_Flash_Write_NoCheck
*	����:		�޼���дSPI FLASH;�����Զ���ҳ����;
*			����ȷ����д�ĵ�ַ��Χ��ȫ��Ϊ0xFF,�����ڷ�0xFF����д��ʧ��!
*	����:		pBuffer:���ݴ洢��;
*			WriteAddr:��ʼд��ĵ�ַ(24bit)
*			NumByteToWrite:Ҫд����ֽ���(���65535)
*	���:
*/
static void SPI_Flash_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{ 			 		 
	uint16_t pageremain;	   
	pageremain = 256 - WriteAddr % 256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite <= pageremain)
		pageremain = NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)
			break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			//��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite > 256)
				pageremain = 256;				//һ�ο���д��256���ֽ�
			else
				pageremain = NumByteToWrite;	//����256���ֽ���
		}
	};	    
}


//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
static uint8_t SPI_Flash_ReadSR(void)
{  
	uint8_t byte = 0;   
	SPI_FLASH_CS = 0;							//ʹ������   
	SPI2_ReadWriteByte(W25X_ReadStatusReg);		//���Ͷ�ȡ״̬�Ĵ�������    
	byte = SPI2_ReadWriteByte(0Xff);			//��ȡһ���ֽ�  
	SPI_FLASH_CS = 1;							//ȡ��Ƭѡ     
	return byte;   
}

////дSPI_FLASH״̬�Ĵ���
////ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
//static void SPI_FLASH_Write_SR(uint8_t sr)
//{   
//	SPI_FLASH_CS = 0;							//ʹ������   
//	SPI2_ReadWriteByte(W25X_WriteStatusReg);	//����дȡ״̬�Ĵ�������    
//	SPI2_ReadWriteByte(sr);						//д��һ���ֽ�  
//	SPI_FLASH_CS = 1;							//ȡ��Ƭѡ     	      
//}

//�ȴ�����
static void SPI_Flash_Wait_Busy(void)
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);	//�ȴ�BUSYλ���
}  

