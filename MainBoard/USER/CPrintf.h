#ifndef _CPRINTF_H_
#define _CPRINTF_H_

#include "usart.h"

//如果需要在上位机模拟STM32芯片功能，需要添加这个宏，否则需要注释这个宏
//#define STM32SIM

//如果需要在上位机调试STM32芯片功能（只有板，无电机和传感器等），需要添加这个宏，否则需要注释这个宏
#define STM32DEBUG

//串口1和串口3之间转发
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
