#include "./DDC112/ddc112_interface.h"
#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TX_8(P1) SEND_DATA((P1)&0xFF)  //���͵����ֽ�
#define TX_8N(P,N) SendNU8((uint8 *)P,N)  //����N���ֽ�
#define TX_16(P1) TX_8((P1)>>8);TX_8(P1)  //����16λ����
#define TX_16N(P,N) SendNU16((uint16 *)P,N)  //����N��16λ����
#define TX_32(P1) TX_16((P1)>>16);TX_16((P1)&0xFFFF)  //����32λ����

static void SEND_DATA(uint8 c)
{
    USART_SendData(USART2, c);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET));//�ȴ����ڷ������
}

#define BEGIN_CMD()
#define END_CMD()

/*****************************************************************************************
*
*
*
******************************************************************************************/

void DDC112IF_RangeSetting(uint8_t range)
{
	BEGIN_CMD();
	TX_8(0x81);
	switch(range)
	{
		case 0:
			TX_8(0x00);
			TX_8(0x00);
			TX_8(0x00);
		break;
		case 1:
			TX_8(0x01);
			TX_8(0x00);
			TX_8(0x00);	
		break;
		case 2:
			TX_8(0x00);
			TX_8(0x01);
			TX_8(0x00);	
		break;
		case 3:
			TX_8(0x01);
			TX_8(0x01);
			TX_8(0x00);	
		break;
		case 4:
			TX_8(0x00);
			TX_8(0x00);
			TX_8(0x01);	
		break;
		case 5:
			TX_8(0x01);
			TX_8(0x00);
			TX_8(0x01);	
		break;
		case 6:
			TX_8(0x00);
			TX_8(0x01);
			TX_8(0x01);	
		break;
		case 7:
			TX_8(0x01);
			TX_8(0x01);
			TX_8(0x01);	
		break;
		default:
			TX_8(0x00);
			TX_8(0x00);
			TX_8(0x00);	
		break;
	}
	TX_8(0x00);
	END_CMD();
}

void DDC112IF_RedLedMode(uint8_t mode)
{
	BEGIN_CMD();
	TX_8(0x82);
	TX_8(mode);
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	END_CMD();
}

void DDC112IF_PurpleLedEnable(uint8_t enable)
{
	BEGIN_CMD();
	TX_8(0x83);
	TX_8(enable);
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	END_CMD();
}

void DDC112IF_SampleEnable(uint8_t enable)
{
	BEGIN_CMD();
	if(enable)
		TX_8(0x84); //��ʼ
	else
		TX_8(0x85); //ֹͣ
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	END_CMD();
}

void DDC112IF_Sample(void)
{
	BEGIN_CMD();
	TX_8(0x8A); //��һ��
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	END_CMD();
}

void DDC112IF_IntegralTimeSetting(uint8_t time)
{
	BEGIN_CMD();
	TX_8(0x86); //�޸Ļ���ʱ�� 500us��λ
	TX_8(time);
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	END_CMD();
}

void DDC112IF_BufferSizeSetting(uint8_t size)
{
	BEGIN_CMD();
	TX_8(0x87); //�޸Ļ�������С
	TX_8(size);
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	END_CMD();
}

//=================================PC=================================

#define TX1_8(P1) SEND1_DATA((P1)&0xFF)  //���͵����ֽ�
#define TX1_16(P1) TX1_8((P1)>>8);TX1_8(P1)  //����16λ����
#define TX1_32(P1) TX1_16((P1)>>16);TX1_16((P1)&0xFFFF)  //����32λ����

static void SEND1_DATA(uint8 c)
{
    USART_SendData(USART1, c);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET));//�ȴ����ڷ������
}

void DDC112IF_SendPC(uint32_t index, uint32_t data)
{
	BEGIN_CMD();
	TX1_16(0xA55A); //֡ͷ
	TX1_8(0x05);//֡����
	TX1_8(0x8A);//��ʾADC����	
	TX1_32(data);
	//TX1_16(index);//������
	END_CMD();
}

void DDC112IF_SendPCStart(void)
{
	BEGIN_CMD();
	TX1_16(0xA55A); //֡ͷ
	TX1_8(0x01);//֡����
	TX1_8(0x8B);//��ʾ��ʼADC���ݴ���
	END_CMD();
}

void DDC112IF_SendPCEnd(void)
{
	BEGIN_CMD();
	TX1_16(0xA55A); //֡ͷ
	TX1_8(0x01);//֡����
	TX1_8(0x8C);//��ʾ����ADC���ݴ���
	END_CMD();
}

#ifdef __cplusplus
}
#endif
