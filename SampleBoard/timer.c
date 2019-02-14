#include "timer.h"
#include "io.h"
#include "stddef.h"
#include "stepmotor.h"
#include "management.h"

//#define T500US (65536-FOSC/2000) //500us
//#define T1MS (65536-FOSC/1000)   //1ms timer calculation method in 12T mode
//#define T2MS (65536-FOSC/500)  //2ms

#define T1US    (0xFFF8) //1us
#define T10US   (0xFF10) //10us
#define T100US  (0xF6A0) //100us
#define T500US  (0xD120) //500us
#define T1MS    (0xA240) //1ms
#define T2MS    (0x4480) //1ms

//#define TIMEOUT T100US
#define TIMEOUT T500US  //定时器重装载值


uint16_t timerCount = 0;

//定时器0初始化
void Timer0_Init(void)
{
    TMOD |= 0x01;                    //set timer0 as mode1 (16-bit)
    TL0 = TIMEOUT;                   //initial timer0 low byte
    TH0 = TIMEOUT >> 8;              //initial timer0 high byte
    //TR0 = 1;                         //timer0 start running
	TF0 = 0;
    ET0 = 1;                         //enable timer0 interrupt
}

//定时器1初始化
//void Timer1_Init(void)
//{
//    //TMOD |= 0x10;                    //set timer1 as mode1 (16-bit)
//
//    TMOD &= ~0x40;                  //C/T1=0, 对内部时钟进行时钟输出
////  TMOD |= 0x40;                   //C/T1=1, 对T1引脚的外部时钟进行时钟输出
//
//	//AUXR &= ~0x40;
//	AUXR |= 0x40;
//
//    TL1 = timer1_TH_TL[stepMotorSpeedIndex][1];  //initial timer1 low byte
//    TH1 = timer1_TH_TL[stepMotorSpeedIndex][0];  //initial timer1 high byte
//    //TR1 = 1;                       //timer1 start running
//	TF1 = 0;
//    //ET1 = 1;                         //enable timer1 interrupt
//
//	//INT_CLKO = 0x02;				 //使能定时器1的时钟输出功能
//}

//-----------------------------------------------
/* Timer0 interrupt routine */
void Timer0_Isr() interrupt 1 using 1
{
#if CONV_PIN_REVERSE_TIMER

    static uint8_t cnt = 0;
//    static uint16_t ledCnt = 0;

    cnt++;
    if(cnt >= man.integralTime)
    {    
        OUT_CONV1 = !OUT_CONV1;
        //AUX_LED = !AUX_LED;
        cnt = 0;

//        ledCnt++;
    }

//    if(ledCnt > 100)
//    {
//        AUX_LED = !AUX_LED;
//        ledCnt = 0;
//    }

    //OUT_CONV1 = !OUT_CONV1;

#endif

    TL0 = TIMEOUT;                     //reload timer0 low byte
    TH0 = TIMEOUT >> 8;                //reload timer0 high byte

//	if(timerCount > 0)
//		timerCount--;
//	else
//		TR0 = 0;	
}

//-----------------------------------------------
/* Timer1 interrupt routine */
//void Timer1_Isr() interrupt 3 using 1
//{
////    TL1 = timer1_TH_TL[stepMotorSpeedIndex][1];  //initial timer1 low byte
////    TH1 = timer1_TH_TL[stepMotorSpeedIndex][0];  //initial timer1 high byte
//
//	if(stepMotorStep)
//		stepMotorStep--;
//	else
//	{
//		ET1 = 0;
//		TR1 = 0;
//	}
//
//	//AUX_LED = ~AUX_LED;
//}

//----------------------------------------------------------------
void Delay1us()		//@24.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 3;
	while (--i);
}

void Delay10us()		//@24.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 57;
	while (--i);
}

void Delay100us()		//@24.000MHz
{
	unsigned char i, j;

	i = 3;
	j = 82;
	do
	{
		while (--j);
	} while (--i);
}

void Delay500us()		//@24.000MHz
{
	unsigned char i, j;

	i = 12;
	j = 169;
	do
	{
		while (--j);
	} while (--i);
}

void Delay1ms()		//@24.000MHz
{
	unsigned char i, j;

	i = 24;
	j = 85;
	do
	{
		while (--j);
	} while (--i);
}

void Delay5ms()		//@24.000MHz
{
	unsigned char i, j;

	i = 117;
	j = 184;
	do
	{
		while (--j);
	} while (--i);
}

void Delay10ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 1;
	j = 234;
	k = 113;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Delay100ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 10;
	j = 31;
	k = 147;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Timer_Delay100ns(uint8_t n)
{	
	timerCount = n;
	TR0 = 1;
	while(timerCount);
}
