/*
	串口1波特率最高4800
	该串口解析程序只能实现半双通信，而且接收到的数据格式必须是0x0D 0x0A结尾
	串口解析协议请看下面解析部分的说明
	串口缓冲区大小不能大于32
*/

#ifndef __UART_H__
#define __UART_H__

#include "common.h"

#define CMD_MAX_SIZE 15 //64        /*!<单条指令大小，根据需要调整，尽量设置大一些*/

#define BAUD 9600           //UART baudrate

/*Define UART parity mode*/
#define NONE_PARITY     0   //None parity
#define ODD_PARITY      1   //Odd parity
#define EVEN_PARITY     2   //Even parity
#define MARK_PARITY     3   //Mark parity
#define SPACE_PARITY    4   //Space parity

#define PARITYBIT NONE_PARITY   //Testing even parity

extern uint8_t cmd_buffer[];

void Uart_Init(void);
void Uart_SendData(uint8_t dat);
void Uart_SendString(uint8_t *s);

#endif