#ifndef __MANAGEMENT_H__
#define __MANAGEMENT_H__

#include  "common.h"

#define CONTROL_SAMPLE      0x01
#define CONTROL_REDLED      0x02
#define CONTROL_SENDDATA    0x04

#define BUFFER_SIZE 20

typedef struct
{
    uint32_t ddc112Data;

    uint32_t buffer[BUFFER_SIZE];
    uint8_t index;
    uint8_t bufferSize; //ʵ��ʹ�õĻ�������С��1 <= bufferSize <= BUFFER_SIZE
    
    uint8_t integralTime; //����ʱ�䣬us��λ

    uint8_t control;
    uint8_t dataSendFlag; //���ݷ��ͱ�־λ
    uint8_t dataValidFlag; //������Ч��־λ

    //��ʱ���ж���
    uint16_t timerReload; //��ʱ����װ��ֵ
    uint16_t timerIntCnt; //��ʱ���жϴ������������ڶ�ʱ���ȳ������ζ�ʱ�������
}Man_TypeDef;

extern Man_TypeDef man;

void InitMan(void);
void DDC112ParaSet(uint32_t arg);

#endif
