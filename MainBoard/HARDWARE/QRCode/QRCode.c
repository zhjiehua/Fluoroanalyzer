#include "./QRCode/QRCode.h"
#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8 reg0000;
uint8 reg0002;
uint8 reg0003;
uint8 reg0007;
uint8 reg0008;
uint8 reg000D;
uint8 reg002C;
uint8 reg0060;
uint8 reg0062;
	
#define TX_8(P1) SEND_DATA((P1)&0xFF)  //发送单个字节
#define TX_8N(P,N) SendNU8((uint8 *)P,N)  //发送N个字节
#define TX_16(P1) TX_8((P1)>>8);TX_8(P1)  //发送16位整数
#define TX_16N(P,N) SendNU16((uint16 *)P,N)  //发送N个16位整数
#define TX_32(P1) TX_16((P1)>>16);TX_16((P1)&0xFFFF)  //发送32位整数

static void SendData(uint8 c)
{
    USART_SendData(USART3, c);
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET));//等待串口发送完毕
}

#if(QRCODE_CRC16_ENABLE)

static uint16 _crc16 = 0x0;
void QRCode_AddCRC16(uint8 *buffer, uint16 n, uint16 *pcrc)
{
	uint8 i;
	while (n-- != 0)
	{
		for (i = 0x80; i != 0; i /= 2)
		{
			*pcrc *= 2;
			if ((*pcrc & 0x10000) != 0) //???CRC? 2?,????1,??? 0x11021 
				*pcrc ^= 0x11021;
			if ((*buffer & i) != 0) //?????1,??CRC = ????CRC + ??/CRC_CCITT 
				*pcrc ^= 0x1021;
		}
		buffer++;
	}
}

uint16 QRCode_CheckCRC16(uint8 *buffer, uint16 n)
{
	uint16 crc0 = 0x0;
	uint16 crc1 = 0x0;

	if (n >= 2)
	{
		crc0 = ((buffer[n - 2] << 8) | buffer[n - 1]);
		QRCode_AddCRC16(buffer, n - 2, &crc1);
	}

	return (crc0 == crc1);
}

void SEND_DATA(uint8 c)
{
	QRCode_AddCRC16(&c, 1, &_crc16);

	SendData(c);
}

void BEGIN_CMD()
{
	//TX_16(SEND_CMD_HEAD);
	SendData((uint8)(SEND_CMD_HEAD >> 8)); //CRC?????????
	SendData((uint8)SEND_CMD_HEAD);
	_crc16 = 0x0;//????CRC16
}

void END_CMD()
{
	TX_16(_crc16);//??CRC16
	//TX_16(0xABCD);
}

#else//NO CRC16   //========================================================================================

void SEND_DATA(uint8 c)
{
	SendData(c);
}

void BEGIN_CMD()
{
	TX_16(SEND_CMD_HEAD);
}

void END_CMD()
{
	TX_16(0xABCD); //crc16???0xABCD
}

#endif


void SendStrings(uint8_t *str)
{
	while(*str)
	{
		TX_8(*str);
		str++;
	}
}

void SendNU8(uint8 *pData,uint16 nDataLen)
{
	uint16 i = 0;
	for (;i<nDataLen;++i)
	{
		TX_8(pData[i]);
	}
}

void SendNU16(uint16 *pData,uint16 nDataLen)
{
	uint16 i = 0;
	for (;i<nDataLen;++i)
	{
		TX_16(pData[i]);
	}
}

/*****************************************************************************************
*
*
*
******************************************************************************************/

void QRCode_ResetDefault(void)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x00D9);
	TX_8(0x55);
	END_CMD();
}

void QRCode_Sleep(uint8 mode)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x00D9);
	if(!mode) //???
		TX_8(0xA0);
	else //???
		TX_8(0xA5);
	END_CMD();
}

void QRCode_Wakeup(void)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x00D9);
	TX_8(0x0);
	END_CMD();
}

void QRCode_SettingCode(uint8 enable)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0003);
	if(enable)
		reg0003 &= ~0x02;
	else
		reg0003 |= 0x02;
	TX_8(reg0003);
	END_CMD();
}

void QRCode_SettingCode_Output(uint8 enable)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0003);
	if (!enable)
		reg0003 &= ~0x01;
	else
		reg0003 |= 0x01;
	TX_8(reg0003);
	END_CMD();
}

void QRCode_SettingCode_Save(void)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_SAVEDATA);
	TX_8(0x01);
	TX_16(0x0000);
	TX_8(0x00);
	END_CMD();
}

