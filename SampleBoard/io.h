#ifndef __IO_H__
#define __IO_H__

#include "common.h"

//CONV�����Ƿ����ö�ʱ����ת
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

#define LITTLEENDIAN  0  //1��ʾС��ģʽ��0��ʾ���ģʽ
//ע�⣺STC��STM8Ϊ���ģʽ��������Ҫ�����ݴ���STM32��PC����2��CPU����С��ģʽ�����Խ��յ����ݺ���Ҫ������ת����С��ģʽ

void IO_Init(void);

#endif