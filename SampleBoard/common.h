#ifndef __COMMON_H__
#define __COMMON_H__

//#include <REG52.H>                /* special function register declarations   */
//                                  /* for the intended 8051 derivative         */
//#include "STC89C5xRC_RDP.H"
#include "stc15f2k60s2.h"
#include "intrins.h"

//typedef unsigned char BYTE; //sizeof(BYTE) = 1
//typedef unsigned int WORD;  //sizeof(WORD) = 2

typedef unsigned char uint8_t;
typedef          char int8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

//----------------------------------------------- 
/* define constants */
//#define FOSC 11059200L      //System frequency
#define FOSC 24000000L      //System frequency

sfr IP0			= 0xB8; //中断优先级低位 PPCA  PLVD  PADC  PS   PT1  PX1  PT0  PX0   0000,0000
sfr WAKE_CLKO	= 0x8F; //PCAWAKEUP  RXD_PIN_IE  T1_PIN_IE  T0_PIN_IE  LVD_WAKE  _  T1CLKO  T0CLKO  0000,0000B
    //b7 - PCAWAKEUP : PCA 中断可唤醒 powerdown。
    //b6 - RXD_PIN_IE : 当 P3.0(RXD) 下降沿置位 RI 时可唤醒 powerdown(必须打开相应中断)。
    //b5 - T1_PIN_IE : 当 T1 脚下降沿置位 T1 中断标志时可唤醒 powerdown(必须打开相应中断)。
    //b4 - T0_PIN_IE : 当 T0 脚下降沿置位 T0 中断标志时可唤醒 powerdown(必须打开相应中断)。
    //b3 - LVD_WAKE : 当 CMPIN 脚低电平置位 LVD 中断标志时可唤醒 powerdown(必须打开相应中断)。
    //b2 - 
    //b1 - T1CLKO : 允许 T1CKO(P3.5) 脚输出 T1 溢出脉冲，Fck1 = 1/2 T1 溢出率
    //b0 - T0CLKO : 允许 T0CKO(P3.4) 脚输出 T0 溢出脉冲，Fck0 = 1/2 T1 溢出率
sfr P4SW 		= 0xBB; //Port-4 switch	  -  LVD_P4.6 ALE_P4.5 NA_P4.4  -   -   -   -	  x000,xxxx

#endif