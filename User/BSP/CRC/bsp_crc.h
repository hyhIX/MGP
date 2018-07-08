#ifndef __CRC_H__
#define	__CRC_H__

#include "stm32f10x.h"


void BSP_CRC_Config(void);
uint32_t BSP_CalcBlockCRC ( uint32_t *DataBuffer, uint32_t length );

#endif /* __CRC_H */
