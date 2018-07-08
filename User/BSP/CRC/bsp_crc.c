#include "./CRC/bsp_crc.h"

__IO uint32_t gCRCValue;		 // ���ڴ�Ų�����CRCУ��ֵ

/*
 * ��������CRC_Config
 * ����  ��ʹ��CRCʱ��
 * ����  ����
 * ���  ����
 * ����  : �ⲿ����
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

