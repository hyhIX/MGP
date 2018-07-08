#include "./CRC/bsp_crc.h"

__IO uint32_t gCRCValue;		 // 用于存放产生的CRC校验值

/*
 * 函数名：CRC_Config
 * 描述  ：使能CRC时钟
 * 输入  ：无
 * 输出  ：无
 * 调用  : 外部调用
 */
void BSP_CRC_Config(void)
{
	/* Enable CRC clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
}


uint32_t BSP_CalcBlockCRC ( uint32_t *DataBuffer, uint32_t length )
{
    CRC_ResetDR(  );
    gCRCValue = CRC_CalcBlockCRC( DataBuffer, length );
    return gCRCValue;
}

