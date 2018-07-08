#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__
#include "./SYS/bsp_sys.h" 

	  
/* W25X系列/Q系列芯片列表 */
#define W25Q80 	0xEF13
#define W25Q16 	0xEF14
#define W25Q32 	0xEF15
#define W25Q64 	0xEF16
#define W25Q128	0xEF17

#define	SPI_FLASH_CS PBout(12)  //选中FLASH	
				 
#define FLASH_FONT_ADDRESS  0
////////////////////////////////////////////////////////////////////////////
 
//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

void BSP_SPI_Flash_Init(void);
uint16_t BSP_SPI_Flash_GetID(void);					//获取静态变量sSpiFlashID中的ID
uint16_t BSP_SPI_Flash_ReadID(void);				//从芯片获取ID，并保存在sSpiFlashID中
void BSP_SPI_Flash_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void BSP_SPI_Flash_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite) ;
void BSP_SPI_Flash_Erase_Chip(void);
void BSP_SPI_Flash_Erase_Sector(u32 Dst_Addr);
void BSP_SPI_Flash_PowerDown(void);
void BSP_SPI_Flash_Wakeup(void);
void BSP_SPI_FLASH_Write_Enable(void);
void BSP_SPI_FLASH_Write_Disable(void);





#endif
















