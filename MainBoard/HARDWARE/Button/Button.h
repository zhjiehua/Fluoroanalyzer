#ifndef __BUTTON_H
#define __BUTTON_H	 
#include "sys.h"
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

#define BUTTON_COUNT 4	//	   

typedef struct
{
    uint8_t flag; //��־�Ƿ������ж�
    void (*CallBackFun)(void *para); //�����жϺ�Ļص�����
}Button_TypeDef;

extern Button_TypeDef button[BUTTON_COUNT];

void Button_Init(void);	//��ʼ��
		 				    
#endif

