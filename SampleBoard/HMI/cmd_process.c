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
 *  \brief  消息处理流程，此处一般不需要更改
 *  \param msg 待处理消息
 *  \param size 消息长度
 */
void ProcessMessage(uint8 *msg, uint16 size)
{
	uint8 cmd_type = msg[0];//指令类型
    uint8_t range = 0;

	switch(cmd_type)
	{		
	case CMD_RANGE_SETTING://设置采集范围  0X81

        IapEraseSector(IAP_ADDRESS);

        AD_RANGE0 = msg[1];
    	AD_RANGE1 = msg[2];
    	AD_RANGE2 = msg[3];

        msg[1] ? (range |= 0x01) : (range &= ~0x01);
        msg[2] ? (range |= 0x02) : (range &= ~0x02);
        msg[3] ? (range |= 0x04) : (range &= ~0x04);

        IapProgramByte(IAP_ADDRESS, (BYTE)range);

		break;
	case CMD_REDLED_SETTING://红光LED设置  0X82
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
	case CMD_PURPLELED_SETTING://紫光LED设置  0X83
        EN_LD = msg[1] ? 1 : 0;
		break;
    case CMD_SAMPLE_START://开始采集  0X84
        man.dataSendFlag = 1;
		break;
    case CMD_SAMPLE_STOP://停止采集  0X85
        man.dataSendFlag = 0;
		break;
	case CMD_SAMPLE://读取采集值  0X8A

		break;
    case CMD_INTEGRALTIME_SETTING://设置积分时间  0X86
//        if(msg[1] < 50) msg[1] = 50;
//        else if(msg[1] > )
        man.integralTime = msg[1] ? msg[1] : 1;
//        man.integralTime *= 10;

        //printf("man.integralTime = %d\r\n", (int)man.integralTime);

		break;
    case CMD_BUFFERSIZE_SETTING://设置缓冲区大小  0X87
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
