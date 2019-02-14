#include "beep.h"

#include "FreeRTOS.h"
#include "task.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PB8Ϊ�����.��ʹ������ڵ�ʱ��		    
//��������ʼ��
void Beep_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��GPIOB�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //BEEP-->PB.8 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //�ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);	 //���ݲ�����ʼ��GPIOB.8

    GPIO_ResetBits(GPIOA, GPIO_Pin_1);//���0���رշ��������
}

/*
 * ����������
 * ������mode: 0:һֱ��
 *		 	   1:����5��
 */
void beepAlarm(uint8_t millisec)
{									   
	if(millisec == 0)
		BEEP = 1;  //һֱ��
	else
	{
        TickType_t ticks = millisec / portTICK_PERIOD_MS;
        BEEP = 1;
        
        vTaskDelay(ticks ? ticks : 1);          /* Minimum delay = 1 tick */
        
		BEEP = 0;
		vTaskDelay(1000);  //�ȴ�1000��ʱ�ӵδ�(ticks),��1s
	}
}

