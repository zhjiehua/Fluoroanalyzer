#ifndef _USER_UART__
#define _USER_UART__

//#include "stm32f10x_it.h"     //根据用户MCU进行修改

#define uchar    unsigned char
#define uint8    unsigned char
#define uint16   unsigned short int
#define uint32   unsigned int
#define int16    short int
#define int32    int

/****************************************************************************
* 名    称： HMIUartInit()
* 功    能： 串口初始化
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void HMIUartInit(uint32 Baudrate);

/*****************************************************************
* 名    称： HMISendChar()
* 功    能： 发送1个字节
* 入口参数： t  发送的字节
* 出口参数： 无
 *****************************************************************/
void  HMISendChar(uchar t);


#endif
