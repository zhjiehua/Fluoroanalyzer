/************************************��Ȩ����********************************************
**                             ���ݴ�ʹ��Ƽ����޹�˾
**                             http://www.gz-dc.com
**-----------------------------------�ļ���Ϣ--------------------------------------------
** �ļ�����:   hmi_user_uart.c
** �޸�ʱ��:   2011-05-18
** �ļ�˵��:   �û�MCU��������������
** ����֧�֣�  Tel: 020-82186683  Email: hmi@gz-dc.com Web:www.gz-dc.com
--------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------
                                  ʹ�ñض�
   hmi_user_uart.c�еĴ��ڷ��ͽ��պ�����3�����������ڳ�ʼ��Uartinti()������1���ֽ�SendChar()��
   �����ַ���SendStrings().����ֲ������ƽ̨����Ҫ�޸ĵײ��
   ��������,����ֹ�޸ĺ������ƣ������޷���HMI������(hmi_driver.c)ƥ�䡣
--------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------
                          1. ����STM32ƽ̨��������
----------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#include "uart.h"
#include "hmi_user_uart.h"
#include "cmd_queue.h"
    
#if 0
/****************************************************************************
* ��    �ƣ� UartInit()
* ��    �ܣ� ���ڳ�ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void UartInit(uint32 BaudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);//
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //  /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//;GPIO_Mode_IPU
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
    //USART_DeInit(UART4);//��λUSART1
    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    /* USART configuration */
    USART_Init(UART4, &USART_InitStructure);
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); //�����ж�ʹ��

	USART_ClearFlag(UART4,USART_FLAG_TC);
    /* Enable USART */
    USART_Cmd(UART4, ENABLE);
}

/*****************************************************************
* ��    �ƣ� SendChar()
* ��    �ܣ� ����1���ֽ�
* ��ڲ����� t  ���͵��ֽ�
* ���ڲ����� ��
 *****************************************************************/
void  SendChar(uchar t)
{
#if (LCDUART == 0)
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) t;
#else
	//while((UART4->SR&0X40)==0);
    USART_SendData(UART4,t);
    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET));//�ȴ����ڷ������
#endif	
}

void UART4_IRQHandler(void)                	//����4�жϷ������
{
	u8 Res;
    uint32_t oldBasePri = portSET_INTERRUPT_MASK_FROM_ISR();
        
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(UART4);//(USART1->DR);	//��ȡ���յ�������
		
		queue_push(Res);

		//USART_SendData(USART1, Res);
    } 
        
    portCLEAR_INTERRUPT_MASK_FROM_ISR(oldBasePri);
}


#else


/*----------------------------------------------------------------------------------------
	1. Qt���ģ��
----------------------------------------------------------------------------------------*/
//void  SendChar(uchar t)��������LogicThread.cpp��

/*----------------------------------------------------------------------------------------
	1. 51��Ƭ��
----------------------------------------------------------------------------------------*/
void SendChar(uchar t)
{
	Uart_SendData(t);
}
#endif

#ifdef __cplusplus
}
#endif
