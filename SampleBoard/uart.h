/*
	����1���������4800
	�ô��ڽ�������ֻ��ʵ�ְ�˫ͨ�ţ����ҽ��յ������ݸ�ʽ������0x0D 0x0A��β
	���ڽ���Э���뿴����������ֵ�˵��
	���ڻ�������С���ܴ���32
*/

#ifndef __UART_H__
#define __UART_H__

#include "common.h"

#define CMD_MAX_SIZE 15 //64        /*!<����ָ���С��������Ҫ�������������ô�һЩ*/

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