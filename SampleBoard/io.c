#include "io.h"
#include "uart.h"
#include "misc.h"
#include "timer.h"
#include "eeprom.h"
#include "management.h"

//IO��ʼ��
void IO_Init(void)
{
    uint8_t range = 0;
	//IO--------------------------------------------------------------------------------------
//	P0M1 = 0x00;    //˫˫˫˫˫������
//	P0M0 = 0x07;
//	P0 = 0xFE;    //-,-,-,-,IN_DATA2,OUT_RST,OUT_FDS,OUT_RSET
	//----------------------
	P1M1 = 0x00;    //˫˫˫˫˫˫˫��
	P1M0 = 0x01;    //
	P1 = 0xFF;    //XTAL1,XTAL2,-,-,-,-,-,EN_LD
	//----------------------
//	P2M1 = 0x10;    //����˫��˫˫��˫
//	P2M0 = 0xD2;
//	P2 = 0xFF;     //OUT_DTE,OUT_SCLK,IN_DATA1,AUX_LED,IN_OF2,IN_OF1,OUT_SETIN,A_MCU_M
 
	P2M1 = 0x00;    //˫˫˫��˫˫˫˫
	P2M0 = 0x00;
	P2 = 0xFF;    //-,-,-,-,OUT_CONV1,OUT_SCLK1,OUT_DXMIT1,IN_DATA1
	//----------------------
	P3M1 = 0x00;    //˫������˫˫˫˫
	P3M0 = 0x00;
	P3 = 0xFF;    //IN_VALID1,AD_RANGE0,AD_RANGE2,AD_RANGE1,AUX_LED,-,BM_TXD,BM_RXD
	//----------------------
	P4M1 = 0x00;    //˫
	P4M0 = 0x00;
	P4 = 0xFF;    //1
	
	P5M1 = 0x00;    //˫  ˫  ˫      ˫ ˫     ˫  ˫  ˫  ˫
	P5M0 = 0x00;
	P5 = 0xFF;    //---,---,A_DIR,A_RST,---,---,---,---

	//SFR---------------------------------------------------------------------------
	WDT_CONTR = 0x00;         //WDTδ����
	PCON = 0x00;
	SCON = 0x40; //RS232 MODE 1
	TMOD = 0x00; //T0,TIMER0,16λ��ʱ,T1 TIMER0,16λ�Զ���װ
	TCON = 0x00; //
	AUXR = 0xD5; //T0/T1/T2����Ƶ,12T����ģʽ;T2Ϊ����1�Ĳ����ʷ�����

    AUXR1 = 0x20;	//ʹ��DPTR0,CCP��P24P25P26,����1��P30/P31	
//	if(A_MCU_MOD)		//����ñ����ʹΪ���İ����ģʽ
//		AUXR1 = 0x20;	//ʹ��DPTR0,CCP��P24P25P26,����1��P30/P31
//	else
//		AUXR1 = 0x60;   //ʹ��DPTR0,CCP��P24P25P26,����1��P36/P37

	P_SW2 = 0x00; //
	P1ASF = 0x00; //��ģ������
	ADC_CONTR = 0x00; //ADδ����
	P4SW = 0x70; //P46P45P44��ΪIO��
	WAKE_CLKO = 0x00; //�����ʱ��
	CLK_DIV = 0x00; //ʱ�Ӳ���Ƶ,�������ʱ��
	IE = 0x00; //
	IE2 = 0x00; //
	IP0 = 0x10; //---PCA,LVD,ADC,UART,T1,EX1,T0,EX0,UART�����ȼ�
	//-------------------------------------------------------------
//	T2H = 0xFD;           //9600������(24MHz,1T)
//	T2L = 0x8F;
	T2L = (65536 - (FOSC/4/BAUD));
	T2H = (65536 - (FOSC/4/BAUD))>>8;

    INT_CLKO |= 0x20;               //(EX3 = 1)ʹ��INT3�ж�

	//��ʼ��IO----DDC112
    range = IapReadByte(IAP_ADDRESS);
	AD_RANGE0 = 1;
	AD_RANGE1 = 0;
	AD_RANGE2 = 0;
//    AD_RANGE0 = !!(range&0x01);
//	AD_RANGE1 = !!(range&0x02);
//	AD_RANGE2 = !!(range&0x04);
	OUT_DXMIT1 = 1;
	OUT_SCLK1 = 0;

	//��ʼ��IO----��Դ
    EN_LD = 0;
}

//�жϷ������
void exint3() interrupt 11 using 2          //INT3�ж����
{
    man.dataValidFlag = 1;

    //AUX_LED = !AUX_LED;                     //�����Կ�ȡ��
    
//  INT_CLKO &= 0xDF;               //����Ҫ�ֶ�����жϱ�־,���ȹر��ж�,��ʱϵͳ���Զ�����ڲ����жϱ�־
//  INT_CLKO |= 0x20;               //Ȼ���ٿ��жϼ���
}
