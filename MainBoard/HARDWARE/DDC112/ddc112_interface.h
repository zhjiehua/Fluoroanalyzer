#ifndef _DDC112_INTERFACE_H_
#define _DDC112_INTERFACE_H_

#define CMD_MAX_SIZE 30 //64        /*!<����ָ���С��������Ҫ�������������ô�һЩ*/
#define QUEUE_MAX_SIZE 90 //128  /*!< ָ����ջ�������С��������Ҫ�������������ô�һЩ*/

#include "../HMI/hmi_user_uart.h"
#include "stdint.h"

//���ܺ���
void DDC112IF_RangeSetting(uint8_t range);
void DDC112IF_RedLedMode(uint8_t mode);
void DDC112IF_PurpleLedEnable(uint8_t enable);
void DDC112IF_SampleEnable(uint8_t enable);
void DDC112IF_Sample(void);
void DDC112IF_IntegralTimeSetting(uint8_t time);
void DDC112IF_BufferSizeSetting(uint8_t size);

void DDC112IF_SendPC(uint32_t index, uint32_t data);
void DDC112IF_SendPCStart(void);
void DDC112IF_SendPCEnd(void);

#endif