void QRCode_ComType(uint8 type)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x000D);
	if (type == QRCODE_COMTYPE_UART) //????
		reg000D &= ~0x03;
	else if (type == QRCODE_COMTYPE_USB) //USB PC??
	{
		reg000D &= ~0x03;
		reg000D |= 0x01;
	}
	else //USB????
		reg000D |= 0x03;
	TX_8(reg000D);
	END_CMD();
}

void QRCode_UartBaudRate(uint8 br)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x02);
	TX_16(0x002A);
	switch (br)
	{
	case QRCODE_BAUDRATE_1200:
		TX_16(0xC409);
		break;
	case QRCODE_BAUDRATE_4800:
		TX_16(0x7102);
		break;
	case QRCODE_BAUDRATE_9600:
		TX_16(0x3901);
		break;
	case QRCODE_BAUDRATE_14400:
		TX_16(0xD000);
		break;
	case QRCODE_BAUDRATE_19200:
		TX_16(0x9C00);
		break;
	case QRCODE_BAUDRATE_38400:
		TX_16(0x4E00);
		break;
	case QRCODE_BAUDRATE_57600:
		TX_16(0x3400);
		break;
	case QRCODE_BAUDRATE_115200:
		TX_16(0x1A00);
		break;
	default:
		TX_16(0x3901);
		break;
	}
	END_CMD();
}

void QRCode_RecognizeMode(uint8 mode)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0000);
	if (mode == QRCODE_RECOGNIZEMODE_MANUAL) //????
		reg0000 &= ~0x03;
	else if (mode == QRCODE_RECOGNIZEMODE_CODETRIGER) //??????
	{
		reg0000 &= ~0x03;
		reg0000 |= 0x01;
	}
	else if (mode == QRCODE_RECOGNIZEMODE_CONTINUAL) //????
	{
		reg0000 &= ~0x03;
		reg0000 |= 0x02;
	}
	else //????
		reg0000 |= 0x03;
	TX_8(reg0000);
	END_CMD();
}

void QRCode_SingleRecognizeTime(uint8 time)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0006);
	TX_8(time);
	END_CMD();
}

void QRCode_IntervalTime(uint8 time)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0005);
	TX_8(time);
	END_CMD();
}

void QRCode_StableRecognizeTime(uint8 time)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0004);
	TX_8(time);
	END_CMD();
}

void QRCode_SleepMode(uint8 enable)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0007);
	if (!enable)
		reg0007 &= ~0x01;
	else
		reg0007 |= 0x01;
	TX_8(reg0007);
	END_CMD();
}

void QRCode_StartRecognize(void)
{
	uint8 temp = reg0002;
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0002);
	temp |= 0x01;
	TX_8(temp);
	END_CMD();
}

void QRCode_LightingMode(uint8 mode)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0000);
	switch (mode)
	{
	case QRCODE_LIGHT_NONE:
		reg0000 &= ~0x0C;
		break;
	case QRCODE_LIGHT_NORMAL:
		reg0000 &= ~0x0C;
		reg0000 |= 0x04;
		break;
	case QRCODE_LIGHT_ON:
		reg0000 |= 0x0C;
		break;
	default:
		reg0000 &= ~0x0C;
		reg0000 |= 0x04;
		break;
	}
	TX_8(reg0000);
	END_CMD();
}

void QRCode_AimMode(uint8 mode)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0000);
	switch (mode)
	{
	case QRCODE_AIM_NONE:
		reg0000 &= ~0x30;
		break;
	case QRCODE_AIM_NORMAL:
		reg0000 &= ~0x30;
		reg0000 |= 0x10;
		break;
	case QRCODE_AIM_ON:
		reg0000 |= 0x30;
		break;
	default:
		reg0000 &= ~0x30;
		reg0000 |= 0x10;
		break;
	}
	TX_8(reg0000);
	END_CMD();
}

void QRCode_Mute(uint8 enable)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0000);
	if(enable)
		reg0000 &= ~0x40;
	else
		reg0000 |= 0x40;
	TX_8(reg0000);
	END_CMD();
}

void QRCode_BeepVolume(uint8 enable)
{
	//BEGIN_CMD();
	//TX_8(SEND_CMD_TYPE_WRITE);
	//TX_8(0x01);
	//TX_16(0x0000);
	//if (!enable)
	//	reg0000 &= ~0x01;
	//else
	//	reg0000 |= 0x01;
	//TX_8(reg0000);
	//END_CMD();
}

void QRCode_RecognizeSuccessTips(uint8 enable)
{
	uint8 temp;
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x000E);
	if (!enable)
		temp &= ~0x04;
	else
		temp |= 0x04;
	TX_8(temp);
	END_CMD();
}

