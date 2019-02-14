#ifndef __TIMER_H__
#define __TIMER_H__

#include "common.h"


void Timer0_Init(void);
void Timer1_Init(void);

void Delay1us();
void Delay10us();
void Delay100us();
void Delay500us();
void Delay1ms();
void Delay5ms();
void Delay10ms();
void Delay100ms();


void Timer_Delay100ns(uint8_t n);

#endif