#include "io.h"
#include "uart.h"
#include "misc.h"
#include "timer.h"
#include "eeprom.h"
#include "management.h"

//IO初始化
void IO_Init(void)
{
    uint8_t range = 0;
	//IO--------------------------------------------------------------------------------------
//	P0M1 = 0x00;    //双双双双双推推推
//	P0M0 = 0x07;
//	P0 = 0xFE;    //-,-,-,-,IN_DATA2,OUT_RST,OUT_FDS,OUT_RSET
	//----------------------
	P1M1 = 0x00;    //双双双双双双双推
	P1M0 = 0x01;    //
	P1 = 0xFF;    //XTAL1,XTAL2,-,-,-,-,-,EN_LD
	//----------------------
//	P2M1 = 0x10;    //推推双开双双推双
//	P2M0 = 0xD2;
//	P2 = 0xFF;     //OUT_DTE,OUT_SCLK,IN_DATA1,AUX_LED,IN_OF2,IN_OF1,OUT_SETIN,A_MCU_M
 
	P2M1 = 0x00;    //双双双开双双双双
	P2M0 = 0x00;
	P2 = 0xFF;    //-,-,-,-,OUT_CONV1,OUT_SCLK1,OUT_DXMIT1,IN_DATA1
	//----------------------
	P3M1 = 0x00;    //双推推推双双双双
	P3M0 = 0x00;
	P3 = 0xFF;    //IN_VALID1,AD_RANGE0,AD_RANGE2,AD_RANGE1,AUX_LED,-,BM_TXD,BM_RXD
	//----------------------
	P4M1 = 0x00;    //双
	P4M0 = 0x00;
	P4 = 0xFF;    //1
	
	P5M1 = 0x00;    //双  双  双      双 双     双  双  双  双
	P5M0 = 0x00;
	P5 = 0xFF;    //---,---,A_DIR,A_RST,---,---,---,---

	//SFR---------------------------------------------------------------------------
	WDT_CONTR = 0x00;         //WDT未启动
	PCON = 0x00;
	SCON = 0x40; //RS232 MODE 1
	TMOD = 0x00; //T0,TIMER0,16位计时,T1 TIMER0,16位自动重装
	TCON = 0x00; //
	AUXR = 0xD5; //T0/T1/T2不分频,12T快速模式;T2为串口1的波特率发生器

    AUXR1 = 0x20;	//使用DPTR0,CCP在P24P25P26,串口1在P30/P31	
//	if(A_MCU_MOD)		//跳线帽接入使为核心板控制模式
//		AUXR1 = 0x20;	//使用DPTR0,CCP在P24P25P26,串口1在P30/P31
//	else
//		AUXR1 = 0x60;   //使用DPTR0,CCP在P24P25P26,串口1在P36/P37

	P_SW2 = 0x00; //
	P1ASF = 0x00; //无模拟输入
	ADC_CONTR = 0x00; //AD未启动
	P4SW = 0x70; //P46P45P44设为IO口
	WAKE_CLKO = 0x00; //不输出时钟
	CLK_DIV = 0x00; //时钟不分频,不输出主时钟
	IE = 0x00; //
	IE2 = 0x00; //
	IP0 = 0x10; //---PCA,LVD,ADC,UART,T1,EX1,T0,EX0,UART高优先级
	//-------------------------------------------------------------
//	T2H = 0xFD;           //9600波特率(24MHz,1T)
//	T2L = 0x8F;
	T2L = (65536 - (FOSC/4/BAUD));
	T2H = (65536 - (FOSC/4/BAUD))>>8;

    INT_CLKO |= 0x20;               //(EX3 = 1)使能INT3中断

	//初始化IO----DDC112
    range = IapReadByte(IAP_ADDRESS);
	AD_RANGE0 = 1;
	AD_RANGE1 = 0;
	AD_RANGE2 = 0;
//    AD_RANGE0 = !!(range&0x01);
//	AD_RANGE1 = !!(range&0x02);
//	AD_RANGE2 = !!(range&0x04);
	OUT_DXMIT1 = 1;
	OUT_SCLK1 = 0;

	//初始化IO----灯源
    EN_LD = 0;
}

//中断服务程序
void exint3() interrupt 11 using 2          //INT3中断入口
{
    man.dataValidFlag = 1;

    //AUX_LED = !AUX_LED;                     //将测试口取反
    
//  INT_CLKO &= 0xDF;               //若需要手动清除中断标志,可先关闭中断,此时系统会自动清除内部的中断标志
//  INT_CLKO |= 0x20;               //然后再开中断即可
}
