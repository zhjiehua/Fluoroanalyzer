#include "./Printer/Printer.h"
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

static void SendStrings(uint8_t *str)
{
	while(*str)
	{
		TX_8(*str);
		str++;
	}
}

//static void SendNU8(uint8 *pData,uint16 nDataLen)
//{
//	uint16 i = 0;
//	for (;i<nDataLen;++i)
//	{
//		TX_8(pData[i]);
//	}
//}

//static void SendNU16(uint16 *pData,uint16 nDataLen)
//{
//	uint16 i = 0;
//	for (;i<nDataLen;++i)
//	{
//		TX_16(pData[i]);
//	}
//}

/*****************************************************************************************
*
*
*
******************************************************************************************/

void Printer_SendStrings(uint8_t *str)
{
	BEGIN_CMD();
	SendStrings(str);
	END_CMD();
}

void Printer_PrintLF(void)
{
	BEGIN_CMD();
	TX_8(0x0A);
	END_CMD();
}

void Printer_PrintReturn(void)
{
	BEGIN_CMD();
	TX_8(0x0D);
	END_CMD();
}

void Printer_Init(void)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x40);
	END_CMD();
}

void Printer_PrintPaperFeed(uint8_t n)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x4A);
	TX_8(n);
	END_CMD();
}

void Printer_PrintPaperSkipNLine(uint8_t n) //
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x64);
	TX_8(n);
	END_CMD();
}

void Printer_ModeSetting(uint8_t mode)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x21);
	TX_8(mode);
	END_CMD();
}

void Printer_FontSelect(uint8_t font)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x4D);
	TX_8(font);
	END_CMD();
}

void Printer_ChineseModeSetting(void)
{
	BEGIN_CMD();
	TX_8(0x1C);
	TX_8(0x26);
	END_CMD();
}

void Printer_ChineseModeCancel(void)
{
	BEGIN_CMD();
	TX_8(0x1C);
	TX_8(0x2E);
	END_CMD();
}

void Printer_AlignTypeSelect(uint8_t type)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x61);
	TX_8(type);
	END_CMD();
}

void Printer_DefaultLineSpaceSetting(void)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x32);
	END_CMD();
}

void Printer_LineSpaceSetting(uint8_t n)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x33);
	TX_8(n);
	END_CMD();
}

void Printer_CharRightSpaceSetting(uint8_t n)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x20);
	TX_8(n);
	END_CMD();
}

void Printer_LeftSpaceSetting(uint16_t n)
{
	BEGIN_CMD();
	TX_8(0x1D);
	TX_8(0x4C);
	TX_16(n);
	END_CMD();
}

void Printer_PrintRangeWidthSetting(uint16_t width)
{
	BEGIN_CMD();
	TX_8(0x1D);
	TX_8(0x57);
	TX_16(width);
	END_CMD();
}

void Printer_CharSizeSetting(uint8_t size)
{
	BEGIN_CMD();
	TX_8(0x1D);
	TX_8(0x21);
	TX_8(size);
	END_CMD();
}

void Printer_PrintGrayLevelSetting(uint8_t grayLevel)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x6D);
	TX_8(grayLevel);
	END_CMD();
}

void Printer_PrintInverseSetting(uint8_t flag)
{
	BEGIN_CMD();
	TX_8(0x1D);
	TX_8(0x42);
	TX_8(flag);
	END_CMD();
}

void Printer_PrintBottonUpSetting(uint8_t flag)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x7B);
	TX_8(flag);
	END_CMD();
}

void Printer_PrintUnderlineSetting(uint8_t flag)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x2D);
	TX_8(flag);
	END_CMD();
}

void Printer_StatusSend(uint8_t status)
{
	BEGIN_CMD();
	TX_8(0x1C);
	TX_8(0x76);
	TX_8(status);
	END_CMD();
}

void Printer_BarCodeWidthSetting(uint8_t width)
{
	BEGIN_CMD();
	TX_8(0x1D);
	TX_8(0x77);
	TX_8(width);
	END_CMD();
}

void Printer_BarCodeHeightSetting(uint8_t height)
{
	BEGIN_CMD();
	TX_8(0x1D);
	TX_8(0x68);
	TX_8(height);
	END_CMD();
}

void Printer_HRICharPositionSetting(uint8_t position)
{
	BEGIN_CMD();
	TX_8(0x1D);
	TX_8(0x48);
	TX_8(position);
	END_CMD();
}

void Printer_BarCodePrint(uint8_t barCodeType, uint8_t dataLen, uint8_t *data)
{
	uint8_t i;
	BEGIN_CMD();
	TX_8(0x1D);
	TX_8(0x6B);
	TX_8(barCodeType);
	TX_8(dataLen);
	for(i=0;i<dataLen;i++)
		TX_8(data[i]);
	END_CMD();
}

void Printer_BitmapModeSelect(uint8_t bitmapMode, uint16_t dataLen, uint8_t *data)
{
	uint8_t i;
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x2A);
	TX_8(bitmapMode);
	TX_16(dataLen);
	for (i = 0; i<dataLen; i++)
		TX_8(data[i]);
	END_CMD();
}

void Printer_NVBitmapDownload(uint8_t bitmapCount, uint8_t *data)
{
	//uint8_t i;
	//uint16_t x, y;
	//BEGIN_CMD();
	//TX_8(0x1C);
	//TX_8(0x71);
	//TX_8(bitmapCount);
	//for (i = 0; i < bitmapCount; i++)
	//{

	//	TX_8(data[i]);
	//}
	//END_CMD();
}

void Printer_NVBitmapPrint(uint8_t bitmapMode, uint8_t index)
{
	//uint8_t i;
	//uint16_t x, y;
	//BEGIN_CMD();
	//TX_8(0x1C);
	//TX_8(0x71);
	//TX_8(bitmapCount);
	//for (i = 0; i < bitmapCount; i++)
	//{

	//	TX_8(data[i]);
	//}
	//END_CMD();
}

void Printer_PattenBitmapPrint(uint8_t bitmapMode, uint16_t xCount, uint16_t yCount, uint8_t *data)
{
	uint8_t i;
	uint32_t dataLen = xCount * yCount;
	BEGIN_CMD();
	TX_8(0x1D);
	TX_8(0x76);
	TX_8(0x30);
	TX_8(bitmapMode);
	TX_16(xCount);
	TX_16(yCount);
	for (i = 0; i<dataLen; i++)
		TX_8(data[i]);
	END_CMD();
}

void Printer_CharSet1Select(void)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x36);
	END_CMD();
}

void Printer_CharSet2Select(void)
{
	BEGIN_CMD();
	TX_8(0x1B);
	TX_8(0x37);
	END_CMD();
}

void Printer_CursorSelect(uint8_t flag)
{
	BEGIN_CMD();
	TX_8(0x1C);
	TX_8(0x72);
	TX_8(flag);
	END_CMD();
}

#ifdef __cplusplus
}
#endif
