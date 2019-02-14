#ifndef __HARDWARECOMMON_H
#define __HARDWARECOMMON_H	 
#include "sys.h"
#include "stm32f10x_exti.h"
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
#include "stm32f10x.h"

#define SIZEOF(X)   sizeof(X)/sizeof(X[0])

typedef FunctionalState Status;

typedef enum
{
	CW = 0,	//��ת
	CCW,	//��ת
}Direction_TypeDef;

typedef struct
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
    GPIOMode_TypeDef GPIO_Mode;
    uint8_t NVIC_IRQChannelPreemptionPriority;
    uint8_t NVIC_IRQChannelSubPriority;
    EXTITrigger_TypeDef EXTI_Trigger;
    
    char *name;
    void (*CallBackFun)(void *para); //�����жϺ�Ļص�����
}ExtiConfig_Typedef;
		 				    
#endif

