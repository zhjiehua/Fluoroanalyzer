#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//蜂鸣器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

#include "sys.h"
#include "StepMotor/Drv8825.h"
#include "StepMotor/StepMotor.h"
#include "Beep/beep.h"
#include "LED/LED.h"
#include "usart.h"
#include "PhotoelectricSensor/PhSensor.h"
#include "stdlib.h"
#include "CPrintf.h"
#include "../Logic/management.h"

#include "FreeRTOS.h"
#include "task.h"

static StepMotor_TypeDef stepMotor[STEPMOTOR_COUNT];
StepMotor_TypeDef *pStepMotor = stepMotor;

const StepMotorPin_TypeDef StepMotorPin[] = 
{
    {GPIOB, GPIO_Pin_0, TIM3, TIM_Channel_3, TIM3_IRQn, GPIOA, GPIO_Pin_8, GPIOA, GPIO_Pin_15},//"StepMotor1"
    {GPIOB, GPIO_Pin_1, TIM1, TIM_Channel_3, TIM1_UP_IRQn, GPIOB, GPIO_Pin_4, GPIOB, GPIO_Pin_3},//"StepMotor2"
};

//定位速度
const SpeedLevel_TypeDef speedLevel[] = {
	{STEPMOTOR_FREQ(0.09), 0.10*SPEED_CONST},   //0  0.10
	{STEPMOTOR_FREQ(0.15), 0.15*SPEED_CONST},
	{STEPMOTOR_FREQ(0.20), 0.20*SPEED_CONST},
	{STEPMOTOR_FREQ(0.25), 0.25*SPEED_CONST},
	{STEPMOTOR_FREQ(0.40), 0.40*SPEED_CONST},
	{STEPMOTOR_FREQ(0.60), 0.60*SPEED_CONST},	//5
	{STEPMOTOR_FREQ(0.80), 0.80*SPEED_CONST},
	{STEPMOTOR_FREQ(1.00), 1.00*SPEED_CONST},
	{STEPMOTOR_FREQ(1.20), 1.20*SPEED_CONST},
	{STEPMOTOR_FREQ(1.40), 1.40*SPEED_CONST},
	{STEPMOTOR_FREQ(1.60), 1.60*SPEED_CONST},	//10
	{STEPMOTOR_FREQ(1.80), 1.80*SPEED_CONST},
	{STEPMOTOR_FREQ(2.00), 2.00*SPEED_CONST},
	{STEPMOTOR_FREQ(2.20), 2.20*SPEED_CONST},
};

//设置步进电机方向
void StepMotor_SetDir(uint8_t num, Direction_TypeDef dir)
{
	if(dir == CW)
	{
		GPIO_ResetBits(StepMotorPin[num].DIR_GPIOx, StepMotorPin[num].DIR_GPIO_Pin);
		stepMotor[num].direction = CW;
	}
	else
	{
		GPIO_SetBits(StepMotorPin[num].DIR_GPIOx, StepMotorPin[num].DIR_GPIO_Pin);
		stepMotor[num].direction = CCW;
	}
}

//设置步进电机速度
void StepMotor_SetSpeed(uint8_t num, uint8_t speedIndex)
{
	stepMotor[num].desSpeedIndex = speedIndex;	 //设置目标速度
}

//设置步进电机脉冲数
//如果调用此命令则进入了脉冲计数模式
void StepMotor_SetPluse(uint8_t num, uint32_t pulseCount)
{
	uint8_t i;
	uint32_t decCount = 0;
	
	stepMotor[num].pulseCount = pulseCount;	 //设置脉冲数
	stepMotor[num].status |= 0x80;	//进入脉冲计数模式
	
	for(i=0;i<stepMotor[num].desSpeedIndex;i++)
	{
		decCount += stepMotor[num].pSpeedLevel[i].speedConst;
	}
	if(2*decCount > pulseCount)
	{
		stepMotor[num].pulseDec = pulseCount/3;
		stepMotor[num].desSpeedIndex--;
	}
	else
		stepMotor[num].pulseDec = pulseCount - decCount;	 //设置减速点
	
	//cDebug("pulseCount = %d\r\n", pulseCount);
	//cDebug("decCount = %d\r\n", decCount);
}

