#include "led.h"
#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"

#include "../StepMotor/StepMotor.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PC,PA�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED1-->PC.9 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC.9
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);						 //PC.9 �����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	    		 //LED2-->PA.8 �˿�����, �������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
	GPIO_ResetBits(GPIOA, GPIO_Pin_4); 					 //PA.8 ����� 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
 
void LED1Task(void)
{
	static uint16_t cnt;

	if(cnt++ > 1)
	{
		cnt = 0;
		LED1 = !LED1;
	}
}

void LED2Task(void)
{
	static uint16_t cnt;

	if(cnt++ > 20)
	{
		cnt = 0;
		LED2 = !LED2;
	}
}
