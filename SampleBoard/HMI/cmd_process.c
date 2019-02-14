#include "hmi_driver.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "stdio.h"

#include "uart.h"
#include "stepmotor.h"
#include "io.h"
#include "eeprom.h"

#include "misc.h"
#include "management.h"


#ifdef __cplusplus
extern "C" {
#endif

/*! 
 *  \brief  ��Ϣ�������̣��˴�һ�㲻��Ҫ����
 *  \param msg ��������Ϣ
 *  \param size ��Ϣ����
 */
void ProcessMessage(uint8 *msg, uint16 size)
{
	uint8 cmd_type = msg[0];//ָ������
    uint8_t range = 0;

	switch(cmd_type)
	{		
	case CMD_RANGE_SETTING://���òɼ���Χ  0X81

        IapEraseSector(IAP_ADDRESS);

        AD_RANGE0 = msg[1];
    	AD_RANGE1 = msg[2];
    	AD_RANGE2 = msg[3];

        msg[1] ? (range |= 0x01) : (range &= ~0x01);
        msg[2] ? (range |= 0x02) : (range &= ~0x02);
        msg[3] ? (range |= 0x04) : (range &= ~0x04);

        IapProgramByte(IAP_ADDRESS, (BYTE)range);

		break;
	case CMD_REDLED_SETTING://���LED����  0X82
        if(msg[1] == 0)
        {
            AUX_LED = 1;
            man.control &= ~CONTROL_REDLED;
        }
        else if(msg[1] == 1)
        {
            AUX_LED = 0;
            man.control &= ~CONTROL_REDLED;
        }
        else
        {
            man.control |= CONTROL_REDLED;
            //Uart_SendData(0xaa);
        }
		break;
	case CMD_PURPLELED_SETTING://�Ϲ�LED����  0X83
        EN_LD = msg[1] ? 1 : 0;
		break;
    case CMD_SAMPLE_START://��ʼ�ɼ�  0X84
        man.dataSendFlag = 1;
		break;
    case CMD_SAMPLE_STOP://ֹͣ�ɼ�  0X85
        man.dataSendFlag = 0;
		break;
	case CMD_SAMPLE://��ȡ�ɼ�ֵ  0X8A

		break;
    case CMD_INTEGRALTIME_SETTING://���û���ʱ��  0X86
//        if(msg[1] < 50) msg[1] = 50;
//        else if(msg[1] > )
        man.integralTime = msg[1] ? msg[1] : 1;
//        man.integralTime *= 10;

        //printf("man.integralTime = %d\r\n", (int)man.integralTime);

		break;
    case CMD_BUFFERSIZE_SETTING://���û�������С  0X87
        if(msg[1] > BUFFER_SIZE)
            msg[1] = BUFFER_SIZE;
        man.bufferSize = msg[1] ? msg[1] : 1;
		break;
	default:
		break;
	}
}

#ifdef __cplusplus
}
#endif