//设置步进电机命令
void StepMotor_SetCMD(uint8_t num, Status sta)
{
	if(sta == ENABLE)
	{
        if(((stepMotor[num].status)&0x01) == 0)
        {
            stepMotor[num].speedStatus = SPEED_ACC;  //加速
            stepMotor[num].curSpeedIndex = 0;  //起始速度为最小速度
			stepMotor[num].status |= 0x01;
            TIM_SetCounter(StepMotorPin[num].TIMx, stepMotor[num].pSpeedLevel[0].speed);
            TIM_SetCompare3(StepMotorPin[num].TIMx, (stepMotor[num].pSpeedLevel[0].speed)/2);
			GPIO_ResetBits(StepMotorPin[num].EN_GPIOx, StepMotorPin[num].EN_GPIO_Pin);
            TIM_Cmd(StepMotorPin[num].TIMx, ENABLE);//启动定时器
        }
		else if(stepMotor[num].curSpeedIndex <= stepMotor[num].desSpeedIndex)
        {
            stepMotor[num].speedStatus = SPEED_ACC;  //加速
        }
        else
        {
            stepMotor[num].speedStatus = SPEED_DEC;
        }		
	}
	else
	{
        if(((stepMotor[num].status)&0x01) != 0)
        {
            stepMotor[num].desSpeedIndex = 0;
            stepMotor[num].speedStatus = SPEED_DEC;  //减速，减速到最小速度
        }

        //TIM_Cmd(StepMotorPin[0].TIMx, ENABLE);
		//TR1 = 0;  //停止定时器1
		//pStepMotor->control = 0x00;
	}
}

//设置步进电机停止命令
void StepMotor_Stop(uint8_t num)
{
    //if((((stepMotor[num].status)&0x01) != 0) && stepMotor[num].speedStatus != SPEED_POSOFFSET)
	if((((stepMotor[num].status)&0x01) != 0) && stepMotor[num].speedStatus != SPEED_STOP)
        stepMotor[num].speedStatus = SPEED_STOP;
	
	//TF1 = 1; //进入定时器1中断		
}

//检查步进电机是否已经停止
//返回1,表示已经停止
uint8_t StepMotor_IsStop(uint8_t num)
{
	return !(stepMotor[num].status&0x01);
}

