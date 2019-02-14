#ifndef _USER_UART__
#define _USER_UART__

//#include "stm32f10x_it.h"     //�����û�MCU�����޸�

#define uchar    unsigned char
#define uint8    unsigned char
#define uint16   unsigned short int
#define uint32   unsigned int
#define int16    short int
#define int32    int

/****************************************************************************
* ��    �ƣ� HMIUartInit()
* ��    �ܣ� ���ڳ�ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void HMIUartInit(uint32 Baudrate);

/*****************************************************************
* ��    �ƣ� HMISendChar()
* ��    �ܣ� ����1���ֽ�
* ��ڲ����� t  ���͵��ֽ�
* ���ڲ����� ��
 *****************************************************************/
void  HMISendChar(uchar t);


#endif
