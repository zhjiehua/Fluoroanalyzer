#include "beep.h"
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

//DRV8825初始化
void Drv8825_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//设置细分
	Drv8825_MicroStep_Set(MICROSTEP_1_8);
	
	DRV8825_SLEEP = 1;  //SLEEP
}

