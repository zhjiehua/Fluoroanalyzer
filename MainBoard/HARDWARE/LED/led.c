#include "led.h"
#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"

#include "../StepMotor/StepMotor.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PC,PA端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED1-->PC.9 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC.9
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);						 //PC.9 输出高

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	    		 //LED2-->PA.8 端口配置, 推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	GPIO_ResetBits(GPIOA, GPIO_Pin_4); 					 //PA.8 输出高 

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