void QRCode_RecognizeSuccessTipsTime(uint8 time)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x000B);
	TX_8(time);
	END_CMD();
}

void QRCode_RecognizeStatusOutput(uint8 enable)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x000B);
	if (!enable)
		reg0002 &= ~0x40;
	else
		reg0002 |= 0x40;
	TX_8(reg0002);
	END_CMD();
}

void QRCode_DataEncodeType(uint8 type)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x000D);
	switch (type)
	{
	case QRCODE_ENCODE_GBK:
		reg000D &= ~0x0C;
		break;
	case QRCODE_ENCODE_UNICODE:
		reg000D &= ~0x0C;
		reg000D |= 0x04;
		break;
	case QRCODE_ENCODE_BIG5:
		reg000D &= ~0x0C;
		reg000D |= 0x08;
		break;
	default:
		reg000D &= ~0x0C;
		break;
	}
	TX_8(reg000D);
	END_CMD();
}

void QRCode_KeyBoardType(uint8 type)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0061);
	switch (type)
	{
	case QRCODE_ENCODE_GBK:
		reg000D &= ~0x0C;
		break;
	case QRCODE_ENCODE_UNICODE:
		reg000D &= ~0x0C;
		reg000D |= 0x04;
		break;
	case QRCODE_ENCODE_BIG5:
		reg000D &= ~0x0C;
		reg000D |= 0x08;
		break;
	default:
		reg000D &= ~0x0C;
		break;
	}
	TX_8(reg000D);
	END_CMD();
}

void QRCode_Model(void)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_READ);
	TX_8(0x01);
	TX_16(0x00E0);
	END_CMD();
}

void QRCode_HardwareVersion(void)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_READ);
	TX_8(0x01);
	TX_16(0x00E1);
	END_CMD();
}

void QRCode_SoftwareVersion(void)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_READ);
	TX_8(0x01);
	TX_16(0x00E2);
	END_CMD();
}

void QRCode_AddPrefix(uint8 enable)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0060);
	if (enable)
		reg0060 |= 0x08;
	else
		reg0060 &= ~0x08;
	TX_8(reg0060);
	END_CMD();
}

void QRCode_Prefix(uint8 *str)
{
	uint8 len, i;
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	len = strlen((const char*)str);
	if (len > 15) len = 15;
	TX_8(len);
	TX_16(0x0063);
	for(i=0;i<len;i++)
		TX_8(str[i]);
	END_CMD();

	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0062);
	reg0062 &= ~0xF0;
	reg0062 |= len << 4;
	TX_8(reg0062);
	END_CMD();
}

void QRCode_AddSuffix(uint8 enable)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0060);
	if (enable)
		reg0060 |= 0x02;
	else
		reg0060 &= ~0x02;
	TX_8(reg0060);
	END_CMD();
}

void QRCode_Suffix(uint8 *str)
{
	uint8 len, i;
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	len = strlen((const char*)str);
	if (len > 15) len = 15;
	TX_8(len);
	TX_16(0x0072);
	for (i = 0; i<len; i++)
		TX_8(str[i]);
	END_CMD();

	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0062);
	reg0062 &= ~0x0F;
	reg0062 |= len;
	TX_8(reg0062);
	END_CMD();
}

void QRCode_AddCodeID(uint8 enable)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0060);
	if (enable)
		reg0060 |= 0x04;
	else
		reg0060 &= ~0x04;
	TX_8(reg0060);
	END_CMD();
}

void QRCode_CodeIDResetDefault(uint8 enable)
{
	//BEGIN_CMD();
	//TX_8(SEND_CMD_TYPE_WRITE);
	//TX_8(0x01);
	//TX_16(0x0060);
	//if (enable)
	//	reg0060 |= 0x02;
	//else
	//	reg0060 &= ~0x02;
	//TX_8(reg0060);
	//END_CMD();
}

void QRCode_CodeID(uint8 id, uint8 code)
{
	//BEGIN_CMD();
	//TX_8(SEND_CMD_TYPE_WRITE);
	//TX_8(0x01);
	//TX_16(0x0062);
	//reg0062 &= ~0x0F;
	//reg0062 |= len;
	//TX_8(reg0062);
	//END_CMD();
}

void QRCode_AddTail(uint8 enable)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0060);
	if (enable)
		reg0060 |= 0x01;
	else
		reg0060 &= ~0x01;
	TX_8(reg0060);
	END_CMD();
}

