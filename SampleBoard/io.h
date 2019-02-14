#ifndef __IO_H__
#define __IO_H__

#include "common.h"

//CONV引脚是否是用定时器反转
#define CONV_PIN_REVERSE_TIMER 1

sbit EN_LD = P1^0;

sbit AUX_LED = P3^3;
sbit A_MCU_MOD = P2^0;

//IM481
sbit A_PUL = P2^4;
sbit A_DIR = P2^5;
sbit A_RST = P2^6;

//DDC112
sbit AD_RANGE0 = P3^6;
sbit AD_RANGE1 = P3^4;
sbit AD_RANGE2 = P3^5;

sbit OUT_DXMIT1 = P2^1;
sbit OUT_SCLK1 = P2^2;
sbit OUT_CONV1 = P2^3;
sbit IN_VALID1 = P3^7;
sbit IN_DATA1 = P2^0;

#define LITTLEENDIAN  0  //1表示小端模式；0表示大端模式
//注意：STC，STM8为大端模式，这里需要把数据传给STM32和PC，这2种CPU都是小端模式，所以接收到数据后需要将数据转换成小端模式

void IO_Init(void);

#endif