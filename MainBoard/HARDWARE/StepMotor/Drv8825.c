#include "beep.h"
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

#include "sys.h"
#include "StepMotor/Drv8825.h"
#include "Beep/beep.h"
#include "LED/LED.h"
#include "usart.h"
#include "PhotoelectricSensor/PhSensor.h"
#include "stdlib.h"
#include "CPrintf.h"
#include "../Logic/management.h"

#include "FreeRTOS.h"
#include "task.h"

void Drv8825_MicroStep_Set(uint8_t microStep)
{
	switch(microStep)
	{
		case MICROSTEP_FULL:
			DRV8825_MODE0 = 0;
			DRV8825_MODE1 = 0;
			DRV8825_MODE2 = 0;
		break;
		case MICROSTEP_1_2:
			DRV8825_MODE0 = 1;
			DRV8825_MODE1 = 0;
			DRV8825_MODE2 = 0;
		break;
		case MICROSTEP_1_4:
			DRV8825_MODE0 = 0;
			DRV8825_MODE1 = 1;
			DRV8825_MODE2 = 0;
		break;
		case MICROSTEP_1_8:
			DRV8825_MODE0 = 1;
			DRV8825_MODE1 = 1;
			DRV8825_MODE2 = 0;
		break;
		case MICROSTEP_1_16:
			DRV8825_MODE0 = 0;
			DRV8825_MODE1 = 0;
			DRV8825_MODE2 = 1;
		break;
		case MICROSTEP_1_32:
			DRV8825_MODE0 = 1;
			DRV8825_MODE1 = 0;
			DRV8825_MODE2 = 1;
		break;
		default:
			DRV8825_MODE0 = 1;
			DRV8825_MODE1 = 0;
			DRV8825_MODE2 = 1;
		break;
	}
}

//DRV8825��ʼ��
void Drv8825_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //�ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//����ϸ��
	Drv8825_MicroStep_Set(MICROSTEP_1_8);
	
	DRV8825_SLEEP = 1;  //SLEEP
}

