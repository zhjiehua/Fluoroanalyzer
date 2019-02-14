#include "uart.h"
#include "io.h"
#include "timer.h"

#include "cmd_queue.h"

//���ڻ�����
uint8 cmd_buffer[CMD_MAX_SIZE];

//��־�������ڷ�������
static bit Uart_Busy;

//���ڳ�ʼ�������ڲ����ʷ������õ��Ƕ�ʱ��1
void Uart_Init(void)
{
	//PCON |= 0x80;	//�����ʼӱ�

#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;            //8-bit variable UART
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;            //9-bit variable UART, parity bit initial to 1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;            //9-bit variable UART, parity bit initial to 0
#endif
//	TMOD |= 0x20;            //Set Timer1 as 8-bit auto reload mode
//	TH1 = TL1 = 256 - (FOSC/12/32/BAUD); //Set auto-reload vaule ��9600�������ò���
//	TR1 = 1;                //Timer1 start run
	ES = 1;                 //Enable UART interrupt
}

/*----------------------------
UART interrupt service routine
----------------------------*/
void Uart_Isr() interrupt 4 using 1
{
	uint8_t Buf = SBUF;

    if (RI)
    {
        RI = 0;             //Clear receive interrupt flag

        //SBUF = Buf; //����

		//8λ��Ƭ���жϺ������ܵ��ú���
		{
			qsize pos = (que._head+1)%QUEUE_MAX_SIZE;
			if(pos!=que._tail)//����״̬
			{
				que._data[que._head] = Buf;
				que._head = pos;
			}
		}
		//queue_push(Buf);
    }
    if (TI)
    {
        TI = 0;             //Clear transmit interrupt flag
        Uart_Busy = 0;           //Clear transmit busy flag
    }
}

/*----------------------------
Send a byte data to UART
Input: Data (data to be sent)
Output:None
----------------------------*/
void Uart_SendData(uint8_t Data)
{
    while (Uart_Busy);           //Wait for the completion of the previous data is sent
    ACC = Data;              //Calculate the even parity bit P (PSW.0)
    if (P)                  //Set the parity bit according to P
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;            //Set parity bit to 0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;            //Set parity bit to 1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;            //Set parity bit to 1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;            //Set parity bit to 0
#endif
    }
    Uart_Busy = 1;
    SBUF = ACC;             //Send data to UART buffer
}

/*----------------------------
Send a string to UART
Input: s (address of string)
Output:None
----------------------------*/
void Uart_SendString(uint8_t *s)
{
    while (*s)              //Check the end of the string
    {
        Uart_SendData(*s++);     //Send current char and increment string ptr
    }
}

//��дputchar����
char putchar(char c)
{
    Uart_SendData(c);
    return c;
}