void QRCode_Tail(uint8 type)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0060);
	reg0060 &= ~0x60;
	switch (type)
	{
	case QRCODE_TAIL_CR:
		break;
	case QRCODE_TAIL_CRLF:
		reg0060 |= 0x20;
		break;
	case QRCODE_TAIL_TAB:
		reg0060 |= 0x40;
		break;
	case QRCODE_TAIL_NONE:
		reg0060 |= 0x60;
		break;
	default: 
		break;
	}
	TX_8(reg0060);
	END_CMD();
}

void QRCode_DataCutOutMode(uint8 mode)
{
	uint8 temp;
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x00B0);
	switch (mode)
	{
	case QRCODE_DATA_ALL:
		temp = 0x0;
		break;
	case QRCODE_DATA_PREFIX:
		temp = 0x01;
		break;
	case QRCODE_DATA_SUFFIX:
		temp = 0x02;
		break;
	case QRCODE_DATA_MIDDLE:
		temp = 0x03;
		break;
	default:
		temp = 0x0;
		break;
	}
	TX_8(temp);
	END_CMD();
}

void QRCode_DataCutOutStart(uint8 n)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x00B1);
	TX_8(n);
	END_CMD();
}

void QRCode_DataCutOutEnd(uint8 n)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x00B2);
	TX_8(n);
	END_CMD();
}

void QRCode_AddReadFailInfo(uint8 enable)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0060);
	if (enable)
		reg0060 |= 0x10;
	else
		reg0060 &= ~0x10;
	TX_8(reg0060);
	END_CMD();
}

void QRCode_ReadFailInfo(uint8 *str)
{
	uint8 len, i;
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	len = strlen((const char*)str);
	if (len > 15) len = 15;
	TX_8(len);
	TX_16(0x0082);
	for (i = 0; i<len; i++)
		TX_8(str[i]);
	END_CMD();

	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x0081);
	TX_8(len);
	END_CMD();
}

void QRCode_RecognizeCodeTypeSwitch(uint8 type)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x002C);
	switch (type)
	{
	case QRCODE_SWITCH_NONE:
		reg002C = 0x0;
		break;
	case QRCODE_SWITCH_ALL:
		reg002C = 0x01;
		break;
	case QRCODE_SWITCH_DEFAULT:
		reg002C = 0x02;
		break;
	default:
		reg002C = 0x01;
		break;
	}
	TX_8(reg002C);
	END_CMD();
}

void QRCode_RecognizeCodeType(uint8 type, uint8 enable)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	switch (type)
	{
	case QRCODE_EAN13:
		TX_16(0x002E);
		TX_8(!!enable);
		break;
	case QRCODE_EAN8:
		TX_16(0x002F);
		TX_8(!!enable);
		break;
	case QRCODE_UPCA:
		TX_16(0x0030);
		TX_8(!!enable);
		break;
	case QRCODE_UPCE0:
		TX_16(0x0031);
		TX_8(!!enable);
		break;
	case QRCODE_UPCE1:
		TX_16(0x0032);
		TX_8(!!enable);
		break;


	default:
		break;
	}
	END_CMD();
}

void QRCode_PictureMirror(uint8 enable)
{
	BEGIN_CMD();
	TX_8(SEND_CMD_TYPE_WRITE);
	TX_8(0x01);
	TX_16(0x002C);
	if (enable)
		reg002C |= 0x01;
	else
		reg002C &= ~0x01;
	TX_8(reg002C);
	END_CMD();
}




//初始化
void QRCodeModelInit(void)
{
	uint8_t str[6] = {'\0'};
	HMIUartInit(9600);
	QRCode_ComType(QRCODE_COMTYPE_UART);
	QRCode_UartBaudRate(QRCODE_BAUDRATE_115200);
	QRCode_RecognizeMode(QRCODE_RECOGNIZEMODE_CODETRIGER); //QRCODE_RECOGNIZEMODE_CODETRIGER    QRCODE_RECOGNIZEMODE_MANUAL
	QRCode_LightingMode(QRCODE_LIGHT_NORMAL);
	QRCode_AimMode(QRCODE_AIM_NORMAL);
	
	str[0] = 0xEE; //帧头
	str[1] = 0xEF; //识别类型
	//str[2] = 0x0;
	QRCode_Prefix(str);
	QRCode_AddPrefix(1);
	str[0] = 0xFF; //帧尾
	str[1] = 0xFC;
	str[2] = 0xFF;
	str[3] = 0xFF;
	//str[5] = 0x0;
	QRCode_Suffix(str);
	QRCode_AddSuffix(1);
	QRCode_AddCodeID(0);
	QRCode_AddTail(0);
	
	QRCode_Mute(0);
	QRCode_RecognizeSuccessTips(1);
	
	QRCode_SettingCode_Save();
	HMIUartInit(115200);
}

#ifdef __cplusplus
}
#endif
