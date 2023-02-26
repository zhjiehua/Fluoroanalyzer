#include "sys.h"
#include "usart.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_adc.h"

#include "HardwareCommon.h"
#include "Beep/beep.h"
#include "Button/Button.h"
#include "PhotoelectricSensor/PhSensor.h"
#include "RelayMOS/RelayMOS.h"
#include "StepMotor/StepMotor.h"
#include "WDG/WDG.h"

#include "../HMI/cmd_process.h"
#include "../UILogic/pageCommon.h"

#include "CPrintf.h"

//#define SIZEOF(X)   sizeof(X)/sizeof(X[0])
	
////////////////////////////////////////////////////////////////////////////////// 	 
#include "FreeRTOS.h"
#include "task.h"
#include "../HMI/cmd_queue.h"
#include "../UILogic/pageCommon.h"
#include "../Logic/management.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_DeInit(USART1);  //��λ����1
	USART_DeInit(USART2);
	
	//================================================================================================
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������  GPIO_Mode_Out_PP  GPIO_Mode_AF_PP
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
 
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������  GPIO_Mode_IN_FLOATING
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10
	
	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

	//===============================================================================================
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������  GPIO_Mode_Out_PP  GPIO_Mode_AF_PP
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
 
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������  GPIO_Mode_IN_FLOATING
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = 9600;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	uint32_t oldBasePri = portSET_INTERRUPT_MASK_FROM_ISR();
        
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������

//		//USART_SendData(UART4, Res);
//		USART_SendData(USART1, Res);
		
#if (LCDUART == 0)	//LCDUART
#if !UART1_UART3_TRANSMIT		
		queue_push(&hmi_que, Res);
#else
		USART_SendData(USART3, Res);
#endif
#else            
//		if((USART_RX_STA&0x8000)==0)//����δ���
//        {
//            if(USART_RX_STA&0x4000)//���յ���0x0d
//            {
//                if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
//                else USART_RX_STA|=0x8000;	//��������� 
//            }
//            else //��û�յ�0X0D
//            {	
//                if(Res==0x0d)USART_RX_STA|=0x4000;
//                else
//                {
//                    USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
//                    USART_RX_STA++;
//                    if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
//                }		 
//            }
//        }

		queue_push(&user_que, Res);
#endif
	} 
	 
    portCLEAR_INTERRUPT_MASK_FROM_ISR(oldBasePri);
}

void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 Res;
    uint32_t oldBasePri = portSET_INTERRUPT_MASK_FROM_ISR();
        
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
	{
		Res = USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
		
		queue_push(&ddc112_que, Res);
	}
	 
    portCLEAR_INTERRUPT_MASK_FROM_ISR(oldBasePri);
}

uint8_t pcWriteBuffer[500];

void UartTask(void)
{
    uint16_t i;
	uint8_t rtosInfoFlag = 0;
//	uint8_t buf[2048];
	
//	for(i=2047;i>=0;i++)
//	{
//		buf[i] = i;
//		vTaskDelay(1);
//	}
	
	while(1)
	{
		vTaskDelay(10);
		
		for(i=0;i<BUTTON_COUNT;i++)
		{
				if(button[i].flag)
				{
						char s[20];
						sprintf(s, "Button%d", i);
						cDebug("%s is pressed!\n", s);
						button[i].flag = 0;
				}
		}
		
		if(USART_RX_STA&0x8000)
		{					   
			//len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			
            switch(USART_RX_BUF[0])
            {
                case 'R':  //�̵���
                    RELAY = !!(USART_RX_BUF[1]-0x30);
                break;
                case 'B':	//������
                    BEEP = !!(USART_RX_BUF[1]-0x30);
                break;
                case 'M':	//MOS��
                    MOS = !!(USART_RX_BUF[1]-0x30);
                break;
				case 'p':
					rtosInfoFlag = 1;		
				break;
                default:
                    
                break;
            }
                
			USART_RX_STA=0;
		}

		if(rtosInfoFlag)
		{
			rtosInfoFlag = 0;
			printf("=================================================\r\n");
			printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
			vTaskList((char *)&pcWriteBuffer);
			printf("%s\r\n", pcWriteBuffer);
		
			printf("\r\n������       ���м���         ʹ����\r\n");
			vTaskGetRunTimeStats((char *)&pcWriteBuffer);
			printf("%s\r\n", pcWriteBuffer);
		}		
	}
}

#endif	

