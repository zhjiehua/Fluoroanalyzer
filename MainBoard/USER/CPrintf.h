#ifndef _CPRINTF_H_
#define _CPRINTF_H_

#include "usart.h"

//�����Ҫ����λ��ģ��STM32оƬ���ܣ���Ҫ�������꣬������Ҫע�������
//#define STM32SIM

//�����Ҫ����λ������STM32оƬ���ܣ�ֻ�а壬�޵���ʹ������ȣ�����Ҫ�������꣬������Ҫע�������
#define STM32DEBUG

//����1�ʹ���3֮��ת��
#define UART1_UART3_TRANSMIT 0

#ifdef STM32SIM
#define LCDUART 0	//UART1
#else
#define LCDUART	1	//UART3
//#define LCDUART 0	//UART1
#endif

#if (LCDUART != 0)
#define _CDebug
#endif

#ifdef _CDebug
	#ifdef __cplusplus
		void myCPrintf(const char *fmt, ...);
		#define cDebug(format, ...)     myCPrintf(format, ##__VA_ARGS__)
		//#define cDebug(format, args...)    printf(format, ##args)
	#else
		#define cDebug printf
	#endif
#else
	#ifdef __cplusplus
		#define cDebug(format, ...)
	#else
		//#define cDebug uart_printf_none
		#define cDebug
	#endif
#endif
#endif
