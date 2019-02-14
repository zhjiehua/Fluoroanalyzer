#include "uart.h"
#include "timer.h"
#include "io.h"
#include "eeprom.h"
#include "stepmotor.h"

#include "HMI/hmi_driver.h"
#include "HMI/cmd_queue.h"
#include "HMI/cmd_process.h"

#include "management.h"
#include "ddc112.h"
#include "misc.h"

#include <stdio.h>

//-----------------------------------------------

/* main program */
void main()
{
	uint8_t size;
	
	uint32_t testData = 0x11223344;
		
	//---------------��ʼ��------------------------------------
	IO_Init();
	Timer0_Init(); //��ʱ����ʼ��
//	Timer1_Init();
	
	Uart_Init();//���ڳ�ʼ��

	InitMan();
	queue_reset();

	//WDT_CONTR = 0x3C;//�����Ź������ʱ��1.1377s

    EA = 1;//����ȫ���ж�

	Uart_SendString("helloDDC112!\r\n");

#if CONV_PIN_REVERSE_TIMER    
    TR0 = 1;
#endif

    while (1)                       //loop
	{
		//ι��
		//WDT_CONTR = 0x3C;

		//������������
		size = queue_find_cmd(cmd_buffer, CMD_MAX_SIZE); //�ӻ������л�ȡһ��ָ��        
		if(size>0)//���յ�ָ��
		{
			ProcessMessage(cmd_buffer, size);//ָ���
		}

        if(man.dataValidFlag)
        {
            if(!DDC112_Read(man.buffer+man.index))
            {
                man.index++;
                if(man.index >= man.bufferSize)
                {
                    man.index = 0;
                    Sort(man.buffer, man.bufferSize);
                    man.ddc112Data = GetAverage(man.buffer, man.bufferSize, man.bufferSize/4);
    
                    if(man.dataSendFlag)
                        DDC112_SendData(0x8A, (uint8_t*)(&man.ddc112Data), 4);
                }
                man.dataValidFlag = 0;
            }
            else
                //printf("error!\r\n");
                Uart_SendString("error!\r\n");

            //AUX_LED = !AUX_LED;
        }

        if(man.control&CONTROL_REDLED)
        {
            if(man.control&CONTROL_SAMPLE)
            {
                AUX_LED = !AUX_LED;
            }
            else
            {
                static uint16_t cnt = 0;
                if(cnt++ > 5000)
                {
                    cnt = 0;
    		        AUX_LED = !AUX_LED;
                }
            }
        }
	}
}

