#include "ddc112.h"
#include "io.h"
#include "misc.h"
#include "uart.h"

uint8_t DDC112_Read(uint32_t *pReadData)
{
    int8_t i, j;
    uint8_t *p = (uint8_t*)pReadData;

	if(IN_VALID1 == 0) //���ݿɶ�
    {
        *pReadData = 0;

		OUT_DXMIT1 = 0; //ʹ����λ���ݼĴ���

        //��DDC112��2ͨ������
        for(j=3;j>=0;j--)
        {
			OUT_SCLK1 = 1;
            
			if(IN_DATA1 == 1)
                p[1] |= (0x01<<j);

			OUT_SCLK1 = 0;
        }
        for(i=2;i<4;i++)
        {
            for(j=7;j>=0;j--)
            {
                OUT_SCLK1 = 1;

				if(IN_DATA1 == 1)
                    p[i] |= (0x01<<j);

				OUT_SCLK1 = 0;
            }
        }
        //��DDC112��1ͨ������
        for(j=3;j>=0;j--)
        {
			OUT_SCLK1 = 1;

//            if(IN_DATA1 == 1)
//                p[1] |= (0x01<<j);

			OUT_SCLK1 = 0;
        }
        for(i=6;i<8;i++)
        {
            for(j=7;j>=0;j--)
            {
				OUT_SCLK1 = 1;

//				if(IN_DATA1 == 1)
//                    p[i-4] |= (0x01<<j);

				OUT_SCLK1 = 0;
            }
        }
	
		OUT_DXMIT1 = 1; //������λ���ݼĴ���
		
        //����Ǹ���������Ϊ0�������ȥƫ�ơ�����������ʵ�ʲ����ã��������ݲ����ǳ���
//		if(p[2]&0x10)
//			p[2] &= ~0x10;
//		else
//            *((uint32_t*)p) = 0;

        if(*pReadData < 4096)
            *pReadData = 0;
        else
            *pReadData = *pReadData - 4096;

        return 0;
    }
    else
        return 1;
}

void DDC112_SendData(uint8_t head, uint8_t *pData, uint8_t count)
{
	int8_t i;
	Uart_SendData(head);
	for(i=0;i<count;i++)
        Uart_SendData(pData[i]);	
}
