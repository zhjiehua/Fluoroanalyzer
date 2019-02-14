#ifndef _HMI_QR_CODE_
#define _HMI_QR_CODE_

#define QRCODE_CRC16_ENABLE 	0

#define SEND_CMD_HEAD			0x7E00
#define RECEIVE_CMD_HEAD		0x0200

#define SEND_CMD_TYPE_READ		0x07
#define SEND_CMD_TYPE_WRITE		0x08
#define SEND_CMD_TYPE_SAVEDATA	0x09

#define RECEIVE_CMD_TYPE_SUCCESS	0x00

//???
#define QRCODE_BAUDRATE_1200	0
#define QRCODE_BAUDRATE_4800	1
#define QRCODE_BAUDRATE_9600	2
#define QRCODE_BAUDRATE_14400	3
#define QRCODE_BAUDRATE_19200	4
#define QRCODE_BAUDRATE_38400	5
#define QRCODE_BAUDRATE_57600	6
#define QRCODE_BAUDRATE_115200	7

//????
#define QRCODE_COMTYPE_UART					0
#define QRCODE_COMTYPE_USB					1
#define QRCODE_COMTYPE_USBVIRTUALUART		2

//????
#define QRCODE_RECOGNIZEMODE_MANUAL			0
#define QRCODE_RECOGNIZEMODE_CODETRIGER		1
#define QRCODE_RECOGNIZEMODE_CONTINUAL		2
#define QRCODE_RECOGNIZEMODE_REACTIION		3

//????
#define QRCODE_LIGHT_NONE					0
#define QRCODE_LIGHT_NORMAL					1
#define QRCODE_LIGHT_ON						2

//????
#define QRCODE_AIM_NONE						0
#define QRCODE_AIM_NORMAL					1
#define QRCODE_AIM_ON						2

//????
#define QRCODE_ENCODE_GBK					0
#define QRCODE_ENCODE_UNICODE				1
#define QRCODE_ENCODE_BIG5					2

//????
#define QRCODE_KEYBOARD_AMERICA				0
#define QRCODE_KEYBOARD_CZECH				1
#define QRCODE_KEYBOARD_FRANCE				2
#define QRCODE_KEYBOARD_GERMANY				3
#define QRCODE_KEYBOARD_HUNGARY				4
#define QRCODE_KEYBOARD_ITALY				5
#define QRCODE_KEYBOARD_JAPAN				6
#define QRCODE_KEYBOARD_SPAIN				7

//???
#define QRCODE_TAIL_CR						0 
#define QRCODE_TAIL_CRLF					1 
#define QRCODE_TAIL_TAB						2 
#define QRCODE_TAIL_NONE					3 

//data??????
#define QRCODE_DATA_ALL						0 
#define QRCODE_DATA_PREFIX					1 
#define QRCODE_DATA_SUFFIX					2 
#define QRCODE_DATA_MIDDLE					3

//???????
#define QRCODE_SWITCH_NONE					0 
#define QRCODE_SWITCH_ALL					1 
#define QRCODE_SWITCH_DEFAULT				2 

//?????
#define QRCODE_EAN13						0 
#define QRCODE_EAN8							1 
#define QRCODE_UPCA							2
#define QRCODE_UPCE0						3
#define QRCODE_UPCE1						4
//...


#include "../HMI/hmi_user_uart.h"

#include "string.h"

#if(QRCODE_CRC16_ENABLE)
void QRCode_AddCRC16(uint8 *buffer, uint16 n, uint16 *pcrc);

/*!
*  \brief  ????????CRC16??
*  \param buffer ??????,????CRC16
*  \param n ????,??CRC16
*  \return ??????1,????0
*/
uint16 QRCode_CheckCRC16(uint8 *buffer, uint16 n);

#endif

//void SEND_DATA(uint8 c);
//void BEGIN_CMD();
//void END_CMD();
void SendNU8(uint8 *pData, uint16 nDataLen);
void SendNU16(uint16 *pData, uint16 nDataLen);

void SendStrings(uint8 *str);

//????
//????
void QRCode_ResetDefault(void); //??????
void QRCode_Sleep(uint8 mode); //??????
void QRCode_Wakeup(void); //??
void QRCode_SettingCode(uint8 enable); //?????
void QRCode_SettingCode_Output(uint8 enable); //???????
void QRCode_SettingCode_Save(void); //?????

//????
void QRCode_ComType(uint8 type);//????,???usb?usb????
void QRCode_UartBaudRate(uint8 br); //?????????

//????
void QRCode_RecognizeMode(uint8 mode); //????,?????????????????????
void QRCode_SingleRecognizeTime(uint8 time);//?????? 0.1s??
void QRCode_IntervalTime(uint8 time);//??????
void QRCode_StableRecognizeTime(uint8 time);//????,??????
void QRCode_SleepMode(uint8 enable);//??/??????,??????
void QRCode_StartRecognize(void);//????,????????

//?????
void QRCode_LightingMode(uint8 mode);//????
void QRCode_AimMode(uint8 mode);//????

//????
void QRCode_Mute(uint8 enable);//????
void QRCode_BeepVolume(uint8 enable);//?????
void QRCode_RecognizeSuccessTips(uint8 enable);//????????
void QRCode_RecognizeSuccessTipsTime(uint8 time);//?????????
void QRCode_RecognizeStatusOutput(uint8 enable);//?????????
void QRCode_DataEncodeType(uint8 type);//??????,GBK?UNICODE?BIG5
void QRCode_KeyBoardType(uint8 type);//????
void QRCode_Model(void);//????
void QRCode_HardwareVersion(void);//????????
void QRCode_SoftwareVersion(void);//????????

//????
void QRCode_AddPrefix(uint8 enable);//????
void QRCode_Prefix(uint8 *str);//????
void QRCode_AddSuffix(uint8 enable);//????
void QRCode_Suffix(uint8 *str);//????
void QRCode_AddCodeID(uint8 enable);//??CodeID
void QRCode_CodeIDResetDefault(uint8 enable);//CodeID?????
void QRCode_CodeID(uint8 id, uint8 code);//??CodeID??
void QRCode_AddTail(uint8 enable);//???
void QRCode_Tail(uint8 type);//?????
void QRCode_DataCutOutMode(uint8 mode);//data????????
void QRCode_DataCutOutStart(uint8 n);//data????????
void QRCode_DataCutOutEnd(uint8 n);//data????????
void QRCode_AddReadFailInfo(uint8 enable);//RF??????
void QRCode_ReadFailInfo(uint8 *str);//??RF??

//??????
void QRCode_RecognizeCodeTypeSwitch(uint8 type); //??????
void QRCode_RecognizeCodeType(uint8 type, uint8 enable);//??????
void QRCode_PictureMirror(uint8 enable);//???????



void QRCodeModelInit(void);
	
#endif
