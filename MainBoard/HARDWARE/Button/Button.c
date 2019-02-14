#include "Button.h"
#include "usart.h"
#include "stm32f10x_exti.h"
#include "PhotoelectricSensor/PhSensor.h"
#include "../Logic/management.h"
#include "../StepMotor/StepMotor.h"
#include "../RelayMOS/RelayMOS.h"
#include "../WDG/WDG.h"
#include "../HARDWARE/24CXX/24cxx.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//蜂鸣器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

Button_TypeDef button[BUTTON_COUNT];

//初始化PB8为输出口.并使能这个口的时钟		    
//蜂鸣器初始化
void Button_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);	 //使能GPIOB端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//BEEP-->PB.8 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);	 //根据参数初始化GPIOB.8
    
    // NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //外部中断配置
    EXTI_ClearITPendingBit(EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15);
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);
    EXTI_InitStructure.EXTI_Line = EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15; 
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
    EXTI_Init(&EXTI_InitStructure);
}

void EXTI15_10_IRQHandler(void)
{
	//uint8_t i;
	uint32_t oldBasePri = portSET_INTERRUPT_MASK_FROM_ISR();
	
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == SET)
	{
    button[0].flag = 1;
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
	else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == SET)
	{
    button[1].flag = 1;
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
	else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == SET)
	{
    button[2].flag = 1;
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
	else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == SET)
	{
    button[3].flag = 1;
		EXTI_ClearITPendingBit(EXTI_Line15);
	}

	portCLEAR_INTERRUPT_MASK_FROM_ISR(oldBasePri);
}