//步进电机初始化
void StepMotor_Init(void)
{
    /*********************初始化步进电机外设***************************************/
    uint8_t i;
    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO|RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    for(i=0;i<SIZEOF(StepMotorPin);i++)
    {
        GPIO_InitStructure.GPIO_Pin = StepMotorPin[i].EN_GPIO_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //速度为50MHz
        GPIO_Init(StepMotorPin[i].EN_GPIOx, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = StepMotorPin[i].DIR_GPIO_Pin;
        GPIO_Init(StepMotorPin[i].DIR_GPIOx, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = StepMotorPin[i].PWM_GPIO_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //速度为50MHz
        GPIO_Init(StepMotorPin[i].PWM_GPIOx, &GPIO_InitStructure);
        
        //STEPMOTOR_DEFAULT_PERIOD
        TIM_TimeBaseStructure.TIM_Period = STEPMOTOR_DEFAULT_PERIOD; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值,如72000000/900=80KHz
        TIM_TimeBaseStructure.TIM_Prescaler = 0; //设置用来作为TIMx时钟频率除数的预分频值 
        TIM_TimeBaseStructure.TIM_ClockDivision = 1; //设置时钟分割:TDTS = Tck_tim
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //TIM向上计数模式
        TIM_TimeBaseInit(StepMotorPin[i].TIMx, &TIM_TimeBaseStructure);
        
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2  TIM_OCMode_Timing
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
        TIM_OCInitStructure.TIM_Pulse = STEPMOTOR_DEFAULT_PERIOD/2; //设置待装入捕获比较寄存器的脉冲值
        
        switch(StepMotorPin[i].TIM_Channel)
        {
            case TIM_Channel_1:
                TIM_OC1Init(StepMotorPin[i].TIMx, &TIM_OCInitStructure);
                TIM_OC1PreloadConfig(StepMotorPin[i].TIMx, TIM_OCPreload_Enable);
                break;
            case TIM_Channel_2:
                TIM_OC2Init(StepMotorPin[i].TIMx, &TIM_OCInitStructure);
                TIM_OC2PreloadConfig(StepMotorPin[i].TIMx, TIM_OCPreload_Enable);
                break;
            case TIM_Channel_3:
                TIM_OC3Init(StepMotorPin[i].TIMx, &TIM_OCInitStructure);
                TIM_OC3PreloadConfig(StepMotorPin[i].TIMx, TIM_OCPreload_Enable);
                break;
            case TIM_Channel_4:
                TIM_OC4Init(StepMotorPin[i].TIMx, &TIM_OCInitStructure);
                TIM_OC4PreloadConfig(StepMotorPin[i].TIMx, TIM_OCPreload_Enable);
                break;
        }
        
		GPIO_SetBits(StepMotorPin[i].EN_GPIOx, StepMotorPin[i].EN_GPIO_Pin);
        //GPIO_ResetBits(StepMotorPin[i].EN_GPIOx, StepMotorPin[i].EN_GPIO_Pin);  //使能步进驱动器
        GPIO_SetBits(StepMotorPin[i].DIR_GPIOx, StepMotorPin[i].DIR_GPIO_Pin);
        
        NVIC_InitStructure.NVIC_IRQChannel = StepMotorPin[i].NVIC_IRQChannel;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
        NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
        
        TIM_ARRPreloadConfig(StepMotorPin[i].TIMx, ENABLE);
        TIM_ITConfig(StepMotorPin[i].TIMx, TIM_IT_Update, ENABLE);
        //TIM_Cmd(StepMotorPin[i].TIMx, ENABLE);
		TIM_Cmd(StepMotorPin[i].TIMx, DISABLE);
		
		/****************初始化步进电机结构体***********************************/
		stepMotor[i].offset = STEPMOTOR_OFFSET;
		//stepMotor[i].offset = pProjectMan->posCali1;
		stepMotor[i].speedStatus = SPEED_NONE;
		stepMotor[i].pSpeedLevel = speedLevel;
		stepMotor[i].curSpeedIndex = 0;
		stepMotor[i].desSpeedIndex = 0;
		stepMotor[i].status = 0;
		stepMotor[i].direction = CW;
		
		stepMotor[i].control = 0x00;
    }
    //不使用步进电机2中断
    TIM_ITConfig(StepMotorPin[1].TIMx, TIM_IT_Update, DISABLE);
    
//    for(i=0;i<SIZEOF(speedLevel);i++)
//    {
//        printf("speedLevel[%d].speed = %d\t\n", i, speedLevel[i].speed);
//        printf("speedLevel[%d].speedConst = %d\t\n", i, speedLevel[i].speedConst);
//    }
	
	//DRV8825初始化
	Drv8825_Init();
}

void TIM1_UP_IRQHandler(void)
{
    uint32_t oldBasePri = portSET_INTERRUPT_MASK_FROM_ISR();
    if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR(oldBasePri);
}

/* Timer5 interrupt routine */
void TIM3_IRQHandler(void)
{
	static uint16_t cnt = 0;
	static uint16_t offset = 0;
	static uint32_t pluseCount = 0;
    uint32_t oldBasePri = portSET_INTERRUPT_MASK_FROM_ISR();
    
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        switch(stepMotor[0].speedStatus)
        {				  
            case SPEED_ACC: //加速
            {
                cnt++;
                if(cnt >= stepMotor[0].pSpeedLevel[stepMotor[0].curSpeedIndex].speedConst)
                {
                    cnt = 0;
                    stepMotor[0].curSpeedIndex++;
                }
        
                if(stepMotor[0].curSpeedIndex >= stepMotor[0].desSpeedIndex) //加速完成
                {
                    stepMotor[0].curSpeedIndex = stepMotor[0].desSpeedIndex;
                    stepMotor[0].speedStatus = SPEED_NONE;
                }		
            }
            break;
            case SPEED_DEC: //减速
            {
                cnt++;
                if(cnt >= stepMotor[0].pSpeedLevel[stepMotor[0].curSpeedIndex].speedConst)
                {
                    cnt = 0;
                    stepMotor[0].curSpeedIndex--;
                }
        
                if(stepMotor[0].curSpeedIndex <= stepMotor[0].desSpeedIndex)  //减速到目标速度
                {
                    stepMotor[0].curSpeedIndex = stepMotor[0].desSpeedIndex;
                    stepMotor[0].speedStatus = SPEED_NONE;
                    
                    cnt = 0;
                    //TR1 = 0;  //停止定时器1
                    //TIM_Cmd(StepMotorPin[0].TIMx, DISABLE);
                }
            }
            break;
            case SPEED_STOP: 
            {
//                cnt++;
//                if(cnt >= stepMotor[0].pSpeedLevel[stepMotor[0].curSpeedIndex].speedConst)
//                {
//                    cnt = 0;
//                    stepMotor[0].curSpeedIndex--;
//                }
//        
//                if(stepMotor[0].curSpeedIndex <= 0)  //减速到最小速度
//                {
//                    stepMotor[0].curSpeedIndex = 0;			
//                    cnt = 0;
//                }
//                stepMotor[0].speedStatus = SPEED_POSOFFSET;
//            }
//            break;
//            case SPEED_POSOFFSET:
//            {
                offset++;
                if(offset++ >= stepMotor[0].offset)
                {
					offset = 0;
                    cnt = 0;
                    stepMotor[0].curSpeedIndex = 0;
                    stepMotor[0].speedStatus = SPEED_NONE;
					//stepMotor[0].status &= ~0x01;
					
					pluseCount = 0;
					stepMotor[0].status &= ~0x81; //停止电机，强制退出脉冲计数模式
					
                    //停止定时器1
					GPIO_SetBits(StepMotorPin[0].EN_GPIOx, StepMotorPin[0].EN_GPIO_Pin);
                    TIM_Cmd(TIM3, DISABLE);
                    //cDebug("interrupt---stop the timer5\n");
                    break;	
                }

                cnt++;
                if(cnt >= stepMotor[0].pSpeedLevel[stepMotor[0].curSpeedIndex].speedConst)
                {
                    cnt = 0;
                    stepMotor[0].curSpeedIndex--;
                }
        
                if(stepMotor[0].curSpeedIndex <= 0)  //减速到最小速度
                {
                    stepMotor[0].curSpeedIndex = 0;
                    cnt = 0;
                }
            }
            break;
            case SPEED_NONE:
            break;
            default:
            break;
        }

		if(stepMotor[0].status & 0x80)
		{
			pluseCount++;
			
			if(pluseCount >= stepMotor[0].pulseDec && stepMotor[0].speedStatus != SPEED_DEC)
			{
				stepMotor[0].desSpeedIndex = 0;
				stepMotor[0].speedStatus = SPEED_DEC;
				//cDebug("stepMotor[0].pulseDec = %d\r\n", stepMotor[0].pulseDec);
			}
			
			if(pluseCount >= stepMotor[0].pulseCount)
			{
				pluseCount = 0;
				offset = 0;
				cnt = 0;
				
				stepMotor[0].status &= ~0x81;
				stepMotor[0].curSpeedIndex = 0;
				stepMotor[0].speedStatus = SPEED_NONE;
				
				//停止定时器1
				GPIO_SetBits(StepMotorPin[0].EN_GPIOx, StepMotorPin[0].EN_GPIO_Pin);
				TIM_Cmd(TIM3, DISABLE);
			}
		}
        //更新定时器
        TIM_SetCounter(TIM3, stepMotor[0].pSpeedLevel[stepMotor[0].curSpeedIndex].speed);
        TIM_SetCompare3(TIM3, (stepMotor[0].pSpeedLevel[stepMotor[0].curSpeedIndex].speed)/2);
        
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR(oldBasePri);
}
