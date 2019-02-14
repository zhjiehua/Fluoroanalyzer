#include "./DDC112/ddc112_interface.h"
#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TX_8(P1) SEND_DATA((P1)&0xFF)  //发送单个字节
#define TX_8N(P,N) SendNU8((uint8 *)P,N)  //发送N个字节
#define TX_16(P1) TX_8((P1)>>8);TX_8(P1)  //发送16位整数
#define TX_16N(P,N) SendNU16((uint16 *)P,N)  //发送N个16位整数
#define TX_32(P1) TX_16((P1)>>16);TX_16((P1)&0xFFFF)  //发送32位整数

static void SEND_DATA(uint8 c)
{
    USART_SendData(USART2, c);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET));//等待串口发送完毕
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
		TX_8(0x84); //开始
	else
		TX_8(0x85); //停止
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	END_CMD();
}

void DDC112IF_Sample(void)
{
	BEGIN_CMD();
	TX_8(0x8A); //读一次
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	END_CMD();
}

void DDC112IF_IntegralTimeSetting(uint8_t time)
{
	BEGIN_CMD();
	TX_8(0x86); //修改积分时间 500us单位
	TX_8(time);
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	END_CMD();
}

void DDC112IF_BufferSizeSetting(uint8_t size)
{
	BEGIN_CMD();
	TX_8(0x87); //修改缓冲区大小
	TX_8(size);
	TX_8(0x00);
	TX_8(0x00);
	TX_8(0x00);
	END_CMD();
}

//=================================PC=================================

#define TX1_8(P1) SEND1_DATA((P1)&0xFF)  //发送单个字节
#define TX1_16(P1) TX1_8((P1)>>8);TX1_8(P1)  //发送16位整数
#define TX1_32(P1) TX1_16((P1)>>16);TX1_16((P1)&0xFFFF)  //发送32位整数

static void SEND1_DATA(uint8 c)
{
    USART_SendData(USART1, c);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET));//等待串口发送完毕
}

void DDC112IF_SendPC(uint32_t index, uint32_t data)
{
	BEGIN_CMD();
	TX1_16(0xA55A); //帧头
	TX1_8(0x05);//帧长度
	TX1_8(0x8A);//表示ADC数据	
	TX1_32(data);
	//TX1_16(index);//横坐标
	END_CMD();
}

void DDC112IF_SendPCStart(void)
{
	BEGIN_CMD();
	TX1_16(0xA55A); //帧头
	TX1_8(0x01);//帧长度
	TX1_8(0x8B);//表示开始ADC数据传输
	END_CMD();
}

void DDC112IF_SendPCEnd(void)
{
	BEGIN_CMD();
	TX1_16(0xA55A); //帧头
	TX1_8(0x01);//帧长度
	TX1_8(0x8C);//表示结束ADC数据传输
	END_CMD();
}

#ifdef __cplusplus
}
#endif
