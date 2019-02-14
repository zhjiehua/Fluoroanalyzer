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
    uint8_t bufferSize; //实际使用的缓冲区大小，1 <= bufferSize <= BUFFER_SIZE
    
    uint8_t integralTime; //积分时间，us单位

    uint8_t control;
    uint8_t dataSendFlag; //数据发送标志位
    uint8_t dataValidFlag; //数据有效标志位

    //定时器中断用
    uint16_t timerReload; //定时器重装载值
    uint16_t timerIntCnt; //定时器中断次数计数，用于定时长度超出单次定时周期情况
}Man_TypeDef;

extern Man_TypeDef man;

void InitMan(void);
void DDC112ParaSet(uint32_t arg);

#endif
