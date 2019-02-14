#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "LED/led.h"
#include "Key/key.h"
#include "Beep/beep.h"
#include "Button/Button.h"
#include "PhotoelectricSensor/PhSensor.h"
#include "RelayMOS/RelayMOS.h"
#include "StepMotor/StepMotor.h"
#include "WDG/WDG.h"
#include "Timer/timer.h"
#include "24CXX/24cxx.h"
#include "Printer/Printer.h"
#include "QRCode/QRCode.h"

#include "../Logic/management.h"
#include "CPrintf.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/************************************************

************************************************/
#define default_TASK_PRIORITY           ( tskIDLE_PRIORITY + 4 )
#define uart_TASK_PRIORITY           		( tskIDLE_PRIORITY + 3 )
#define project_TASK_PRIORITY           ( tskIDLE_PRIORITY + 2 )
#define ui_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )

#define default_TASK_STACKDEP						( 50 )
#define uart_TASK_STACKDEP            	( 250 )
#define project_TASK_STACKDEP           ( 250 )
#define ui_TASK_STACKDEP                ( 1024+250 )

void UartTask(void);
void ProjectTask(void);
void UITask(void);

//������    500 / portTICK_PERIOD_MS = 500ms
int main(void)
{
//	uint8_t size;
//	size = sizeof(uint64_t);
//	printf("sizeof = ", size);
	
	/*
	���ȼ�����4
	����	���ȼ�	��;
	Uart1		1	RS232�ӿ�
	EXTI0		1	Button1
	EXTI1~3		2	Button2~3
	Timer2��5	2	StepMotor
	Uart4		3	�������ӿ�
	Timer6		2	������ӡ������Ϣ
	
	SysTick		15
	PendSV		15
	SVC			0
	*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����2
	
	delay_init();
	uart_init(115200);	//��ʼ�����ڲ�����Ϊ115200
	LED_Init();			//��ʼ��LED
	Printer_Init();
	//QRCodeModelInit(); //�ֶ���ʼ�����ɣ����ﲻ��Ҫ��
	
	AT24CXX_Init();
#ifndef STM32SIM
	while(AT24CXX_Check())//��ⲻ��24c64
	{
		cDebug("EEPROM Check Failed!\n");
		delay_ms(300);
		LED1=!LED1;//DS0��˸
	}
#endif
	//Beep_Init();
	Button_Init();
	//PhSensor_Init();
	RelayMOS_Init();//
	StepMotor_Init();
	
	//bsp_SetTIMforInt(TIM6, 20000, 2, 0); //50us�ж�1��
	
	InitUI();
//	initSoftwareTimer();
	
//	pProjectMan->projectStatusSem = xSemaphoreCreateMutex();
//	xSemaphoreGive(pProjectMan->projectStatusSem);
//	pProjectMan->lcdUartSem = xSemaphoreCreateMutex();
//	xSemaphoreGive(pProjectMan->lcdUartSem);
//	pProjectMan->projectEventGroup = xEventGroupCreate();
	
//	IWDG_Init(4,625);    //���Ƶ��Ϊ64,����ֵΪ625,���ʱ��Ϊ1s
    
	cDebug("Hello Fluoroanalyzer project!\r\n");
	
	/* Start the tasks defined within this file/specific to this demo. */
	xTaskCreate( (TaskFunction_t)UITask, "UITask", ui_TASK_STACKDEP, NULL, ui_TASK_PRIORITY, NULL );
	//xTaskCreate( (TaskFunction_t)UartTask, "UartTask", uart_TASK_STACKDEP, NULL, uart_TASK_PRIORITY, NULL );
	xTaskCreate( (TaskFunction_t)ProjectTask, "ProjectTask", project_TASK_STACKDEP, NULL, project_TASK_PRIORITY, NULL );

	/* Start the scheduler. */
	vTaskStartScheduler();
	
	while(1);
	
	//return 0;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
	printf("Task: %s Stack is overflow!\r\n", pcTaskName);
}
